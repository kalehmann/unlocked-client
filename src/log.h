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

#ifndef UNLOCKED_LOG_H
#define UNLOCKED_LOG_H

#include <string.h>

#include "cli.h"

#define LOG_DEBUG "debug"
#define LOG_ERROR "error"
#define LOG_INFO "info"
#define LOG_WARNING "warning"

#define logger(level, ...) do {                                         \
        if (0 == strcmp(level, LOG_ERROR)                               \
            || 0 == strcmp(level, LOG_WARNING)) {                       \
                fprintf(stderr, "[%s] ", level);                        \
                fprintf(stderr, ##__VA_ARGS__);                         \
                fflush(stderr);                                         \
        } else if (0 == strcmp(level, LOG_DEBUG)) {                     \
	        if (ul_debug()) {                                       \
		        fprintf(stdout, "[%s] ", level);                \
			fprintf(stdout, ##__VA_ARGS__);                 \
			fflush(stdout);                                 \
	        }                                                       \
        } else {                                                        \
                fprintf(stdout, "[%s] ", level);                        \
                fprintf(stdout, ##__VA_ARGS__);                         \
                fflush(stdout);                                         \
        }                                                               \
} while (0)

#endif
