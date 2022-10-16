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

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include "cli.h"

#define LOG_DEBUG "debug"
#define LOG_ERROR "error"
#define LOG_INFO "info"
#define LOG_WARNING "warning"

/**
 * Function for log entries.
 *
 * @param level defines the priority of the log entry. This decides whether
 *              the log entry is written to the standard output or the standard
 *              error stream (or not written at all).
 * @param fmt is the format string. See `man 3 printf` for further details.
 * @param ... are additional arguments for the format string. See `man 3 printf`
 *            for further details.
 */
void logger(const char *const level, const char *const fmt, ...);

#endif
