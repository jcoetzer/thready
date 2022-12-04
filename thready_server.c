#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <libgen.h>
#include <pthread.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>

#include "thready_data.h"
#include "thready_server.h"
#include "tcp_server.h"
#include "tcp_client.h"

int run_check(int pnum, int tcount)
{
    return listen_ports(pnum, tcount);
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
    int sockfd;
    int port;
    int *pn;

    pn = port_num;
    port = *pn;
    printf("Listen on port %d\n", port);

    sockfd = get_server(port);

    // chat between client and server
    run_server(sockfd, 1);

    // After chatting close the socket
    close(sockfd);
}

/*
// Forward message to client.
void forward_server(int sockfd, int port)
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
        bzero(buff, MAX);

        // read message from client
        read(connfd, buff, sizeof(buff));

        if (strlen(buff))
        {
            printf("RX> %s\n", buff);

            sockfd = get_client(port);
            send_client(sockfd, buff, 0);
            close(sockfd);

            // if message contains "bye" or "exit" then exit and end chat
            if ((strncmp("exit", buff, 4) == 0) || (strncmp("bye", buff, 3) == 0))
            {
                printf("Server exit\n");
                break;
            }
        }
        sleep(SLEEP_TIME);
    }
    printf("Server done\n");
}
*/

int run_thready(int pnum, int tcount)
{
    return forward_ports(pnum, tcount);
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
    int sockfd;
    int tx_port, rx_port;
    int *pn;

    struct thread_ports * port_nums = pnums;

    rx_port = port_nums->rx_port;
    tx_port = port_nums->tx_port;
    printf("Listen on port %d, send on %d\n", rx_port, tx_port);

    sockfd = get_server(rx_port);

    // chat between client and server
    run_forward(sockfd, tx_port);

    // After chatting close the socket
    close(sockfd);
}

// Chat between client and server.
void run_forward(int sockfd, int port)
{
    char buff[MAX];
    int n, connfd;
    unsigned int len;
    struct sockaddr_in cli;

    len = sizeof(cli);

    // infinite loop for chat
    for (;;)
    {
        // Accept the data packet from client and verification
        connfd = accept(sockfd, (struct sockaddr*)&cli, &len);
        if (connfd < 0)
        {
            printf("Server accept failed: %s\n", strerror(errno));
            exit(1);
        }
        bzero(buff, MAX);

        // read message from client
        read(connfd, buff, sizeof(buff));

        if (strlen(buff))
        {
            printf("RX> %s\n", buff);

            sockfd = get_client(port);
            send_client(sockfd, buff, 0);
            close(sockfd);

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
