#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define on_error(...) { fprintf(stderr, __VA_ARGS__); fflush(stderr); exit(1); }

#define MAX_THREADS 8
#define MESSAGE_SIZE 1024

void usage(char * pname);
void *thread_check(void * ptr);
int run_thread_check(int tcount);
int get_server(int port);
int listen_ports(int pnum, int tcount);
void *listen_port(void * pnum);
int forward_ports(int pnum, int tcount);
void *forward_port(void * pnum);
int run_server(int pnum);
int run_client(int pnum, char * msg);

struct thread_msg
{
	int thread_num;
	int sleep_time;
	int port_num;
	char message[MESSAGE_SIZE+1];
};

struct thread_ports
{
	int rx_port;
	int tx_port;
};

int main(int argc, char *argv[])
{
	int rc, pnum, tcount;
	if (argc < 3)
	{
		usage(basename(argv[0]));
	}

	if (0 == strcmp(argv[1], "-x"))
	{
		tcount = atoi(argv[2]);
		rc = run_thread_check(tcount);
	}
	else if (0 == strcmp(argv[1], "-l"))
	{
		if (argc != 4)
		{
			usage(basename(argv[0]));
		}
		pnum = atoi(argv[2]);
		tcount = atoi(argv[3]);
		rc = listen_ports(pnum, tcount);
	}
	else if (0 == strcmp(argv[1], "-c"))
	{
        ;
    }
	else if (0 == strcmp(argv[1], "-s"))
	{
		if (argc != 3)
		{
			usage(basename(argv[0]));
		}
		pnum = atoi(argv[2]);
        rc = run_server(pnum);
    }
	else
	{
		pnum = atoi(argv[1]);
		tcount = atoi(argv[2]);
		rc = forward_ports(pnum, tcount);
	}
	return rc;
}

void usage(char *pname)
{
	printf("Run circle:\n");
	printf("\t%s <PORT> <COUNT>\n", pname);
	printf("Check:\n");
	printf("\t%s -x <COUNT>\n", pname);
	printf("Listen and echo:\n");
	printf("\t%s -l <PORT> <COUNT>\n", pname);
	printf("Client:\n");
	printf("\t%s -c <PORT> <MESSAGE>\n", pname);
	printf("Server:\n");
	printf("\t%s -s <PORT>\n", pname);
	exit(1);
}

void *thread_check(void *ptr)
{
	struct thread_msg * msg;

	msg = (struct thread_msg *) ptr;
	printf("%s (sleep %d)\n", msg->message, msg->sleep_time);
	sleep(msg->sleep_time);
	printf("%s done\n", msg->message);
}

int run_thread_check(int tcount)
{
	pthread_t thrd[MAX_THREADS];
	struct thread_msg msg[MAX_THREADS];
	int thready[MAX_THREADS];
	int i, seed;

	seed = (int) time(NULL);

	for (i=0; i<tcount; i++)
	{
		msg[i].thread_num = i+1;
		msg[i].port_num = i;
		msg[i].sleep_time = rand() % 30;
		snprintf(msg[i].message, MESSAGE_SIZE, "thread %d of %d", i+1, tcount);
		thready[i] = pthread_create( &thrd[i], NULL, thread_check, (void*) &msg[i]);
		printf("thread %d returns: %d\n", i+1, thready[i]);
	}

	for (i=0; i<tcount; i++)
	{
		pthread_join( thrd[i], NULL);
	}
	return 0;
}

int get_server(int port)
{
	int server_fd, err;
	struct sockaddr_in server;
	int opt_val = 1;

	printf("Listen on port %d\n", port);

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) on_error("Could not create socket\n");

	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof opt_val);

	err = bind(server_fd, (struct sockaddr *) &server, sizeof(server));
	if (err < 0) on_error("Could not bind socket\n");

    return server_fd;
}

int run_server(int rx_port)
{
	int server_fd, err, read;
	char * end;
	char buf[BUFFER_SIZE];

	server_fd = get_server(rx_port);
	if (server_fd < 0) on_error("Could not create socket server\n");

	err = listen(server_fd, 128);
	if (err < 0) on_error("Could not listen on socket\n");

	printf("Server is listening on %d\n", rx_port);

    while (1)
    {
        err = listen(server_fd, 128);
        if (err < 0) on_error("Could not listen on socket\n");

        read = recv(server_fd, buf, BUFFER_SIZE, 0);

        // done reading
        if (!read) break;
        else if (read < 0)
        {
            sleep(1);
            // on_error("Client read failed on port %d\n", rx_port);
        }
        else
        {
            // Trim trailing space
            buf[read] = 0;
            end = buf + strlen(buf) - 1;
            while(end > buf && isspace((unsigned char)*end)) end--;

            // Write new null terminator character
            end[1] = '\0';
            printf("RX%d-%d> %s \n", rx_port, read, buf);
        }
    }
    return 0;
}

int get_client(int port)
{
	int server2_fd;
	struct sockaddr_in server2;
	int opt_val = 1;

	server2_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server2_fd < 0) on_error("Could not create socket 2 on port %d\n", port);

	server2.sin_family = AF_INET;
	server2.sin_port = htons(port);
	if (inet_pton(AF_INET, "127.0.0.1", &server2.sin_addr) <= 0)
	{
		on_error("Invalid address\n");
	}

	setsockopt(server2_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof opt_val);
    return server2_fd;
}

int run_client(int pnum, char * msg)
{
    return 0;
}

int listen_ports(int pnum, int tcount)
{
	pthread_t thrd[MAX_THREADS];
	int port_nums[MAX_THREADS];
	int thready[MAX_THREADS];
	int i;

	printf("Listen on ports %d to %d\n", pnum, pnum+tcount-1);

	for (i=0; i<tcount; i++)
	{
		// listen_port(pnum+i);
		port_nums[i] = pnum + i;
		thready[i] = pthread_create( &thrd[i], NULL, listen_port, (void*) &port_nums[i]);
	}

	for (i=0; i<tcount; i++)
	{
		pthread_join( thrd[i], NULL);
	}
	return 0;
}

void* listen_port(void * port_num)
{
	int server_fd, client_fd, err;
	struct sockaddr_in server, client;
	char buf[BUFFER_SIZE];
	char * end;
	int port;
	int *pn;

	pn = port_num;
	port = *pn;
	printf("Listen on port %d\n", port);

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) on_error("Could not create socket\n");

	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	int opt_val = 1;
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof opt_val);

	err = bind(server_fd, (struct sockaddr *) &server, sizeof(server));
	if (err < 0) on_error("Could not bind socket\n");

	err = listen(server_fd, 128);
	if (err < 0) on_error("Could not listen on socket\n");

	printf("Server is listening on %d\n", port);

	while (1)
	{
		socklen_t client_len = sizeof(client);
		client_fd = accept(server_fd, (struct sockaddr *) &client, &client_len);

		if (client_fd < 0) on_error("Could not establish new connection\n");

		while (1)
		{
			int read = recv(client_fd, buf, BUFFER_SIZE, 0);

			if (!read) break; // done reading
			if (read < 0) on_error("Client read failed\n");
			buf[read] = 0;
			// Trim trailing space
			end = buf + strlen(buf) - 1;
			while(end > buf && isspace((unsigned char)*end)) end--;

			// Write new null terminator character
			end[1] = '\0';
			printf("RX%d-%d> %s \n", port, read, buf);

			err = send(client_fd, buf, read, 0);
			if (err < 0) on_error("Client write failed\n");

			if (0 == strcmp(buf, "bye"))
			{
				printf("Stop server listening on %d\n", port);
				return NULL;
			}
		}
	}

	return NULL;
}

int forward_ports(int pnum, int tcount)
{
	pthread_t thrd[MAX_THREADS];
	struct thread_ports port_nums[MAX_THREADS];
	int thready[MAX_THREADS];
	int i;

	printf("Forward ports %d to %d\n", pnum, pnum+tcount-1);

	for (i=0; i<tcount; i++)
	{
		// listen_port(pnum+i);
		port_nums[i].rx_port = pnum + i;
		if (tcount == i+1)
			port_nums[i].tx_port = pnum;
		else
			port_nums[i].tx_port = pnum + i + 1;
		thready[i] = pthread_create( &thrd[i], NULL, forward_port, (void*) &port_nums[i]);
	}

	for (i=0; i<tcount; i++)
	{
		pthread_join( thrd[i], NULL);
	}
	return 0;
}

void* forward_port(void * pnums)
{
	int server_fd, server2_fd, client_fd, err;
	struct sockaddr_in server, server2, client;
	char buf[BUFFER_SIZE];
	char * end;
	int rx_port, tx_port;
	int opt_val;

	struct thread_ports * port_nums = pnums;

	rx_port = port_nums->rx_port;
	tx_port = port_nums->tx_port;
	printf("Listen on port %d, send on %d\n", rx_port, tx_port);

    /*
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) on_error("Could not create socket\n");

	server.sin_family = AF_INET;
	server.sin_port = htons(rx_port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	opt_val = 1;
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof opt_val);

	err = bind(server_fd, (struct sockaddr *) &server, sizeof(server));
	if (err < 0) on_error("Could not bind socket\n");
	*/
	server_fd = get_server(rx_port);
	if (server_fd < 0) on_error("Could not create socket server\n");

	sleep(7);

    /*
	server2_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server2_fd < 0) on_error("Could not create socket 2 on port %d\n", tx_port);

	server2.sin_family = AF_INET;
	server2.sin_port = htons(tx_port);
	if (inet_pton(AF_INET, "127.0.0.1", &server2.sin_addr) <= 0)
	{
		on_error("Invalid address\n");
	}

	setsockopt(server2_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof opt_val);
    */
    server2_fd = get_client(tx_port);

	// err = bind(server2_fd, (struct sockaddr *) &server2, sizeof(server2));
	// if (err < 0) on_error("Could not bind socket 2 on port %d\n", tx_port);

	err = listen(server_fd, 128);
	if (err < 0) on_error("Could not listen on socket\n");

	printf("Server is listening on %d\n", rx_port);

	sleep(7);

	while (1)
	{
		socklen_t client_len = sizeof(client);
		// client_fd = accept(server2_fd, (struct sockaddr *) &client, &client_len);
		client_fd = connect(server2_fd, (struct sockaddr*) &server2, sizeof server2);

		if (client_fd < 0) on_error("Could not establish new client connection on port %d\n", tx_port);

		while (1)
		{
			int read = recv(client_fd, buf, BUFFER_SIZE, 0);

			if (!read) break; // done reading
			if (read < 0) on_error("Client read failed on port %d\n", rx_port);

			// Trim trailing space
			buf[read] = 0;
			end = buf + strlen(buf) - 1;
			while(end > buf && isspace((unsigned char)*end)) end--;

			// Write new null terminator character
			end[1] = '\0';
			printf("RX%d-%d> %s \n", rx_port, read, buf);

            sleep(1);

			err = send(client_fd, buf, read, 0);
			if (err < 0) on_error("Client write failed\n");

			if (0 == strcmp(buf, "bye"))
			{
				printf("Stop server listening on %d\n", rx_port);
				return NULL;
			}
			printf("TX%d-%d> %s \n", tx_port, read, buf);
		}
		close(client_fd);
	}

	return NULL;
}
