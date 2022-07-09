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
#include <curl/curl.h>

#include "check_https-client.h"
#include "../src/https-client.h"

START_TEST(test_get_content_type_no_header)
{
	struct Response *response = create_response();
	char *content_type = NULL;

	response->status = 200;
	response->headers =
		curl_slist_append(response->headers,
				  "access-control-allow-origin: *\r\n");
	response->headers =
		curl_slist_append(response->headers,
				  "date: Sat, 09 Jul 2022 23:03:45 GMT\r\n");
	content_type = get_content_type(response);
	ck_assert_ptr_null(content_type);
	free_response(response);
}

// *INDENT-OFF*
END_TEST
// *INDENT-ON*

START_TEST(test_get_content_type_lowercase)
{
	struct Response *response = create_response();
	char *content_type = NULL;

	response->status = 200;
	response->headers =
		curl_slist_append(response->headers,
				  "access-control-allow-origin: *\r\n");
	response->headers =
		curl_slist_append(response->headers,
				  "content-type: text/html; charset=utf-8\r\n");
	response->headers =
		curl_slist_append(response->headers,
				  "date: Sat, 09 Jul 2022 23:03:45 GMT\r\n");
	content_type = get_content_type(response);
	ck_assert_str_eq("text/html; charset=utf-8", content_type);
	free_response(response);
	free(content_type);
}
// *INDENT-OFF*
END_TEST
// *INDENT-ON*

START_TEST(test_get_content_type_uppercase)
{
	struct Response *response = create_response();
	char *content_type = NULL;

	response->status = 200;
	response->headers =
		curl_slist_append(response->headers,
				  "access-control-allow-origin: *\r\n");
	response->headers =
		curl_slist_append(response->headers,
				  "CONTENT-TYPE: application/json\r\n");
	response->headers =
		curl_slist_append(response->headers,
				  "date: Sat, 09 Jul 2022 23:03:45 GMT\r\n");
	content_type = get_content_type(response);
	ck_assert_str_eq("application/json", content_type);
	free_response(response);
	free(content_type);
}
// *INDENT-OFF*
END_TEST
// *INDENT-ON*

TCase *https_client_create_case(void)
{
	TCase *tc_https_client;

	tc_https_client = tcase_create("unlocked-client https_client");
	tcase_add_test(tc_https_client, test_get_content_type_no_header);
	tcase_add_test(tc_https_client, test_get_content_type_lowercase);
	tcase_add_test(tc_https_client, test_get_content_type_uppercase);

	return tc_https_client;
}
