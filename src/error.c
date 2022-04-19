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

#include "error.h"

static const char * ERR_OK = "No error\n";
static const char * ERR_CURL = "There was an error calling libcurl\n";
static const char * ERR_ERR = "Logic error\n";
static const char * ERR_MALLOC = "Failed to allocate memory\n";
static const char * ERR_UNKNOWN = "Unknomn error\n";

const char * ul_error(enum unlocked_err err)
{
	switch(err) {
	case UL_OK:
		return ERR_OK;
	case UL_CURL:
		return ERR_CURL;
	case UL_ERR:
		return ERR_ERR;
	case UL_MALLOC:
		return ERR_MALLOC;
	default:
		return ERR_UNKNOWN;
	}
}
