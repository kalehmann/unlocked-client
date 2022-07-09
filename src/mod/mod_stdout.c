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
#include "module.h"
#include "mod_stdout.h"
#include "../error.h"

static enum unlocked_err cleanup(struct unlocked_module *module)
{
	free(module);

	return UL_OK;
}

static enum unlocked_err success(const char *const key)
{
	size_t key_len = strlen(key);
	if (key_len != fwrite(key, sizeof(char), key_len, stdout)) {
		return UL_ERR;
	}
	if (fflush(stdout)) {
		return UL_ERR;
	}

	return UL_OK;
}

struct unlocked_module *get_mod_stdout(void)
{
	struct unlocked_module *module = malloc(sizeof(struct unlocked_module));
	if (NULL == module) {
		return NULL;
	}
	module->init = NULL;
	module->success = &success;
	module->failure = NULL;
	module->cleanup = &cleanup;

	return module;
}
