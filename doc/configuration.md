## Unlocked client configuration

The unlocked client can be fully configured from configuration files.
All configuration files are passed with the
[iniparser library](https://github.com/ndevilla/iniparser).

### `[unlocked]` section

* `host`: This value is of type string and contains the host name of the
    [unlocked-server](https://github.com/kalehmann/unlocked-server).
* `key_handle`: This value is of type string and specifies the handle of the
    key, that should be requested from the server.
* `port`: This value is a positive integer and specifies the port of the
    application on the host where the server is located.
* `secret`: This value is of type string and specifies a secret value, that is
    used to authenticate the client against the server.
* `username`: This value is of type string and is used to identify the client
    against the server.
* `validate`: This value is of type boolean and specifies whether the
    certificate from the server should be verified.

### `[sd_socket]` section

* `use_socked`: This value is of type boolean.
    If it is set to a truthy value, the client takes a socket file descriptor
    from systemd and on success writes the key into the socket.

### `[stdout]` section

* `use_stdout`: This value is of type boolean and specifies whether the
    key should be printed to the standard output on success.
