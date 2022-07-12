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

#include "check_module.h"
#include "../../src/mod/module.h"

static int failure_called = 0;
static const char *success_key = NULL;

static enum unlocked_err test_mod_success(const char *const key)
{
	success_key = key;

	return UL_OK;
}

static enum unlocked_err test_mod_failure(enum unlocked_err err)
{
	failure_called++;

	return UL_OK;
}

static struct unlocked_module test_module = {
	.init = NULL,
	.success = &test_mod_success,
	.failure = &test_mod_failure,
	.cleanup = NULL,
};

static void setup(void)
{
	register_module(&test_module);
}

static void teardown(void)
{
	success_key = NULL;
	failure_called = 0;
	cleanup_modules();
}

START_TEST(test_mod_module_handle_failure)
{
	handle_failure(UL_ERR);
	ck_assert_int_eq(1, failure_called);
}
// *INDENT-OFF*
END_TEST
// *INDENT-ON*

START_TEST(test_mod_module_handle_success)
{
	handle_success("test");
	ck_assert_str_eq("test", success_key);
	success_key = NULL;
}
// *INDENT-OFF*
END_TEST
// *INDENT-ON*

static TCase *make_mod_module_handle_failure_case(void)
{
	TCase *tc;

	tc = tcase_create("mod::module::handle_failure");
	tcase_add_checked_fixture(tc, setup, teardown);
	tcase_add_test(tc, test_mod_module_handle_failure);

	return tc;
}

static TCase *make_mod_module_handle_success_case(void)
{
	TCase *tc;

	tc = tcase_create("mod::module::handle_success");
	tcase_add_checked_fixture(tc, setup, teardown);
	tcase_add_test(tc, test_mod_module_handle_success);

	return tc;
}

Suite *make_mod_module_suite(void)
{
	Suite *s;

	s = suite_create("unlocked-client mod module");
	suite_add_tcase(s, make_mod_module_handle_failure_case());
	suite_add_tcase(s, make_mod_module_handle_success_case());

	return s;

}
