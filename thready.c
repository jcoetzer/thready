#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <pthread.h>
#include <unistd.h>

#include "thready_data.h"
#include "thready_server.h"
#include "tcp_server.h"

int main(int argc, char *argv[])
{
    int rc, pnum, tcount;
    if (argc == 3)
    {
        pnum = atoi(argv[1]);
        tcount = atoi(argv[2]);
        rc = run_thready(pnum, tcount);
    }
    else if (argc == 4)
    {
        if (strcmp(argv[1], "-x"))
        {
            usage(basename(argv[0]));
        }
        pnum = atoi(argv[2]);
        tcount = atoi(argv[3]);
        rc = run_check(pnum, tcount);
    }
    else
    {
        usage(basename(argv[0]));
    }
    return rc;
}

void usage(char *pname)
{
    printf("Run circle of servers:\n");
    printf("\t%s <PORT> <COUNT>\n", pname);
    printf("\t%s -x <PORT> <COUNT>\n", pname);
    exit(1);
}
