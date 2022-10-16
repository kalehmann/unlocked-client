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

#ifndef UNLOCKED_CLI_H
#define UNLOCKED_CLI_H

#include "mod/module.h"
#include "error.h"

enum tristate { no = -1, unset = 0, yes = 1 };

struct arguments {
	char *config_file;
	char *key_handle;
	char *host;
	long port;
	char *secret;
	char *username;
	enum tristate validate;
	enum tristate verbose;
};

/**
 * Create a new structure to hold arguments supplied to the application.
 *
 * @return the allocated argument structure or NULL on failure.
 */
struct arguments *create_args(void);

/**
 * Aquire all the configuration for the unlocked client.
 *
 * The following sources are used in this order:
 * - Default values
 * - Configuration files
 * - Commandline arguments
 *
 * Values from later sources override the values from former ones.
 *
 * @param argc is the number of command line arguments
 * @param argv is the vector with the command line arguments
 * @param args is the structure that will be filled with the parsed values.
 *             If the structure is already prepopulated with any values, these
 *             will be kept if no new value is provided on the command line or
 *             in a config file.
 *
 * @return any error that occured
 */
enum unlocked_err handle_args(int argc, char **argv, struct arguments *args);

/**
 * Merge to structures with arguments to the unlocked client.
 * Unset values will not be transfered from the new to the base
 * arguments.
 *
 * @param base is the structure of arguments which may be overriden.
 * @param new is the structure of arguments which are used to override the
 *            base values.
 */
void merge_config(struct arguments *base, struct arguments *new);

/**
 * Parse values from a configuration file.
 *
 * @param path is the path to the configuration file.
 * @param args is the structure that will be populated with the arguments from
 *             the configuration file.
 *
 * @return any error that occured while parsing the config file.
 */
enum unlocked_err parse_config_file(const char *const path,
				    struct arguments *args);

/**
 * @param module is the module of which a child parser will be registered if
 *               available.
 *
 * @return any error that occured
 */
enum unlocked_err register_child_parser(struct unlocked_module *module);

/**
 * Free all resources related to the arguments structure.
 */
void free_args(struct arguments *args);

/**
 * Free all resources allocated for the child parsers.
 */
void free_child_parsers(void);

/**
 * This function is used to decide whether additional data should be outputted
 * for debugging purposes.
 *
 * The value this function returns is decided in the `handle_args` function.
 *
 * @returns if application is in debug mode.
 */
unsigned int ul_debug(void);

#endif
