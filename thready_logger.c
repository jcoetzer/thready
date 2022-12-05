#include <stdio.h>
#include <string.h>
#include <time.h>

#include "thready_logger.h"

extern int verbose;

void log_data(char * pfix, char * buf, int len)
{
    //? static
    char logbuf[1024 * 4 + 1];
    struct tm * lt;
    if (verbose >= 3)
    {
        lt = localtime(NULL);
        strncpy(logbuf, buf, (size_t) len);
        logbuf[len] = '\0';
        printf("%02d:%02d:%02d %s> %s\n",
               lt->tm_hour, lt->tm_min, lt->tm_sec, pfix, logbuf);
    }
}
