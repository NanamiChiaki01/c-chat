CC = gcc
CFLAGS = -Wall -Wextra -g -pthread
SERVER_OBJS = server.o user.o chatroom.o
CLIENT_OBJS = client.o
TARGETS = server client

all: $(TARGETS)

server: $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o $@ $(SERVER_OBJS)

client: $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o $@ $(CLIENT_OBJS)

server.o: server.c user.h chatroom.h
	$(CC) $(CFLAGS) -c server.c

user.o: user.c user.h chatroom.h
	$(CC) $(CFLAGS) -c user.c

chatroom.o: chatroom.c chatroom.h
	$(CC) $(CFLAGS) -c chatroom.c

client.o: client.c
	$(CC) $(CFLAGS) -c client.c

clean:
	rm -f $(TARGETS) *.o

.PHONY: all clean
