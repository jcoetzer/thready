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

int get_client(int port)
{
    int sockfd;
    struct sockaddr_in servaddr;

    printf("Connect client on port %d\n", port);

    // socket creation and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("Socket creation failed\n");
        exit(0);
    }
    else
    {
        printf("Socket successfully created\n");
    }
    bzero(&servaddr, sizeof(servaddr));

    // assign IP and PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(port);

    // connect client socket to server socket
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("Connection to server failed\n");
        exit(0);
    }
    else
    {
        printf("Connected to server\n");
    }
    return sockfd;
}

void run_client(int sockfd)
{
    char buff[MAX];
    int n;

    for (;;)
    {
        bzero(buff, sizeof(buff));
        printf("Message : ");
        n = 0;
        while ((buff[n++] = getchar()) != '\n')
        {
            ;
        }
        write(sockfd, buff, sizeof(buff));
        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff));
        printf("From Server : %s", buff);
        if (((strncmp(buff, "exit", 4)) == 0) ||
            ((strncmp(buff, "bye", 3)) == 0))
        {
            printf("Client exit\n");
            break;
        }
    }
}

void send_client(int sockfd, char * msg, int echo)
{
    char buff[MAX+1];

    strncpy(buff, msg, MAX);
    printf("To Server : %s\n", buff);
    write(sockfd, buff, sizeof(buff));
    if (echo)
    {
        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff));
        printf("From Server : %s\n", buff);
    }
}
