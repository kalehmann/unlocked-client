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

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "error.h"
#include "http-client.h"
#include <curl/curl.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>

static char * authHeader(struct curl_slist * headers, const char * username,
			 const char * key, const char * body);
static char * dateHeader(void);
static size_t header_callback(char *buffer, size_t size, size_t nitems,
			      void *userdata);
static const char * hmac_sha512(const char * const key,
				const char * const message);
static char * joinHeaderNames(struct curl_slist * header_list);
static void strToLower(char * str);
static size_t write_callback(char *ptr, size_t size, size_t nmemb,
			     void *userdata);

struct Response * create_response(void)
{
	struct Response * resp = malloc(sizeof(struct Response));
	if (NULL == resp) {
		return NULL;
	}
	resp->body = NULL;
	resp->body_len = 0;
	resp->headers = NULL;
	resp->status = 0;

	return resp;
}

void free_response(struct Response * response)
{
	if (NULL == response) {
		return;
	}
	if (response->body) {
		free(response->body);
	}
	curl_slist_free_all(response->headers);

	free(response);
}

enum unlocked_err init_https_client(void)
{
	CURLcode status;
	status = curl_global_init(CURL_GLOBAL_DEFAULT);
	if (CURLE_OK != status) {
		fprintf(stderr, "libcurl error: %s \n",
			curl_easy_strerror(status));

		return UL_CURL;
	}

	return UL_OK;
}

enum unlocked_err https_hmac_POST(const char * username, const char * secret,
				  const char * url, long port,
				  const char * body)
{
	CURL *curl;
	CURLcode status;
	struct Response * resp = NULL;
	struct curl_slist * headers = NULL;
	char * auth_header = NULL;
	char * date_header = dateHeader();
	if (NULL == date_header) {
		return UL_MALLOC;
	}
	curl = curl_easy_init();

	headers = curl_slist_append(headers, date_header);
	free(date_header);
	auth_header = authHeader(headers, username, secret, body);
	if (NULL == auth_header) {
		curl_slist_free_all(headers);

		return UL_MALLOC;
	}
	headers = curl_slist_append(headers, auth_header);
	free(auth_header);
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json; charsets: utf-8");

	if (!curl) {
		curl_slist_free_all(headers);

		return UL_CURL;
	}
	resp = create_response();
	if (NULL == resp) {
		curl_slist_free_all(headers);

		return UL_MALLOC;
	}
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(curl, CURLOPT_PORT, port);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, resp);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, resp);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

	status = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	curl_slist_free_all(headers);
	if (CURLE_OK != status) {
		free_response(resp);
		fprintf(stderr, "libcurl error: %s \n",
			curl_easy_strerror(status));

		return UL_CURL;
	}
	free_response(resp);

	return UL_OK;
}

void cleanup_https_client(void)
{
	curl_global_cleanup();
}

/**
 * Generates a header for HMAC based authentication.
 * The format of the header is
 * `Authorization: hmac username="<user>", algorithm="<algo>", "headers="<headers>", signature="<hex-digest>"`
 *
 * @param headers is the list with headers that should be included in the
 *                hashed message.
 * @param username is a handle that helps the server to identify the key that
 *                 was used to generate the hash.
 * @param key is the secret key used to generate the hash.
 * @param body is the data that should be included in the hashed message.
 *
 * @return a string with the header that must be freed after use.
 */
static char * authHeader(struct curl_slist * headers, const char * username,
			 const char * key, const char * body)
{
	const char * const auth_fmt = "Authorization: hmac username=\"%s\", "
		"algorithm=\"sha512\", headers=\"%s\", signature=\"%s\"";
	int auth_header_size = 0;
	char * auth_header = NULL;
	char * data_to_sign = NULL;
	size_t data_to_sign_size = strlen(body) + 1;
	const char * hex_digest = NULL;
	struct curl_slist * header_iterator = headers;
	char * header_names = joinHeaderNames(headers);
	if (NULL == header_names) {
		return NULL;
	}

	while (header_iterator) {
		data_to_sign_size += strlen(header_iterator->data) + 1;
		header_iterator = header_iterator->next;
	}

	data_to_sign = malloc(data_to_sign_size);
	if (NULL == data_to_sign) {
		free(header_names);

		return NULL;
	}
	data_to_sign[0] = '\0';
	header_iterator = headers;
	while (header_iterator) {
		strcat(data_to_sign, header_iterator->data);
		strcat(data_to_sign, "\n");
		header_iterator = header_iterator->next;
	}
	strcat(data_to_sign, body);
	hex_digest = hmac_sha512(key, data_to_sign);
	free(data_to_sign);

	auth_header_size = snprintf(NULL, 0, auth_fmt, username, header_names,
				    hex_digest);
	if (0 > auth_header_size) {
		free(header_names);

		return NULL;
	}
	auth_header_size += 1;
	auth_header = malloc(auth_header_size);
	if (NULL == auth_header) {
		free(header_names);

		return NULL;
	}
	if (0 > snprintf(auth_header, auth_header_size, auth_fmt, username,
			 header_names, hex_digest)) {
		free(auth_header);
		free(header_names);

		return NULL;
	}
	free(header_names);

	return auth_header;
}

/**
 * Generates the date header with the current date formatted according to
 * RFC7231.
 *
 * @return char * a pointer to the date header, that MUST be freed after use.
 */
static char * dateHeader(void)
{
	char * header = NULL;
	static const char * const header_fmt =
		"Date: %s, %02d %s %d %02d:%02d:%02d GMT";
	int header_length = 0;
	static const char * const days[] = {"Mon", "Tue", "Wed", "Thu", "Fri",
			"Sat", "Sun"};
	static const char * const months[] = {"Jan", "Feb", "Mar", "Apr", "May",
			"Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	time_t epoch = time(NULL);
	struct tm *tm = gmtime(&epoch);

	header_length = snprintf(NULL, 0, header_fmt, days[tm->tm_wday],
				 tm->tm_mday, months[tm->tm_mon],
				 tm->tm_year + 1900, tm->tm_hour, tm->tm_min,
				 tm->tm_sec);
	if (0 > header_length) {
		return NULL;
	}
	header = malloc(header_length + 1);
	if (NULL == header) {
		return NULL;
	}

	if (0 > snprintf(header, header_length + 1, header_fmt,
			 days[tm->tm_wday], tm->tm_mday, months[tm->tm_mon],
			 tm->tm_year + 1900, tm->tm_hour, tm->tm_min,
			 tm->tm_sec)) {
		free(header);

		return NULL;
	}

	return header;
}

/**
 *
 */
static size_t header_callback(char *buffer, size_t size, size_t nitems,
			      void *userdata)
{
	size_t length = size * nitems;
	char * header = NULL;
	struct Response * resp = userdata;

	if (NULL == memchr(buffer, ':', length)) {
		// No header
		return length;
	}
	header = malloc(length + 1);
	if (NULL == header) {
		return 0;
	}
	memcpy(header, buffer, length);
	header[length] = '\0';
	resp->headers = curl_slist_append(resp->headers, header);
	free(header);

	return length;
}

/**
 * Generates a HMAC using SHA512.
 *
 * @param key is the secret key.
 * @param message is the message to be authenticated.
 *
 * @return a static string with the hexadecimal hash digest.
 *         This string is 128 characters long (and an additional zero byte).
 *         It must not be freed.
 */
static const char * hmac_sha512(const char * const key,
				const char * const message)
{
	const unsigned char * digest = NULL;
	static char hex_digest[129] = { 0 };

	digest = HMAC(EVP_sha512(), key, strlen(key),
		      (unsigned char *)message, strlen(message),
		      NULL, NULL);
	for (int i = 0; i < 64; i++) {
		sprintf(hex_digest + i * 2, "%02X", digest[i]);
	}

	return hex_digest;
}

/**
 * Joins the names of headers separated by single spaces.
 *
 * @param header_list list of headers in the format
 *                    `<header-name>: <header>`.
 *
 * @return the header names joined by single spaces.
 */
static char * joinHeaderNames(struct curl_slist * header_list)
{
	const char * separator = NULL;
	struct curl_slist * header_iterator = header_list;
	char * headers = NULL;
	size_t headers_size = 1;
	int offset = 0;

	while (header_iterator) {
		separator = strstr(header_iterator->data, ":");
		if (NULL == separator) {
			return NULL;
		}
		headers_size += separator - header_iterator->data;
		header_iterator = header_iterator->next;
		if (header_iterator) {
			headers_size += 1;
		}
	}
	header_iterator = header_list;
	headers = malloc(headers_size);
	if (NULL == headers) {
		return NULL;
	}
	while (header_iterator) {
		separator = strstr(header_iterator->data, ":");
		memcpy(headers + offset, header_iterator->data,
		       separator - header_iterator->data);
		offset += separator - header_iterator->data;
		header_iterator = header_iterator->next;
		if (header_iterator) {
			headers[offset] = ' ';
		} else {
			headers[offset] = '\0';
		}
		offset += 1;
	}
	strToLower(headers);

	return headers;
}

/**
 * Converts a string (inplace) to lower case.
 *
 * @param str the string to convert to lower case.
 */
static void strToLower(char * str)
{
	for (int i = 0; str[i]; i++) {
		str[i] = tolower(str[i]);
	}
}

/**
 *
 */
static size_t write_callback(char *ptr, size_t size, size_t nmemb,
			     void *userdata)
{
	size_t length = size * nmemb;
	struct Response * resp = userdata;

	if (NULL == resp->body) {
		resp->body = malloc(length + 2);
		if (NULL == resp->body) {
			return 0;
		}
	} else {
		resp->body = realloc(resp->body,
				     resp->body_len + length + 2);
		if (NULL == resp->body) {
			return 0;
		}
	}
	memcpy(resp->body + resp->body_len, ptr, length);
	resp->body_len += length;
	resp->body[resp->body_len] = '\n';
	resp->body[resp->body_len + 1] = '\0';

	return length;

}
