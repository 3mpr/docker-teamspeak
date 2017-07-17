CC=gcc
CFLAGS=-static -Wall -ansi -pedantic
DEPS_DIR=iniparser
DEPS=:libiniparser.a

ts3w: ./src/ts3w.c
	$(CC) ./src/ts3w.c -L$(DEPS_DIR) -l$(DEPS) $(CFLAGS) -g -o ts3w
