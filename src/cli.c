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
#include <iniparser.h>

#include "cli.h"
#include "log.h"

#define OPT_CONFIG 'c'
#define	OPT_HOST 'h'
#define	OPT_KEY 'k'
#define	OPT_PORT 'p'
#define OPT_SECRET 's'
#define OPT_USER 'u'
#define OPT_SKIP_VALIDATION 256

static char doc[] = "unlocked-client -- a tool to fetch keys from a server";

// *INDENT-OFF*
static struct argp_option options[] = {
        {
		.name = "config",
		.key = OPT_CONFIG,
		.arg = "<path>",
		.flags = 0,
		.doc = "Path to the configuration file",
	},
	{
		.name = "host",
		.key = OPT_HOST,
		.arg = "<hostname>",
		.flags = 0,
		.doc = "Hostname of the server",
	},
	{
		.name = "key",
		.key = OPT_KEY,
		.arg = "<key handle>",
		.flags = 0,
		.doc = "Handle of the key to request",
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
// *INDENT-ON*

/**
 * Argp parser function.
 *
 * @param key specifies the argument or option passed to the func
 * @param arg is the value for the argument or option
 * @param state is the argp state where the arguments and options are saved to
 *
 * @return zero or any error that occured
 */
static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
	struct arguments *arguments = state->input;

	switch (key) {
	case OPT_CONFIG:
		arguments->config_file = arg;
		break;
	case OPT_HOST:
		arguments->host = arg;
		break;
	case OPT_KEY:
		arguments->key_handle = arg;
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
		arguments->validate = SKIP_VALIDATION;
		break;
	case ARGP_KEY_ARG:
		argp_usage(state);

		break;
	default:
		return ARGP_ERR_UNKNOWN;
	}

	return 0;
}

// *INDENT-OFF*
static struct argp argp_client = {
	.options = options,
	.parser = parse_opt,
	.args_doc = NULL,
	.doc = doc,
	.children = NULL,
	.help_filter = NULL,
	.argp_domain = NULL,
};
// *INDENT-ON*

enum unlocked_err handle_args(int argc, char **argv, struct arguments *args)
{
	struct arguments cli_args = { 0 };
	struct arguments config_args = { 0 };
	enum unlocked_err err = UL_OK;

	argp_parse(&argp_client, argc, argv, 0, 0, &cli_args);

	if (cli_args.config_file) {
		err = parse_config_file(cli_args.config_file, &config_args);
		if (UL_OK != err) {
			return err;
		}
	}

	merge_config(args, &config_args);
	merge_config(args, &cli_args);

	return UL_OK;
}

void merge_config(struct arguments *base, struct arguments *new)
{
	if (new->config_file) {
		base->config_file = new->config_file;
	}
	if (new->key_handle) {
		base->key_handle = new->key_handle;
	}
	if (new->host) {
		base->host = new->host;
	}
	if (new->port) {
		base->port = new->port;
	}
	if (new->secret) {
		base->secret = new->secret;
	}
	if (new->username) {
		base->username = new->username;
	}
	if (new->validate) {
		base->validate = new->validate;
	}
}

enum unlocked_err parse_config_file(const char *const path,
				    struct arguments *args)
{
	dictionary *ini = NULL;
	const char *host = NULL;
	const char *key_handle = NULL;
	const char *secret = NULL;
	const char *username = NULL;
	int validate = 0;

	ini = iniparser_load(path);
	if (NULL == ini) {
		logger(LOG_ERROR,
		       "Could not parse configuration file %s", path);

		return UL_ERR;
	}
	host = iniparser_getstring(ini, "unlocked:host", NULL);
	if (NULL != host) {
		args->host = strdup(host);
		if (NULL == args->host) {
			iniparser_freedict(ini);

			return UL_MALLOC;
		}
	}
	key_handle = iniparser_getstring(ini, "unlocked:key_handle", NULL);
	if (NULL != key_handle) {
		args->key_handle = strdup(key_handle);
		if (NULL == args->key_handle) {
			iniparser_freedict(ini);

			return UL_MALLOC;
		}
	}
	args->port = iniparser_getlongint(ini, "unlocked:port", 0);
	secret = iniparser_getstring(ini, "unlocked:secret", NULL);
	if (NULL != secret) {
		args->secret = strdup(secret);
		if (NULL == args->secret) {
			iniparser_freedict(ini);

			return UL_MALLOC;
		}
	}
	username = iniparser_getstring(ini, "unlocked:username", NULL);
	if (NULL != username) {
		args->username = strdup(username);
		if (NULL == args->username) {
			iniparser_freedict(ini);

			return UL_MALLOC;
		}
	}
	validate = iniparser_getboolean(ini, "unlocked:validate", -1);
	switch (validate) {
	case 1:
		args->validate = VALIDATE;
		break;
	case 0:
		args->validate = SKIP_VALIDATION;
		break;
	default:
		args->validate = 0;
	}

	iniparser_freedict(ini);

	return UL_OK;
}
