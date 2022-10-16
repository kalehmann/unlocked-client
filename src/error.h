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

#ifndef UNLOCKED_ERROR_H
#define UNLOCKED_ERROR_H

enum unlocked_err {
	UL_OK = 0,
	UL_CURL,
	UL_DENIED,
	UL_ERR,
	UL_ERRNO,
	UL_MALLOC,
	UL_SD_SOCKET_DISABLED,
	UL_SD_SOCKET_NO_FD,
	UL_SD_SOCKET_MANY_FD,
};

/**
 * Get a human readable string for an error.
 *
 * @param err the error code to get a human readable string for
 *
 * @return the human readable string describing the error.
 */
const char *ul_error(enum unlocked_err err);

#endif
