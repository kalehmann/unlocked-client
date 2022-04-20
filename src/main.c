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

#include "cli.h"
#include "client.h"
#include "http-client.h"

const char *argp_program_version =
  "unlocked-client dev";
const char *argp_program_bug_address =
  "<mail@kalehmann.de>";

int main(int argc, char **argv)
{
	struct arguments arguments = {
		.no_validation = 0,
		.port = 443,
	};
	struct Request request = { 0 };
	struct Response * response = create_response();
	char * body = NULL;
	long body_len = 0;
	char * url = NULL;
	long url_len = 0;

	handle_args(argc, argv, &arguments);
	if (NULL == arguments.host) {
		fprintf(stderr, "No hostname given\n");

		return EXIT_FAILURE;
	}
	if (0 == arguments.port) {
		fprintf(stderr, "Invalid port given\n");

		return EXIT_FAILURE;
	}
	if (NULL == arguments.secret) {
		fprintf(stderr, "No secret given\n");

		return EXIT_FAILURE;
	}
	if (NULL == arguments.username) {
		fprintf(stderr, "No username given\n");

		return EXIT_FAILURE;
	}

	body_len = snprintf(NULL, 0, "{\"key\": \"%s\"}", arguments.key_handle);
	body = malloc(body_len + 1);
	if (NULL == body) {
		return EXIT_FAILURE;
	}
	if (0 > snprintf(body, body_len + 1, "{\"key\": \"%s\"}",
			 arguments.key_handle)) {
		free(body);

		return EXIT_FAILURE;
	}
	url_len = snprintf(NULL, 0, "https://%s/api/requests", arguments.host);
	url = malloc(url_len + 1);
	if (NULL == url) {
		return EXIT_FAILURE;
	}
	if (0 > snprintf(url, url_len + 1, "https://%s/api/requests",
			 arguments.host)) {
		free(body);
		free(url);

		return EXIT_FAILURE;
	}
	request.body = body;
	request.port = arguments.port;
	request.secret = arguments.secret;
	request.skip_validation = arguments.no_validation;
	request.url = url;
	request.username = arguments.username;

	init_https_client();
	https_hmac_POST(&request, response);
	printf(response->body);
	cleanup_https_client();
	//init_socket();
	//getKey(&socket_callback);
	//cleanup_socket();
	free_response(response);
	free(body);
	free(url);

	return EXIT_SUCCESS;
}
