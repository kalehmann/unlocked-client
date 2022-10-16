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
#define OPT_VERBOSE 257


static char doc[] = "unlocked-client -- a tool to fetch keys from a server";
static size_t sub_parser_count = 0;
static struct argp_child *sub_parsers = NULL;
static void **sub_parser_inputs = NULL;
static unsigned int is_debug = 0;

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
	{
		.name = "verbose",
		.key = OPT_VERBOSE,
		.arg = 0,
		.flags = 0,
		.doc = "Output debug information",
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
		arguments->config_file = strdup(arg);
		break;
	case OPT_HOST:
		arguments->host = strdup(arg);
		break;
	case OPT_KEY:
		arguments->key_handle = strdup(arg);
		break;
	case OPT_PORT:
		arguments->port = atol(arg);
		break;
	case OPT_SECRET:
		arguments->secret = strdup(arg);
		break;
	case OPT_USER:
		arguments->username = strdup(arg);
		break;
	case OPT_SKIP_VALIDATION:
		arguments->validate = no;
		break;
	case OPT_VERBOSE:
		arguments->verbose = yes;
		break;
	case ARGP_KEY_ARG:
		argp_usage(state);

		break;
	case ARGP_KEY_INIT:
		for (size_t i = 0; i < sub_parser_count; i++) {
			state->child_inputs[i] = sub_parser_inputs[i];
		}
		break;
	case ARGP_KEY_FINI:
		break;
	default:
		return ARGP_ERR_UNKNOWN;
	}

	return 0;
}

struct arguments *create_args(void)
{
	return calloc(1, sizeof(struct arguments));
}

enum unlocked_err handle_args(int argc, char **argv, struct arguments *args)
{
	struct arguments *cli_args = create_args();
	struct arguments *config_args = create_args();
	enum unlocked_err err = UL_OK;

	if (NULL == cli_args) {
		return UL_MALLOC;
	}
	if (NULL == config_args) {
		free_args(cli_args);

		return UL_MALLOC;
	}
	struct argp argp_client = {
		.options = options,
		.parser = parse_opt,
		.args_doc = NULL,
		.doc = doc,
		.children = sub_parsers,
		.help_filter = NULL,
		.argp_domain = NULL,
	};

	argp_parse(&argp_client, argc, argv, 0, 0, cli_args);

	if (cli_args->config_file) {
		err = parse_config_file(cli_args->config_file, config_args);
		if (UL_OK != err) {
			return err;
		}
	}

	merge_config(args, config_args);
	free_args(config_args);
	merge_config(args, cli_args);
	free_args(cli_args);

	// Set global debug mode.
	is_debug = args->verbose == yes;

	return UL_OK;
}

void merge_config(struct arguments *base, struct arguments *new)
{
	if (new->config_file) {
		if (base->config_file) {
			free(base->config_file);
		}
		base->config_file = strdup(new->config_file);
	}
	if (new->key_handle) {
		if (base->key_handle) {
			free(base->key_handle);
		}
		base->key_handle = strdup(new->key_handle);
	}
	if (new->host) {
		if (base->host) {
			free(base->host);
		}
		base->host = strdup(new->host);
	}
	if (new->port) {
		base->port = new->port;
	}
	if (new->secret) {
		if (base->secret) {
			free(base->secret);
		}
		base->secret = strdup(new->secret);
	}
	if (new->username) {
		if (base->username) {
			free(base->username);
		}
		base->username = strdup(new->username);
	}
	if (new->validate) {
		base->validate = new->validate;
	}
	if (new->verbose) {
		base->verbose = new->verbose;
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
	enum unlocked_err err = UL_OK;

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
		args->validate = yes;
		break;
	case 0:
		args->validate = no;
		break;
	default:
		args->validate = unset;
	}

	err = parse_config(ini);
	if (UL_OK != err) {
		return err;
	}

	iniparser_freedict(ini);

	return err;
}

enum unlocked_err register_child_parser(const struct unlocked_module *module)
{
	static const size_t child_size = sizeof(struct argp_child);

	if (NULL == module->argp) {
		return UL_OK;
	}

	sub_parsers = realloc(sub_parsers, child_size * (sub_parser_count + 2));
	if (NULL == sub_parsers) {
		return UL_MALLOC;
	}
	sub_parser_inputs = realloc(sub_parser_inputs,
				    sizeof(void *) * (sub_parser_count + 1));
	if (NULL == sub_parser_inputs) {
		return UL_MALLOC;
	}

	sub_parsers[sub_parser_count].argp = module->argp;
	sub_parsers[sub_parser_count].flags = 0;
	sub_parsers[sub_parser_count].header = NULL;
	sub_parsers[sub_parser_count].group = 0;

	sub_parsers[sub_parser_count + 1].argp = NULL;
	sub_parsers[sub_parser_count + 1].flags = 0;
	sub_parsers[sub_parser_count + 1].header = NULL;
	sub_parsers[sub_parser_count + 1].group = 0;

	sub_parser_inputs[sub_parser_count] = module->state;

	sub_parser_count++;
}

void free_args(struct arguments *args)
{
	if (NULL == args) {
		return;
	}

	if (args->config_file) {
		free(args->config_file);
	}
	if (args->key_handle) {
		free(args->key_handle);
	}
	if (args->host) {
		free(args->host);
	}
	if (args->secret) {
		free(args->secret);
	}
	if (args->username) {
		free(args->username);
	}

	free(args);
}

void free_child_parsers(void)
{
	free(sub_parsers);
	free(sub_parser_inputs);
	sub_parser_count = 0;
}

unsigned int ul_debug(void)
{
	return is_debug;
}
