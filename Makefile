CC=gcc
CFLAGS=-I.

all: thready server client portforward numbers

numbers: numbers.c
	gcc -g numbers.c -o numbers

THRY_OBJS = tcp_forward.o thready.o thready_logger.o tcp_client.o

SRVR_OBJS = tcp_server.o server.o

CLNT_OBJS = tcp_client.o client.o

FORW_OBJS = tcp_forward.o portforward.o thready_logger.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

thready: $(THRY_OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

server: $(SRVR_OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

client: $(CLNT_OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

portforward: $(FORW_OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f $(THRY_OBJS) $(SRVR_OBJS) $(CLNT_OBJS) $(FORW_OBJS)
	rm -f thready server client portforward numbers
