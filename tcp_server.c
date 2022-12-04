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

int get_server(int port)
{
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    printf("Open server on port %d\n", port);

    // socket creation and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("Socket creation failed\n");
        exit(1);
    }
    else
    {
        printf("Socket successfully created\n");
    }
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0)
    {
        printf("Socket bind failed\n");
        exit(1);
    }
    else
    {
        printf("Socket bind successful\n");
    }

    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0)
    {
        printf("Listen failed\n");
        exit(1);
    }
    else
    {
        printf("Server listening on port %d\n", port);
    }

    return sockfd;
}

// Function for chat between client and server.
void run_server(int sockfd, int echo)
{
    char buff[MAX];
    int n, connfd, len;
    struct sockaddr_in cli;

    len = sizeof(cli);

    // infinite loop for chat
    for (;;)
    {
        // Accept the data packet from client and verification
        connfd = accept(sockfd, (struct sockaddr*)&cli, &len);
        if (connfd < 0)
        {
            printf("Server accept failed\n");
            exit(1);
        }
        /*
        else
        {
            printf("Server accept client\n");
        }
        */
        bzero(buff, MAX);

        // read message from client
        read(connfd, buff, sizeof(buff));

        if (strlen(buff))
        {
            printf("RX> %s\n", buff);
            if (echo)
            {
                // send buffer to client
                write(connfd, buff, sizeof(buff));
            }

            // if message contains "bye" or "exit" then exit and end chat
            if ((strncmp("exit", buff, 4) == 0) || (strncmp("bye", buff, 3) == 0))
            {
                printf("Server exit\n");
                break;
            }
        }
    }
    printf("Server done\n");
}
