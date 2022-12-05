#include <stdio.h>
#include <string.h>

#include "thready_logger.h"

void logger(char * buf, int len)
{
    static char logbuf[1024 * 4 + 1];
    if (verbose)
    {
        strncpy(logbuf, buf, len);
        logbuf[len] = 0;
        printf("RX> %s\n", logbuf);
    }
}
