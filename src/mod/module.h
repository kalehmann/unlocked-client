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

#ifndef UNLOCKED_MOD_MODULE_H
#define UNLOCKED_MOD_MODULE_H

#include <argp.h>
#include <iniparser.h>

#include "../error.h"

struct unlocked_module {
	struct argp *argp;
	void *state;
	enum unlocked_err (*parse_config) (struct unlocked_module *,
					   const dictionary *);
	enum unlocked_err (*init) (struct unlocked_module *);
	enum unlocked_err (*success) (struct unlocked_module *,
				      const char *const);
	enum unlocked_err (*failure) (struct unlocked_module *,
				      enum unlocked_err);
	enum unlocked_err (*cleanup) (struct unlocked_module *);
};

/**
 * Tell all registered modules to clean up any temporary files, open sockets, ...
 *
 * @returns any error from the modules
 */
enum unlocked_err cleanup_modules(void);

/**
 * Tell the modules about a failure that occured while receiving the key.
 *
 * @param err describes the failure that occured
 *
 * @returns any error from the modules
 */
enum unlocked_err handle_failure(enum unlocked_err err);

/**
 * Provision the key from the server to all registered modules.
 *
 * @param key is the key provided by the server
 *
 * @return any error from the modules
 */
enum unlocked_err handle_success(const char *const key);

/**
 * Let all the registered modules parse the config file.
 *
 * @param ini is the dictionary of the loaded config file.
 *
 * @return any error from the modules.
 */
enum unlocked_err parse_config(const dictionary * ini);

/**
 * Registers a new module for the application.
 *
 * @param module is the module to be registered.
 *
 * @returns any error that occured
 */
enum unlocked_err register_module(struct unlocked_module *module);

#endif
