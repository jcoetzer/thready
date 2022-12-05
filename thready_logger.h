#ifndef THREADY_LOGGER_H
#define THREADY_LOGGER_H

/*?
#define log_warning(...) { if (verbose>=1) { printf(__VA_ARGS__); fflush(stdout); } }
#define log_info(...) { if (verbose>=2) { printf(__VA_ARGS__); fflush(stdout); } }
#define log_debug(...) { if (verbose>=3) { printf(__VA_ARGS__); fflush(stdout); } }
?*/

#define log_warning printf
#define log_info printf
#define log_debug printf

#define on_error(...) { fprintf(stderr, __VA_ARGS__); fflush(stderr); exit(EXIT_FAILURE); }

void log_data(char * pfix, char * buf, int len);

#endif
