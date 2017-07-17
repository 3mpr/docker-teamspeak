CC=gcc
CFLAGS=-Wall -pedantic -I.

ts3w: ts3w.c
	$(CC) -o ts3w ts3w.c $(CFLAGS)
