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

	request_key(&arguments);

	return EXIT_SUCCESS;
}
