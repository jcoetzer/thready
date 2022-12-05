#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <pthread.h>
#include <unistd.h>

#include "thready_data.h"
#include "thready_logger.h"

#include "tcp_forward.h"

#define MAX_THREADS 8

void usage(char *pname);
int run_thready(int pnum, int tcount);
void* forward_port_thread(void * pnums);

int verbose = 0;

int main(int argc, char *argv[])
{
    int rc, pnum, tcount;

    pnum = tcount = 0;

    if (argc == 4)
    {
        if (0 == strcmp(argv[1], "-vvv"))
        {
            verbose = 3;
            pnum = atoi(argv[2]);
            tcount = atoi(argv[3]);
        }
        else if (0 == strcmp(argv[1], "-vv"))
        {
            verbose = 2;
            pnum = atoi(argv[2]);
            tcount = atoi(argv[3]);
        }
        else if (0 == strcmp(argv[1], "-v"))
        {
            verbose = 1;
            pnum = atoi(argv[2]);
            tcount = atoi(argv[3]);
        }
        else
        {
            usage(basename(argv[0]));
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
    log_warning("Open %d ports starting with %d (%d)\n", tcount, pnum, verbose);
    rc = run_thready(pnum, tcount);
    return rc;
}

void usage(char *pname)
{
    printf("Run circle of servers:\n");
    printf("\t%s [-v|-vv|-vvv] <PORT> <COUNT>\n", pname);
    exit(EXIT_FAILURE);
}

int run_thready(int pnum, int tcount)
{
    pthread_t thrd[MAX_THREADS];
    struct thread_ports port_nums[MAX_THREADS];
    int thready[MAX_THREADS];
    int i;

    if (0 == pnum)
    {
        fprintf(stderr, "Invalid port number");
        return 1;
    }

    log_warning("Forward ports %d to %d\n", pnum, pnum+tcount-1);

    for (i=0; i<tcount; i++)
    {
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

    log_warning("Start thread to listen on %d and forward on %d\n", rx_port, tx_port);

    run_forward_server(rx_port, tx_port, server_name);
}
