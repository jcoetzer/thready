#ifndef SERVER_H
#define SERVER_H

#define MAX 80
#define PORT 8080

int get_server(int port);
void run_server(int connfd, int echo);

#endif
