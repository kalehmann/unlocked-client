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


#define	OPT_HOST 'h'
#define	OPT_PORT 'p'
#define OPT_SECRET 's'
#define OPT_USER 'u'
#define OPT_SKIP_VALIDATION 256


static char doc[] =
  "unlocked-client -- a tool to fetch keys from a server";

static char args_doc[] = "KEY_HANDLE";

static struct argp_option options[] = {
	{
		.name = "host",
		.key = OPT_HOST,
		.arg = "<hostname>",
		.flags = 0,
		.doc = "Hostname of the server",
	},
	{
		.name = "port",
		.key = OPT_PORT,
		.arg = "<port>",
		.flags = 0,
		.doc = "Port on the server (default 443)",
	},
	{
		.name = "secret",
		.key = OPT_SECRET,
		.arg = "<secret>",
		.flags = 0,
		.doc = "Secret used to authenticate against the server",
	},
	{
		.name = "user",
		.key = OPT_USER,
		.arg = "<username>",
		.flags = 0,
		.doc = "Handle of the client",
	},
	{
		.name = "skip-validation",
		.key = OPT_SKIP_VALIDATION,
		.arg = 0,
		.flags = 0,
		.doc = "Skip certificate validation",
	},
	{ 0 }
};

/**
 * Argp parser function.
 *
 * @param key specifies the argument or option passed to the func
 * @param arg is the value for the argument or option
 * @param state is the argp state where the arguments and options are saved to
 *
 * @return zero or any error that occured
 */
static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
	struct arguments *arguments = state->input;

	switch (key)
	{
	case OPT_HOST:
		arguments->host = arg;
		break;
	case OPT_PORT:
		arguments->port = atol(arg);
		break;
	case OPT_SECRET:
		arguments->secret = arg;
		break;
	case OPT_USER:
		arguments->username = arg;
		break;
	case OPT_SKIP_VALIDATION:
		arguments->no_validation = 1;
		break;
	case ARGP_KEY_ARG:
		if (state->arg_num >= 1) {
			argp_usage (state);
		}

		arguments->key_handle = arg;

		break;

	case ARGP_KEY_END:
		if (state->arg_num < 1) {
			argp_usage (state);
		}
		break;
	default:
		return ARGP_ERR_UNKNOWN;
	}

	return 0;
}

static struct argp argp_client = {
	.options = options,
	.parser = parse_opt,
	.args_doc = args_doc,
	.doc = doc,
	.children = NULL,
	.help_filter = NULL,
	.argp_domain = NULL,
};

void handle_args(int argc, char ** argv, struct arguments * args)
{
	argp_parse (&argp_client, argc, argv, 0, 0, args);
}
