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

#define _XOPEN_SOURCE 500
char *strdup(const char *s);

void usage(char * pname);

// Entry point
int main(int argc, char * argv[])
{
    int sockfd;
    int port = PORT;
    char * msg = NULL;
    int echo = 0;
    FILE *f;
    long fsize;

    if (argc == 4)
    {
        port = atoi(argv[1]);
        if (0 != strcmp(argv[2], "-f"))
        {
            usage(basename(argv[0]));
        }
        printf("Read file %s\n", argv[3]);
        f = fopen(argv[3], "rb");
        fseek(f, 0, SEEK_END);
        fsize = ftell(f);
        rewind(f);

        msg = malloc(fsize);
        fread(msg, fsize, 1, f);
        fclose(f);
    }
    else if (argc == 3)
    {
        port = atoi(argv[1]);
        msg = strdup(argv[2]);
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

    if (msg)
    {
        free(msg);
    }

    return 0;
}

void usage(char * pname)
{
    printf("Usage:\n");
    printf("\t%s <PORT>\n", pname);
    printf("\t%s <PORT> <MESSAGE>\n", pname);
    printf("\t%s <PORT> -f <FILE>\n", pname);
    exit(EXIT_FAILURE);
}
