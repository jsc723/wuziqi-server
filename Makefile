CC=g++
CFLAGS=-I.

all: server.cpp
	$(CC) -g -o server server.cpp $(CFLAGS)