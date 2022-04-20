CC = gcc
CC_FLAGS = -O0 -g -Wall -Wextra -fanalyzer -lcurl -lcrypto -lssl

all: bin/client

SRC = \
	src/cli.c \
	src/error.c \
	src/http-client.c \
	src/sockets.c
OBJECTS = $(patsubst src/%.c,obj/%.o,$(SRC))

obj/%.o: src/%.c src/%.h 
	$(CC) $(CC_FLAGS) -c $< -o $@

bin/client: src/main.c $(OBJECTS)
	$(CC) $(CC_FLAGS) -o $@ $< $(OBJECTS)

clean:
	@echo $(OBJECTS)
	rm -rf bin/client
	rm -rf obj/*.o

.PHONY: all clean
