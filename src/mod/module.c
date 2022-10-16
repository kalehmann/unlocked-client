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
#include "module.h"
#include "../cli.h"

static struct unlocked_module **modules = NULL;

static unsigned int module_count = 0;

enum unlocked_err cleanup_modules(void)
{
	enum unlocked_err err = UL_OK;

	for (unsigned int i = 0; i < module_count; i++) {
		if (NULL != modules[i]->cleanup) {
			err = modules[i]->cleanup(modules[i]);
			if (UL_OK != err) {
				return err;
			}
		}
	}
	free(modules);
	modules = NULL;
	module_count = 0;

	return err;
}

enum unlocked_err handle_failure(enum unlocked_err provided_err)
{
	enum unlocked_err err = UL_OK;

	for (unsigned int i = 0; i < module_count; i++) {
		if (NULL != modules[i]->failure) {
			err = modules[i]->failure(modules[i], provided_err);
			if (UL_OK != err) {
				return err;
			}
		}
	}

	return err;
}

enum unlocked_err handle_success(const char *const key)
{
	enum unlocked_err err = UL_OK;

	for (unsigned int i = 0; i < module_count; i++) {
		if (NULL != modules[i]->success) {
			err = modules[i]->success(modules[i], key);
			if (UL_OK != err) {
				return err;
			}
		}
	}

	return err;
}

enum unlocked_err initialize_modules(void)
{
	enum unlocked_err err = UL_OK;

	for (unsigned int i = 0; i < module_count; i++) {
		if (NULL != modules[i]->init) {
			err = modules[i]->init(modules[i]);
			if (UL_OK != err) {
				return err;
			}
		}
	}

	return err;
}

enum unlocked_err parse_config(const dictionary * ini)
{
	enum unlocked_err err = UL_OK;

	for (unsigned int i = 0; i < module_count; i++) {
		if (NULL != modules[i]->parse_config) {
			err = modules[i]->parse_config(modules[i], ini);
			if (UL_OK != err) {
				return err;
			}
		}
	}

	return err;
}

enum unlocked_err register_module(struct unlocked_module *module)
{
	static const size_t mod_size = sizeof(struct unlocked_module *);

	modules = realloc(modules, mod_size * (module_count + 1));
	if (NULL == modules) {
		return UL_MALLOC;
	}
	modules[module_count] = module;
	module_count++;
	register_child_parser(module);

	return UL_OK;
}
