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

#ifndef UNLOCKED_HTTP_CLIENT_H
#define UNLOCKED_HTTP_CLIENT_H

#include <curl/curl.h>
#include "error.h"

struct Request {
	char *body;
	long port;
	char *secret;
	int skip_validation;
	char *url;
	char *username;
};

struct Response {
	char *body;
	size_t body_len;
	struct curl_slist *headers;
	long status;
};

/**
 *
 */
struct Response *create_response(void);

/**
 *
 */
void free_response(struct Response *response);

/**
 * Get the content type of a response.
 *
 * @param response is the response to get the content type for.
 *
 * @return the content type of the response or NULL if the content type is not
 *         set. If the return value is not NULL, it must be freed after use.
 *         The returned string will be all lower case and zero terminated.
 */
char *get_content_type(struct Response *response);

/**
 * Initializes the http client.
 *
 * @return a status code indicating any error that occured.
 */
enum unlocked_err init_https_client(void);

/**
 *
 */
enum unlocked_err https_hmac_GET(struct Request *request,
				 struct Response *response);

/**
 *
 */
enum unlocked_err https_hmac_PATCH(struct Request *request,
				   struct Response *response);

/**
 *
 */
enum unlocked_err https_hmac_POST(struct Request *request,
				  struct Response *response);

/**
 * Cleanup after the http client.
 */
void cleanup_https_client(void);

#endif
