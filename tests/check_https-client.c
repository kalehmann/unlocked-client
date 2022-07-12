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

START_TEST(test_add_auth_header)
{
	struct Request request = {
		.body = "test",
		.port = 443,
		.secret = "1234",
		.skip_validation = 0,
		.url = "https://myserver/api/",
		.username = "myuser",
	};
	struct curl_slist *headers =
		curl_slist_append(NULL, "Date: Sun, 10 Jul 2022 09:41:29 GMT");
	struct curl_slist *header_iterator = NULL;
	unsigned int header_count = 0;

	headers = add_auth_header(headers, &request);
	header_iterator = headers;
	while (header_iterator) {
		header_count++;
		header_iterator = header_iterator->next;
	}
	ck_assert_uint_eq(2, header_count);
	// Signature is created by running
	// hmac.new(
	//   b"1234", b"Date: Sun, 10 Jul 2022 09:41:29 GMT\ntest", "SHA512"
	// ).hexdigest().upper()
	// in python
	ck_assert_str_eq("Authorization: hmac username=\"myuser\", "
			 "algorithm=\"sha512\", "
			 "headers=\"date\", "
			 "signature=\"0D137829150CFBE26CBCC48E4AE2EFA1C01CA1183"
			 "6DF65B5C3D6C2A7CF4C67203FD457A144D4E36B96CDE87A765DD6"
			 "D4C69E2213DB13D93FADF70DE560B6676B\"",
			 headers->next->data);
	curl_slist_free_all(headers);
}

START_TEST(test_add_date_header_with_existing_headers)
{
	struct curl_slist *headers = curl_slist_append(NULL, "X-TEST: test");
	struct curl_slist *header_iterator = NULL;
	unsigned int header_count = 0;

	headers = add_date_header(headers);
	header_iterator = headers;
	while (header_iterator) {
		header_count++;
		header_iterator = header_iterator->next;
	}
	ck_assert_uint_eq(2, header_count);
	ck_assert_mem_eq("Date: ", headers->next->data, 6);
	curl_slist_free_all(headers);
}

START_TEST(test_add_date_header_without_existing_headers)
{
	struct curl_slist *headers = NULL;
	struct curl_slist *header_iterator = NULL;
	unsigned int header_count = 0;

	headers = add_date_header(headers);
	header_iterator = headers;
	while (header_iterator) {
		header_count++;
		header_iterator = header_iterator->next;
	}
	ck_assert_uint_eq(1, header_count);
	ck_assert_mem_eq("Date: ", headers->data, 6);
	curl_slist_free_all(headers);
}

START_TEST(test_date_header)
{
	time_t now = 1657446089;
	char *header = date_header(&now);
	ck_assert_str_eq("Date: Sun, 10 Jul 2022 09:41:29 GMT", header);
	free(header);
}

// *INDENT-OFF*
END_TEST
// *INDENT-ON*

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

static TCase *make_https_client_add_auth_header_case(void)
{
	TCase *tc;

	tc = tcase_create("https-client::add_auth_header");
	tcase_add_test(tc, test_add_auth_header);

	return tc;
}

static TCase *make_https_client_add_date_header_case(void)
{
	TCase *tc;

	tc = tcase_create("https-client::add_date_header");
	tcase_add_test(tc, test_add_date_header_with_existing_headers);
	tcase_add_test(tc, test_add_date_header_without_existing_headers);

	return tc;
}

static TCase *make_https_client_date_header_case(void)
{
	TCase *tc;

	tc = tcase_create("https-client::date_header");
	tcase_add_test(tc, test_date_header);

	return tc;
}

static TCase *make_https_client_get_content_type_case(void)
{
	TCase *tc;

	tc = tcase_create("https-client::get_content_type");
	tcase_add_test(tc, test_get_content_type_no_header);
	tcase_add_test(tc, test_get_content_type_lowercase);
	tcase_add_test(tc, test_get_content_type_uppercase);

	return tc;
}

Suite *make_https_client_suite(void)
{
	Suite *s;

	s = suite_create("unlocked-client https-client");
	suite_add_tcase(s, make_https_client_add_auth_header_case());
	suite_add_tcase(s, make_https_client_add_date_header_case());
	suite_add_tcase(s, make_https_client_date_header_case());
	suite_add_tcase(s, make_https_client_get_content_type_case());

	return s;

}
