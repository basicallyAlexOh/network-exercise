CC := clang
CFLAGS := -g
LDFLAGS := -pthread

all: server client

clean:
	rm -rf server client server.dSYM client.dSYM

server: server.c socket.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o server server.c 

client: client.c
	$(CC) $(CFLAGS) -o client client.c

