#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#define MAX 80
#define PORT 8080

int get_client(int port);
void run_client(int sockfd);
void send_client(int sockfd, char * msg, int echo);

#endif
