#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <libgen.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "tcp_client.h"

void usage(char * pname);

// Entry point
int main(int argc, char ** argv)
{
    int sockfd;
    int port = PORT;
    char * msg = NULL;
    int echo = 0;

    if (argc == 3)
    {
        port = atoi(argv[1]);
        msg = argv[2];
    }
    else if (argc == 2)
    {
        port = atoi(argv[1]);
    }
    else
    {
        usage(basename(argv[0]));
    }

    sockfd = get_client(port);

    // do the chat
    if (msg)
    {
        send_client(sockfd, msg, echo);
    }
    else
    {
        run_client(sockfd);
    }

    // close socket
    close(sockfd);

    return 0;
}

void usage(char * pname)
{
    printf("Usage:\n");
    printf("\t%s <PORT>\n", pname);
    printf("\t%s <PORT> <MESSAGE>\n", pname);
    exit(1);
}
