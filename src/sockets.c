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

#include <stdio.h>		/* EXIT_FAILURE, EXIT_SUCCESS */
#include <stdlib.h>		/* perror */
#include <string.h>		/* memset, strcpy */
#include <unistd.h>		/* close, sleep, unlink */

#include <sys/socket.h>		/* accept, bind, listen, PF_UNIX, send, SOCK_STREAM, socket, socklen_t */
#include <sys/stat.h>		/* fchmod */
#include <sys/un.h>		/* struct sockaddr_un */

#define SOCKET_PATH "test.sock"

int socket_fd = -1;

void cleanup_socket(void)
{
	unlink(SOCKET_PATH);
	if (socket_fd >= 0) {
		close(socket_fd);
		socket_fd = -1;
	}
}

int init_socket(void)
{
	struct sockaddr_un addr;

	socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);
	if (socket_fd < 0) {
		perror("Socket");

		return EXIT_FAILURE;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, SOCKET_PATH);
	unlink(SOCKET_PATH);
	fchmod(socket_fd, 777);
	if (bind(socket_fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		perror("Bind");
		cleanup_socket();

		return EXIT_FAILURE;
	}

	if (listen(socket_fd, 1) == -1) {
		perror("Listen");
		cleanup_socket();

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int socket_callback(char *key, size_t key_len)
{
	int clientfd, ret;
	socklen_t clilen;
	struct sockaddr_un cli_addr;

	clilen = sizeof(cli_addr);
	clientfd = accept(socket_fd, (struct sockaddr *) &cli_addr, &clilen);
	if (clientfd < 0) {
		perror("Accept");
		cleanup_socket();

		return EXIT_FAILURE;
	}
	ret = send(clientfd, key, key_len, 0);
	if (ret < 0) {
		perror("Send: ");
		cleanup_socket();

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int getKey(int result_cb(char *, size_t))
{
	sleep(10);

	return result_cb("12345678123456781234567812345678", 32);
}
