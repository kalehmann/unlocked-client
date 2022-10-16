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

#include "cli.h"
#include "client.h"
#include "error.h"
#include "log.h"
#include "mod/module.h"
#include "mod/mod_sd_socket.h"
#include "mod/mod_stdout.h"

const char *argp_program_version = "unlocked-client dev";
const char *argp_program_bug_address = "<mail@kalehmann.de>";

static int validate_args(struct arguments *arguments)
{
	if (NULL == arguments->host) {
		fprintf(stderr, "No hostname given\n");

		return EXIT_FAILURE;
	}
	if (0 == arguments->port) {
		fprintf(stderr, "Invalid port given\n");

		return EXIT_FAILURE;
	}
	if (NULL == arguments->key_handle) {
		fprintf(stderr, "No key handle given\n");

		return EXIT_FAILURE;
	}
	if (NULL == arguments->secret) {
		fprintf(stderr, "No secret given\n");

		return EXIT_FAILURE;
	}
	if (NULL == arguments->username) {
		fprintf(stderr, "No username given\n");

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
	enum unlocked_err err = UL_OK;
	struct arguments *arguments = create_args();
	if (NULL == arguments) {
		return EXIT_FAILURE;
	}
	arguments->port = 443;
	arguments->validate = VALIDATE;

	register_module(get_mod_sd_socket());
	register_module(get_mod_stdout());
	handle_args(argc, argv, arguments);
	if (EXIT_SUCCESS != validate_args(arguments)) {
		free_args(arguments);
		free_child_parsers();
		cleanup_modules();

		return EXIT_FAILURE;
	}
	err = initialize_modules();
	if (UL_OK != err) {
		free_args(arguments);
		free_child_parsers();
		cleanup_modules();
		logger(LOG_ERROR, ul_error(err));

		return EXIT_FAILURE;
	}

	err = request_key(arguments);
	free_args(arguments);
	free_child_parsers();
	cleanup_modules();
	if (UL_OK != err) {
		logger(LOG_ERROR, ul_error(err));

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
