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

#include "log.h"

void logger(const char *const level, const char *const fmt, ...)
{
	FILE *stream = NULL;
	va_list args;
	va_start(args, fmt);
	if (0 == strcmp(level, LOG_ERROR)
	    || 0 == strcmp(level, LOG_WARNING)) {
		stream = stderr;
	} else if (0 == strcmp(level, LOG_DEBUG)) {
		if (ul_debug()) {
			stream = stdout;
		}
	} else {
		stream = stdout;
	}

	if (stream) {
		fprintf(stream, "[%s] ", level);
		vfprintf(stream, fmt, args);
		fflush(stream);
	}

	va_end(args);
}
