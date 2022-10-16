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

#include <argp.h>
#include <systemd/sd-daemon.h>
#include <sys/socket.h>

#include "../log.h"
#include "module.h"
#include "mod_sd_socket.h"

#define OPT_SD_SOCKET 350

struct sd_socket_state {
	int socket_fd;
};

static const char *const module_name = "mod_sd_socket";

// *INDENT-OFF*
static struct argp_option options[] = {
        {
		.name = "sd-socket",
		.key = OPT_SD_SOCKET,
		.arg = 0,
		.flags = 0,
		.doc = "Output the key on the standard output",
	},
	{ 0 }
};
// *INDENT-ON*

static error_t sd_socket_parser(int key, char *arg, struct argp_state *state)
{
	struct unlocked_module *module = state->input;

	switch (key) {
	case OPT_SD_SOCKET:
		module->enabled = 1;
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

static enum unlocked_err init(struct unlocked_module *module)
{
	char **socket_names = 0;
	int fd_count = sd_listen_fds_with_names(0, &socket_names);
	struct sd_socket_state *state = module->state;
	if (!module->enabled) {
		if (fd_count > 0) {
			return UL_SD_SOCKET_DISABLED;
		}
		return UL_OK;
	}

	if (fd_count == 0) {
		return UL_SD_SOCKET_NO_FD;
	}
	if (fd_count > 1) {
		return UL_SD_SOCKET_MANY_FD;
	}

	logger(LOG_DEBUG, "Socket \"%s\" passed by systemd.\n",
	       socket_names[0]);
	state->socket_fd = SD_LISTEN_FDS_START + 0;

	return UL_OK;
}

static enum unlocked_err success(struct unlocked_module *module,
				 const char *const key)
{
	int client_fd = 0, ret = 0;
	size_t key_len = strlen(key);
	struct sd_socket_state *state = module->state;
	if (!module->enabled) {
		return UL_OK;
	}
	client_fd = accept(state->socket_fd, NULL, NULL);
	if (client_fd < 0) {
		return UL_ERRNO;
	}
	ret = send(client_fd, key, key_len, 0);
	if (ret < 0) {
		return UL_ERRNO;
	}

	return UL_OK;
}

static enum unlocked_err parse_sd_socket_config(struct unlocked_module *module,
						const dictionary * ini)
{
	int use = iniparser_getboolean(ini, "sd_socket:use_socket", -1);

	switch (use) {
	case 1:
		module->enabled = 1;
		break;
	case 0:
		module->enabled = 0;
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
	argp->parser = sd_socket_parser;
	argp->args_doc = NULL;
	argp->doc = NULL;
	argp->children = NULL;
	argp->help_filter = NULL;
	argp->argp_domain = NULL;

	return argp;
}

static struct sd_socket_state *init_state(void)
{
	struct sd_socket_state *state = malloc(sizeof(struct sd_socket_state));
	if (NULL == state) {
		return NULL;
	}
	state->socket_fd = 0;

	return state;
}

struct unlocked_module *get_mod_sd_socket(void)
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
	module->name = module_name;
	module->enabled = 0;
	module->init = &init;
	module->parse_config = &parse_sd_socket_config;
	module->success = &success;
	module->failure = NULL;
	module->cleanup = &cleanup;

	return module;
}
