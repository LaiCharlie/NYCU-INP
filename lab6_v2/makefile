CC = gcc
PP = g++
CFLAGS = -static

all: server client

server: server.cpp
	$(PP) $(CFLAGS) -o $@ $^

client: client.cpp
	$(PP) $(CFLAGS) -o $@ $^

udpechosrv: udpechosrv.c
	$(CC) $(CFLAGS) -o $@ $^

udpping: udpping.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f server client
