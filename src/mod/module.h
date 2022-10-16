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

/**
 * Structure for modules.
 *
 * While not recommended, any member can be safely set to NULL.
 */
struct unlocked_module {
	/**
	 * Structure with information about the cli argument parser.
	 */
	struct argp *argp;
	/**
	 * If necessary, the internal state of the module.
	 */
	void *state;
	/**
	 * The name of the module.
	 */
	const char *name;
	/**
	 * The module can set this value after parsing the config to set if
	 * it has been enabled or not.
	 *
	 * If not enabled, the failure or success callbacks will not be invoked
	 * for this module. However init and cleanup will still be invoked in
	 * case the module needs to perform some sanity checks.
	 */
	unsigned int enabled;
	/**
	 * The dictionary from the parsed config file is passed to this
	 * function.
	 *
	 * @param module is the instance of the module.
	 * @param dict is the dictionary of the parsed config file.
	 *
	 * @return any error that occured.
	 */
	enum unlocked_err (*parse_config) (struct unlocked_module * module,
					   const dictionary * dict);
	/**
	 * Used to initialize additional resources for the module.
	 *
	 * This method is called whether the module has been enabled or not.
	 * Therefore it is necessary to check `module->enabled` in the function.
	 *
	 * @param module is the instance of the module.
	 *
	 * @return any error that occured.
	 */
	enum unlocked_err (*init) (struct unlocked_module *);
	/**
	 * Called after a key has been received from the server.
	 *
	 * @param module is the instance of the module.
	 * @param key is the key received from the server.
	 *
	 * @return any error that occured.
	 */
	enum unlocked_err (*success) (struct unlocked_module * module,
				      const char *const key);
	/**
	 * Called on failure.
	 *
	 * @param module is the instance of the module.
	 * @param err is the code of the error that occured.
	 *
	 * @return any error that occured while handling the failure..
	 */
	enum unlocked_err (*failure) (struct unlocked_module * module,
				      enum unlocked_err err);
	/**
	 * Called after retrieving a key has succeded or failed to cleanup any
	 * resource allocated in init.
	 *
	 * This method is called whether the module has been enabled or not.
	 * Therefore it is necessary to check `module->enabled` in the function.
	 *
	 * @param module is the instance of the module.
	 *
	 * @return any error that occured.
	 */
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
 * Give the modules a chance to initialize additional resources.
 *
 * @return any error from the modules
 */
enum unlocked_err initialize_modules(void);

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
