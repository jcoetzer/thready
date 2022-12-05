#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <libgen.h>

#include "tcp_server.h"

void usage(char * pname);

// Entry point
int main(int argc, char ** argv)
{
    int sockfd;
    int port = PORT;
    int echo = 0;

    if (argc == 2)
    {
        if (0 == strcmp(argv[1], "-h"))
        {
            usage(basename(argv[0]));
        }
        else if (0 == strcmp(argv[1], "-e"))
        {
            echo = 1;
        }
        else
        {
            port = atoi(argv[1]);
        }
    }
    else if (argc == 3)
    {
        if (0 == strcmp(argv[1], "-e"))
        {
            echo = 1;
            port = atoi(argv[2]);
        }
        else
        {
            usage(basename(argv[0]));
        }
    }

    sockfd = get_server(port);

	// chat between client and server
	run_server(sockfd, echo);

	// close socket
	close(sockfd);

    return 0;
}

void usage(char * pname)
{
    printf("Usage:\n");
    printf("\t%s [-e] [PORT]\n", pname);
    exit(EXIT_FAILURE);
}
