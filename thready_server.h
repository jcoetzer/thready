#ifndef THREADY_SERVER_H
#define THREADY_SERVER_H

#define MAX_THREADS 8
#define MESSAGE_SIZE 1024

void usage(char *pname);

int run_check(int pnum, int tcount);
void* listen_port(void * port_num);
int listen_ports(int pnum, int tcount);

// void forward_server(int sockfd, int port);
int run_thready(int pnum, int tcount);
int forward_ports(int pnum, int tcount);
void* forward_port(void * pnums);
void run_forward(int sockfd, int port);

#endif
