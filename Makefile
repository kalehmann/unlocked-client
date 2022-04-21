CC = gcc
CC_FLAGS = \
	-O0 -g \
	-Wall -Wextra \
	-fanalyzer -fsanitize=undefined \
	-lcurl -lcrypto -lssl \
	-Ivendor/cJSON

all: bin/client

SRC = \
	src/cli.c \
	src/client.c \
	src/error.c \
	src/https-client.c \
	src/log.c \
	src/sockets.c
HEADERS = $(patsubst src/%.c,src/%.h,$(SRC))
OBJECTS = $(patsubst src/%.c,obj/%.o,$(SRC))
OBJECTS += obj/cJSON.o

obj/cJSON.o: vendor/cJSON/cJSON.c vendor/cJSON/cJSON.h
	$(CC) $(CC_FLAGS) -c $< -o $@

obj/%.o: src/%.c $(HEADERS)
	$(CC) $(CC_FLAGS) -c $< -o $@

bin/client: src/main.c $(OBJECTS) $(HEADERS)
	$(CC) $(CC_FLAGS) -o $@ $< $(OBJECTS)

clean:
	@echo $(OBJECTS)
	rm -rf bin/client
	rm -rf obj/*.o

.PHONY: all clean
