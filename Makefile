CC=gcc
CFLAGS=-I.

all: thready server client

thready: thready.c
	gcc thready.c -o thready

SRVR_OBJS = tcp_server.o server.o

CLNT_OBJS = tcp_client.o client.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: $(SRVR_OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

client: $(CLNT_OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f thready server client
