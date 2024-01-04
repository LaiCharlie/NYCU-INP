
CC      = gcc
PROGS   = server
UID     = $(shell stat -c "%u:%g" /build/Makefile 2>/dev/null || stat -c "%u:%g" ./Makefile 2> /dev/null)
LDLIBS  = -lssl -lcrypto

all: server https

%.o: %.c
	$(CC) -c -g -Wall $<

server: server.o
	$(CC) -o $@ -static server.c
	chown -R "$(UID)" *.o server

https: server_https.o
	$(CC) -o $@ -static server_https.c $(LDLIBS)
	chown -R "$(UID)" *.o https

clean:
	rm -f *.o $(PROGS)
