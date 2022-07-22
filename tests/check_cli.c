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

#include "check_cli.h"
#include "../src/cli.h"

START_TEST(test_config_file_is_not_merged_when_empty)
{
	static char *base_config = "test";

	struct arguments base = {
		.config_file = base_config,
	};
	struct arguments cli = { 0 };

	merge_config(&base, &cli);

	ck_assert_ptr_eq(base_config, base.config_file);
}
// *INDENT-OFF*
END_TEST
// *INDENT-ON*

START_TEST(test_config_file_is_merged)
{
	static char *base_config = "test";
	static char *cli_config = "new";

	struct arguments base = {
		.config_file = base_config,
	};
	struct arguments cli = {
		.config_file = cli_config,
	};

	merge_config(&base, &cli);

	ck_assert_ptr_eq(cli_config, base.config_file);
}
// *INDENT-OFF*
END_TEST
// *INDENT-ON*

START_TEST(test_key_handle_is_not_merged_when_empty)
{
	static char *base_handle = "test";

	struct arguments base = {
		.key_handle = base_handle,
	};
	struct arguments cli = { 0 };

	merge_config(&base, &cli);

	ck_assert_ptr_eq(base_handle, base.key_handle);
}
// *INDENT-OFF*
END_TEST
// *INDENT-ON*

START_TEST(test_key_handle_is_merged)
{
	static char *base_handle = "test";
	static char *cli_handle = "new";

	struct arguments base = {
		.key_handle = base_handle,
	};
	struct arguments cli = {
		.key_handle = cli_handle,
	};

	merge_config(&base, &cli);

	ck_assert_ptr_eq(cli_handle, base.key_handle);
}
// *INDENT-OFF*
END_TEST
// *INDENT-ON*

START_TEST(test_host_is_not_merged_when_empty)
{
	static char *base_host = "test";

	struct arguments base = {
		.host = base_host,
	};
	struct arguments cli = { 0 };

	merge_config(&base, &cli);

	ck_assert_ptr_eq(base_host, base.host);
}
// *INDENT-OFF*
END_TEST
// *INDENT-ON*

START_TEST(test_host_is_merged)
{
	static char *base_host = "test";
	static char *cli_host = "new";

	struct arguments base = {
		.host = base_host,
	};
	struct arguments cli = {
		.host = cli_host,
	};

	merge_config(&base, &cli);

	ck_assert_ptr_eq(cli_host, base.host);
}
// *INDENT-OFF*
END_TEST
// *INDENT-ON*

START_TEST(test_port_is_not_merged_when_empty)
{
	static long base_port = 443;

	struct arguments base = {
		.port = base_port,
	};
	struct arguments cli = { 0 };

	merge_config(&base, &cli);

	ck_assert_int_eq(base_port, base.port);
}
// *INDENT-OFF*
END_TEST
// *INDENT-ON*

START_TEST(test_port_is_merged)
{
	static long base_port = 443;
	static long cli_port = 8443;

	struct arguments base = {
		.port = base_port,
	};
	struct arguments cli = {
		.port = cli_port,
	};

	merge_config(&base, &cli);

	ck_assert_int_eq(cli_port, base.port);
}
// *INDENT-OFF*
END_TEST
// *INDENT-ON*

START_TEST(test_secret_is_not_merged_when_empty)
{
	static char *base_secret = "test";

	struct arguments base = {
		.secret = base_secret,
	};
	struct arguments cli = { 0 };

	merge_config(&base, &cli);

	ck_assert_ptr_eq(base_secret, base.secret);
}
// *INDENT-OFF*
END_TEST
// *INDENT-ON*

START_TEST(test_secret_is_merged)
{
	static char *base_secret = "test";
	static char *cli_secret = "new";

	struct arguments base = {
		.secret = base_secret,
	};
	struct arguments cli = {
		.secret = cli_secret,
	};

	merge_config(&base, &cli);

	ck_assert_ptr_eq(cli_secret, base.secret);
}
// *INDENT-OFF*
END_TEST
// *INDENT-ON*

START_TEST(test_username_is_not_merged_when_empty)
{
	static char *base_username = "test";

	struct arguments base = {
		.username = base_username,
	};
	struct arguments cli = { 0 };

	merge_config(&base, &cli);

	ck_assert_ptr_eq(base_username, base.username);
}
// *INDENT-OFF*
END_TEST
// *INDENT-ON*

START_TEST(test_username_is_merged)
{
	static char *base_username = "test";
	static char *cli_username = "new";

	struct arguments base = {
		.username = base_username,
	};
	struct arguments cli = {
		.username = cli_username,
	};

	merge_config(&base, &cli);

	ck_assert_ptr_eq(cli_username, base.username);
}
// *INDENT-OFF*
END_TEST
// *INDENT-ON*

START_TEST(test_validation_is_not_merged_when_empty)
{
	static int base_validation = VALIDATE;

	struct arguments base = {
		.validate = base_validation,
	};
	struct arguments cli = { 0 };

	merge_config(&base, &cli);

	ck_assert_int_eq(base_validation, base.validate);
}
// *INDENT-OFF*
END_TEST
// *INDENT-ON*

START_TEST(test_validation_is_merged)
{
	static int base_validation = VALIDATE;
	static int cli_validation = SKIP_VALIDATION;

	struct arguments base = {
		.validate = base_validation,
	};
	struct arguments cli = {
		.validate = cli_validation,
	};

	merge_config(&base, &cli);

	ck_assert_int_eq(cli_validation, base.validate);
}
// *INDENT-OFF*
END_TEST
// *INDENT-ON*


static TCase *make_cli_merge_config_case(void)
{
	TCase *tc;

	tc = tcase_create("cli::merge_config");
	tcase_add_test(tc, test_config_file_is_not_merged_when_empty);
	tcase_add_test(tc, test_config_file_is_merged);
	tcase_add_test(tc, test_key_handle_is_not_merged_when_empty);
	tcase_add_test(tc, test_key_handle_is_merged);
	tcase_add_test(tc, test_host_is_not_merged_when_empty);
	tcase_add_test(tc, test_host_is_merged);
	tcase_add_test(tc, test_port_is_not_merged_when_empty);
	tcase_add_test(tc, test_port_is_merged);
	tcase_add_test(tc, test_secret_is_not_merged_when_empty);
	tcase_add_test(tc, test_secret_is_merged);
	tcase_add_test(tc, test_username_is_not_merged_when_empty);
	tcase_add_test(tc, test_username_is_merged);
	tcase_add_test(tc, test_validation_is_not_merged_when_empty);
	tcase_add_test(tc, test_validation_is_merged);

	return tc;
}

Suite *make_cli_suite(void)
{
	Suite *s;

	s = suite_create("unlocked-client cli");
	suite_add_tcase(s, make_cli_merge_config_case());

	return s;
}
