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
#include "client.h"
#include "https-client.h"
#include "log.h"

void request_key(struct arguments * arguments)
{
	struct Request request = { 0 };
	struct Response * response = create_response();
	char * body = NULL;
	long body_len = 0;
	char * content_type = NULL;
	char * url = NULL;
	long url_len = 0;

	body_len = snprintf(NULL, 0, "{\"key\": \"%s\"}", arguments->key_handle);
	body = malloc(body_len + 1);
	if (NULL == body) {
		return;
	}
	if (0 > snprintf(body, body_len + 1, "{\"key\": \"%s\"}",
			 arguments->key_handle)) {
		free(body);

		return;
	}
	url_len = snprintf(NULL, 0, "https://%s/api/requests", arguments->host);
	url = malloc(url_len + 1);
	if (NULL == url) {
		return;
	}
	if (0 > snprintf(url, url_len + 1, "https://%s/api/requests",
			 arguments->host)) {
		free(body);
		free(url);

		return;
	}
	request.body = body;
	request.port = arguments->port;
	request.secret = arguments->secret;
	request.skip_validation = arguments->no_validation;
	request.url = url;
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
	printf(response->body);
	cleanup_https_client();
	free_response(response);
	free(body);
	free(url);
}
