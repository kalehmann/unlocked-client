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

#ifndef UNLOCKED_CLI_H
#define UNLOCKED_CLI_H

#define SKIP_VALIDATION 1
#define VALIDATE -1

#include <argp.h>

#include "error.h"

struct arguments {
	char *config_file;
	char *key_handle;
	char *host;
	long port;
	char *secret;
	char *username;
	int validate;
};

void handle_args(int argc, char **argv, struct arguments *args);

/**
 *
 */
void merge_config(struct arguments *base, struct arguments *new);


#endif
