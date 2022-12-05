#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <pthread.h>
#include <unistd.h>

#include "thready_data.h"

#include "tcp_client.h"
#include "tcp_forward.h"

#define MAX_THREADS 8

void usage(char *pname);
int run_thready(int pnum, int tcount, char * msg);
void* forward_port_thread(void * pnums);

int verbose = 0;

int main(int argc, char *argv[])
{
    int rc, pnum, tcount;
    char message[128];

    pnum = tcount = 0;
    message[0] = 0;

    if (argc == 5)
    {
        if (strcmp(argv[1], "-v"))
        {
            usage(basename(argv[0]));
        }
        verbose = 1;
        pnum = atoi(argv[2]);
        tcount = atoi(argv[3]);
        strcpy(message, argv[4]);
    }
    else if (argc == 4)
    {
        if (0 == strcmp(argv[1], "-v"))
        {
            verbose = 1;
            pnum = atoi(argv[2]);
            tcount = atoi(argv[3]);
        }
        else
        {
            pnum = atoi(argv[1]);
            tcount = atoi(argv[2]);
            strcpy(message, argv[3]);
        }
    }
    else if (argc == 3)
    {
        pnum = atoi(argv[1]);
        tcount = atoi(argv[2]);
    }
    else
    {
        usage(basename(argv[0]));
    }
    rc = run_thready(pnum, tcount, message);
    return rc;
}

void usage(char *pname)
{
    printf("Run circle of servers:\n");
    printf("\t%s [-v] <PORT> <COUNT>\n", pname);
    printf("\t%s [-v] <PORT> <COUNT> <MESSAGE>\n", pname);
    exit(1);
}

int run_thready(int pnum, int tcount, char * msg)
{
    pthread_t thrd[MAX_THREADS];
    struct thread_ports port_nums[MAX_THREADS];
    int thready[MAX_THREADS];
    int i, sockfd;

    if (0 == pnum)
    {
        fprintf(stderr, "Invalid port number");
        return 1;
    }

    printf("Forward ports %d to %d\n", pnum, pnum+tcount-1);

    for (i=0; i<tcount; i++)
    {
        // listen_port(pnum+i);
        port_nums[i].rx_port = pnum + i;
        if (tcount == i+1)
        {
            port_nums[i].tx_port = pnum;
        }
        else
        {
            port_nums[i].tx_port = pnum + i + 1;
        }
        thready[i] = pthread_create(&thrd[i],
                                    NULL,
                                    forward_port_thread,
                                    (void*) &port_nums[i]);
    }

    for (i=0; i<tcount; i++)
    {
        pthread_join( thrd[i], NULL);
    }

    if (msg[0])
    {
        // do the chat
        sockfd = get_client(pnum);
        send_client(sockfd, msg, 0);
        // close socket
        close(sockfd);
    }

    return 0;
}

void* forward_port_thread(void * pnums)
{
    int tx_port, rx_port;
    char server_name[64];
    struct thread_ports * port_nums = pnums;

    strcpy(server_name, "localhost");
    rx_port = port_nums->rx_port;
    tx_port = port_nums->tx_port;

    run_forward_server(rx_port, tx_port, server_name);
}

