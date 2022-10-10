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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <argp.h>

#include "module.h"
#include "mod_stdout.h"
#include "../error.h"

#define OPT_STDOUT 300

struct stdout_state {
	int use_stdout;
};

// *INDENT-OFF*
static struct argp_option options[] = {
        {
		.name = "stdout",
		.key = OPT_STDOUT,
		.arg = 0,
		.flags = 0,
		.doc = "Output the key on the standard output",
	},
	{ 0 }
};
// *INDENT-ON*

static error_t stdout_parser(int key, char *arg, struct argp_state *state)
{
	struct stdout_state *input = state->input;

	switch (key) {
	case OPT_STDOUT:
		input->use_stdout = 1;
		break;
	default:
		return ARGP_ERR_UNKNOWN;
	}

	return 0;
}

static enum unlocked_err cleanup(struct unlocked_module *module)
{
	if (NULL == module) {
		return UL_OK;
	}
	if (NULL != module->argp) {
		free(module->argp);
	}
	if (NULL != module->state) {
		free(module->state);
	}
	free(module);

	return UL_OK;
}

static enum unlocked_err success(struct unlocked_module *module,
				 const char *const key)
{
	size_t key_len = strlen(key);
	struct stdout_state *state = module->state;
	if (!state->use_stdout) {
		return UL_OK;
	}
	if (key_len != fwrite(key, sizeof(char), key_len, stdout)) {
		return UL_ERR;
	}
	if (fflush(stdout)) {
		return UL_ERR;
	}

	return UL_OK;
}

static enum unlocked_err parse_stdout_config(struct unlocked_module *module,
					     const dictionary * ini)
{
	struct stdout_state *state = module->state;
	int use = iniparser_getboolean(ini, "stdout:use_stdout", -1);

	switch (use) {
	case 1:
		state->use_stdout = 1;
		break;
	case 0:
		state->use_stdout = 0;
		break;
	}

	return UL_OK;
}

static struct argp *init_argp(void)
{
	struct argp *argp = malloc(sizeof(struct argp));
	if (NULL == argp) {
		return NULL;
	}
	argp->options = options;
	argp->parser = stdout_parser;
	argp->args_doc = NULL;
	argp->doc = NULL;
	argp->children = NULL;
	argp->help_filter = NULL;
	argp->argp_domain = NULL;

	return argp;
}

static struct stdout_state *init_state(void)
{
	struct stdout_state *state = malloc(sizeof(struct stdout_state));
	if (NULL == state) {
		return NULL;
	}
	state->use_stdout = 0;

	return state;
}

struct unlocked_module *get_mod_stdout(void)
{
	struct unlocked_module *module = malloc(sizeof(struct unlocked_module));
	if (NULL == module) {
		return NULL;
	}
	module->argp = init_argp();
	if (NULL == module->argp) {
		return NULL;
	}
	module->state = init_state();
	if (NULL == module->state) {
		free(module->argp);

		return NULL;
	}
	module->init = NULL;
	module->parse_config = &parse_stdout_config;
	module->success = &success;
	module->failure = NULL;
	module->cleanup = &cleanup;

	return module;
}
