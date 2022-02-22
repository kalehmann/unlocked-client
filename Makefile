CC = gcc
CC_FLAGS = -Wall -Wextra -static

all: bin/client

bin/client: src/client.c
	$(CC) $(CC_FLAGS) -o $@ $<

clean:
	rm -rf bin/client

.PHONY: all clean
