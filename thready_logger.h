#ifndef THREADY_LOGGER_H
#define THREADY_LOGGER_H

extern int verbose;

#define on_error(...) { fprintf(stderr, __VA_ARGS__); fflush(stderr); exit(1); }

void logger(char * buf, int len);

#endif
