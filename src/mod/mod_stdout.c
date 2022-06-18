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
#include "mod_stdout.h"
#include "../error.h"

static enum unlocked_err success(const char * const key)
{
	printf(key);

	return UL_OK;
}

struct unlocked_module * get_mod_stdout(void)
{
	struct unlocked_module * module = malloc(sizeof(struct unlocked_module));
	if (NULL == module) {
		return NULL;
	}
	module->init = NULL;
	module->success = &success;
	module->failure = NULL;
	module->cleanup = NULL;

	return module;
}
