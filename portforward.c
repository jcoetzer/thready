#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <libgen.h>

int verbose = 1;

#include "tcp_forward.h"


void usage(char * pname);
void parse_arguments(int argc, char **argv, int *server_port,
                     char **forward_name, int *forward_port);

int main(int argc,
         char **argv)
{
    int server_port, forward_port;
    char *forward_name;

    parse_arguments(argc, argv, &server_port, &forward_name, &forward_port);

    run_forward_server(server_port, forward_port, forward_name);

    return 0;
}

void usage(char * pname)
{
    fprintf(stderr, "Syntax:  %s listen_port forward_host [forward_port]\n",
            pname);
    exit(1);
}

/**
 * Argument parsing and validation
 */
void parse_arguments(int argc,
                     char **argv,
                     int *server_port,
                     char **forward_name,
                     int *forward_port)
{
    if (argc < 4)
    {
        fprintf(stderr, "Not enough arguments\n");
        usage(basename(argv[0]));
    }

    *server_port = atoi(argv[1]);
    if (*server_port < 1)
    {
        fprintf(stderr, "Listen port is invalid\n");
        exit(1);
    }

    *forward_name = argv[2];

    *forward_port = atoi(argv[3]);
    if (*forward_port < 1)
    {
        fprintf(stderr, "Forwarding port is invalid\n");
        exit(1);
    }
}
