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
#include "cJSON.h"
#include "client.h"
#include "https-client.h"
#include "log.h"

static char * get_key_request_body(const char * const handle);
static char * get_key_request_url(const char * const host);
static int get_request_id(struct Response * response);

void request_key(struct arguments * arguments)
{
	struct Request request = { 0 };
	struct Response * response = create_response();
	char * content_type = NULL;
	int request_id = 0;

	request.body = get_key_request_body(arguments->key_handle);
	if (NULL == request.body) {
		return;
	}
	request.port = arguments->port;
	request.secret = arguments->secret;
	request.skip_validation = arguments->no_validation;
	request.url = get_key_request_url(arguments->host);
	if (NULL == request.body) {
		free(request.body);

		return;
	}
	request.username = arguments->username;

	init_https_client();
	https_hmac_POST(&request, response);
	content_type = get_content_type(response);
	if (NULL == content_type) {
		logger(LOG_WARNING,
		       "No content-type given. Guessing application/json\n");
	} else {
		if (0 != strcmp(content_type, "application/json")) {
			logger(LOG_WARNING,
			       "Expected content-type is application/json, "
			       "got \"%s\"\n", content_type);
		}
		free(content_type);
	}
	request_id = get_request_id(response);
	printf("ID: %d\n", request_id);
	printf(response->body);
	cleanup_https_client();
	free_response(response);
	free(request.body);
	free(request.url);
}

/**
 * Get the body of the json request used to request access to a key.
 *
 * @param handle is the handle of the key that should be request.
 *
 * @return the body of the request that must be freed after use or NULL on
 *         failure.
 */
static char * get_key_request_body(const char * const handle)
{
	char * body = NULL;
	long body_len = 0;
	static const char * const fmt = "{\"key\": \"%s\"}";

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
static char * get_key_request_url(const char * const host)
{
	static const char * const fmt = "https://%s/api/requests";
	char * url = NULL;
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
static int get_request_id(struct Response * response)
{
	cJSON * body_json = NULL, * id_elem = NULL;
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
