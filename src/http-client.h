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

#ifndef UNLOCKED_HTTP_CLIENT_H
#define UNLOCKED_HTTP_CLIENT_H

#include "error.h"

/**
 * Initializes the http client.
 *
 * @return a status code indicating any error that occured.
 */
enum unlocked_err init_https_client(void);

/**
 *
 */
enum unlocked_err https_hmac_POST(const char * username, const char * secret,
				  const char * url, long port,
				  const char * body);

/**
 * Cleanup after the http client.
 */
void cleanup_https_client(void);

#endif