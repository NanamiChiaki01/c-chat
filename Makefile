
CC=gcc
FLAGS=-Wall -Werror -g -fsanitize=address
LDFLAGS = -lm
INCLUDE=-I include

all: $(SERVER) $(CLIENT)

clean:
	rm -f $(OBJS)
	rm -f $(TARGET)
	rm -rf *.out

server: src/server.c
	$(CC) $^ $(INCLUDE) $(CFLAGS) $(LDFLAGS) -o $@

client: src/client.c
	$(CC) $^ $(INCLUDE) $(CFLAGS) $(LDFLAGS) -o $@
