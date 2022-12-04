CC=gcc
CFLAGS=-I.

all: thready server client portforward

#thready: thready.c
#	gcc thready.c -o thready

THRY_OBJS = tcp_server.o tcp_client.o thready.o thready_server.o

SRVR_OBJS = tcp_server.o server.o

CLNT_OBJS = tcp_client.o client.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

thready: $(THRY_OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

server: $(SRVR_OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

client: $(CLNT_OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

portforward: portforward.c
	gcc portforward.c -o portforward

clean:
	rm -f $(THRY_OBJS) $(SRVR_OBJS) $(CLNT_OBJS) $(STUF_OBJS)
	rm -f thready server client portforward
