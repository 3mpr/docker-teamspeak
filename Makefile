CC=gcc
CFLAGS=-static -Wall -ansi -pedantic
DEPS_DIR=iniparser
DEPS=:libiniparser.a
ARTEFACTS=ts3w
DOCKER_NAME=3mpr/teamspeak:latest

ts3w: ./src/ts3w.c
	$(CC) ./src/ts3w.c -L$(DEPS_DIR) -l$(DEPS) $(CFLAGS) -g -o ts3w

clean: $(ARTEFACTS)
	rm $(ARTEFACTS)

docker:
	docker build -t $(DOCKER_NAME) .

all:
	make clean
	make ts3w
	make docker

update:
	make ts3w
	docker cp ./ts3w $$(docker ps -l -q):/usr/local/bin/ts3w
