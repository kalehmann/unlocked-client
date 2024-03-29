// Copyright 2022 by Karsten Lehmann <mail@kalehmann.de>

/*
 * This file is part of unlocked-client.
 *
 * unlocked-client is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cJSON.h"
#include "client.h"
#include "error.h"
#include "https-client.h"
#include "log.h"
#include "mod/module.h"

static char *get_key_request_body(const char *const handle);
static char *get_key_request_url(const char *const host);
static int get_request_id(struct Response *response);
static char *get_request_state(struct Response *response);
static char *get_show_request_url(const char *const host, int id);
static void validate_content_type(struct Response *response);

enum unlocked_err request_key(struct arguments *arguments)
{
	struct Request request = { 0 };
	struct Response *response = create_response();
	int request_id = 0;
	char *key = NULL;
	char *request_state = NULL;
	enum unlocked_err err = UL_OK;

	logger(LOG_DEBUG, "Requesting the key \"%s\" from the host \"%s\"\n",
	       arguments->key_handle, arguments->host);

	request.body = get_key_request_body(arguments->key_handle);
	if (NULL == request.body) {
		return UL_MALLOC;
	}
	request.port = arguments->port;
	request.secret = arguments->secret;
	request.skip_validation = no == arguments->validate;
	request.url = get_key_request_url(arguments->host);
	if (NULL == request.url) {
		free(request.body);

		return UL_MALLOC;
	}
	request.username = arguments->username;

	init_https_client();
	https_hmac_POST(&request, response);
	free(request.body);
	request.body = NULL;
	free(request.url);
	request.url = NULL;
	switch (response->status) {
	case 201:
		// Expected response status, continue
		break;
	case 401:
		// Authentication failed
		logger(LOG_ERROR,
		       "Authentication against the server failed. "
		       "Maybe the user or secret are incorrect?\n");
		free_response(response);

		return UL_ERR;
	default:
		logger(LOG_ERROR,
		       "Communication with the server failed with code %ld.\n",
		       response->status);
		free_response(response);

		return UL_ERR;
	}
	validate_content_type(response);
	request_id = get_request_id(response);
	free_response(response);

	request.url = get_show_request_url(arguments->host, request_id);
	if (NULL == request.url) {
		free(request.url);

		return UL_MALLOC;
	}
	do {
		if (request_state) {
			free(request_state);
			request_state = NULL;
			// Add a sane delay until the next request.
			sleep(1);
		}
		response = create_response();
		https_hmac_GET(&request, response);
		request_state = get_request_state(response);
		free_response(response);
		response = NULL;
	} while (request_state && 0 == strcmp(request_state, "PENDING"));
	free(request.url);
	request.url = NULL;
	if (NULL == request_state) {
		logger(LOG_ERROR, "Could not determine state of request %d\n",
		       request_id);
		cleanup_https_client();

		return UL_ERR;
	}
	if (0 == strcmp(request_state, "DENIED")) {
		free(request_state);

		return UL_DENIED;
	}
	if (strcmp(request_state, "ACCEPTED")) {
		logger(LOG_ERROR, "Unexpected state for request %d: \"%s\"\n",
		       request_id, request_state);
		free(request_state);

		return UL_ERR;
	}
	free(request_state);
	response = create_response();
	request.body = "{\"state\": \"FULFILLED\"}";
	request.url = get_show_request_url(arguments->host, request_id);
	if (NULL == request.url) {
		return UL_MALLOC;
	}
	https_hmac_PATCH(&request, response);
	free(request.url);
	cleanup_https_client();
	key = strdup(response->body);
	free_response(response);
	if (NULL == key) {
		return UL_MALLOC;
	}
	// Override the newline character at the end of the key with a null
	// terminator.
	key[strlen(key) - 1] = '\0';
	err = handle_success(key);
	free(key);

	return err;
}

/**
 * Get the body of the json request used to request access to a key.
 *
 * @param handle is the handle of the key that should be request.
 *
 * @return the body of the request that must be freed after use or NULL on
 *         failure.
 */
static char *get_key_request_body(const char *const handle)
{
	char *body = NULL;
	long body_len = 0;
	static const char *const fmt = "{\"key\": \"%s\"}";

	body_len = snprintf(NULL, 0, fmt, handle);
	body = malloc(body_len + 1);
	if (NULL == body) {
		return NULL;
	}
	if (0 > snprintf(body, body_len + 1, fmt, handle)) {
		free(body);

		return NULL;
	}

	return body;
}

/**
 * Get the url for the endpoint used to request access to a key.
 *
 * @param host is the hostname of the server.
 *
 * @return the url including the protocol or NULL on failure.
 *         This value must be freed after use.
 */
static char *get_key_request_url(const char *const host)
{
	static const char *const fmt = "https://%s/api/requests";
	char *url = NULL;
	long url_len = 0;

	url_len = snprintf(NULL, 0, fmt, host);
	url = malloc(url_len + 1);
	if (NULL == url) {
		return NULL;
	}
	if (0 > snprintf(url, url_len + 1, fmt, host)) {
		free(url);

		return NULL;
	}

	return url;
}

/**
 * Extract the id of the new request for a key from a
 * response from the server.
 *
 * @param response the response with the serialized request.
 *
 * @return int the id of the request or 0 on failure.
 */
static int get_request_id(struct Response *response)
{
	cJSON *body_json = NULL, *id_elem = NULL;
	int id = 0;

	if (NULL == response) {
		return id;
	}

	body_json = cJSON_Parse(response->body);
	if (NULL == body_json) {
		const char *error_ptr = cJSON_GetErrorPtr();
		if (error_ptr != NULL) {
			logger(LOG_ERROR, "Error before: %s\n", error_ptr);

			return id;
		}
		logger(LOG_ERROR, "Error parsing response json\n");

		return id;
	}
	id_elem = cJSON_GetObjectItemCaseSensitive(body_json, "id");
	if (NULL == id_elem) {
		logger(LOG_ERROR, "Key \"id\" not found\n");
		cJSON_Delete(body_json);

		return id;
	}

	if (0 == cJSON_IsNumber(id_elem)) {
		logger(LOG_ERROR, "Value of key \"id\" is not numeric\n");
		cJSON_Delete(body_json);

		return id;
	}
	id = id_elem->valuedouble;
	cJSON_Delete(body_json);

	return id;
}

/**
 * Extract the id of the new request for a key from a
 * response from the server.
 *
 * @param response the response with the serialized request.
 *
 * @return a pointer to a string withthe request state, that must be freed after
 *         use or NULL on failure.
 */
static char *get_request_state(struct Response *response)
{
	cJSON *body_json = NULL, *state_elem = NULL;
	char *state = NULL;
	size_t state_len = 0;

	if (NULL == response) {
		return state;
	}

	body_json = cJSON_Parse(response->body);
	if (NULL == body_json) {
		const char *error_ptr = cJSON_GetErrorPtr();
		if (error_ptr != NULL) {
			logger(LOG_ERROR, "Error before: %s\n", error_ptr);

			return state;
		}
		logger(LOG_ERROR, "Error parsing response json\n");

		return state;
	}
	state_elem = cJSON_GetObjectItemCaseSensitive(body_json, "state");
	if (NULL == state_elem) {
		logger(LOG_ERROR, "Key \"state\" not found\n");
		cJSON_Delete(body_json);

		return state;
	}

	if (0 == cJSON_IsString(state_elem) || NULL == state_elem->valuestring) {
		logger(LOG_ERROR, "Value of key \"state\" is not a string\n");
		cJSON_Delete(body_json);

		return state;
	}
	state_len = strlen(state_elem->valuestring);
	state = malloc(state_len + 1);
	if (NULL == state) {
		cJSON_Delete(body_json);

		return state;
	}
	memcpy(state, state_elem->valuestring, state_len);
	state[state_len] = '\0';
	cJSON_Delete(body_json);

	return state;
}

/**
 * Get the url for the endpoint used to show a request for a key.
 *
 * @param host is the hostname of the server.
 * @param id is the id of the request to show.
 *
 * @return the url including the protocol or NULL on failure.
 *         This value must be freed after use.
 */
static char *get_show_request_url(const char *const host, int id)
{
	static const char *const fmt = "https://%s/api/requests/%d";
	char *url = NULL;
	long url_len = 0;

	url_len = snprintf(NULL, 0, fmt, host, id);
	url = malloc(url_len + 1);
	if (NULL == url) {
		return NULL;
	}
	if (0 > snprintf(url, url_len + 1, fmt, host, id)) {
		free(url);

		return NULL;
	}

	return url;
}

/**
 * Log an error when the content type of the response is not
 * "application/json".
 *
 * @param response is the response to check.
 */
static void validate_content_type(struct Response *response)
{
	char *content_type = NULL;

	if (NULL == response) {
		return;
	}

	content_type = get_content_type(response);
	if (NULL == content_type) {
		logger(LOG_WARNING,
		       "No content-type given. "
		       "Guessing \"application/json\"\n");
	} else {
		if (0 != strcmp(content_type, "application/json")) {
			logger(LOG_WARNING,
			       "Expected content-type is \"application/json\""
			       ", got \"%s\"\n", content_type);
		}
	}

	free(content_type);
}
