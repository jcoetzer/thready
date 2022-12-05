/* Licensed under MIT
 * Original source: https://github.com/antialize/utils/blob/master/portforward.c
 * It was later licensed:  https://github.com/antialize/utils/issues/1
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <netdb.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

#include "thready_logger.h"

extern int verbose;

/**
 * Send the traffic from src socket to dst socket
 *
 * When done or on any error, this dies and will kill the forked process.
 */
void comms(int src,
           int dst,
           int server_port)
{
    char buf[1024 * 4];
    char pfix[32];
    int r, i, j;

    sprintf(pfix, "RX%d", server_port);

    r = read(src, buf, 1024 * 4);
    while (r > 0)
    {
        log_warning("Received %d bytes on port %d\n", r, server_port);
        log_data(pfix, buf, r);
        i = 0;
        while (i < r)
        {
            log_data("TX", buf + i, r - 1);
            j = write(dst, buf + i, r - i);
            if (j == -1)
            {
                // TODO check for error EPIPE
                log_error("write failed: %s\n", strerror(errno));
            }
            i += j;
        }
        r = read(src, buf, 1024 * 4);
    }

    if (r == -1)
    {
        log_error("read failed: %s\n", strerror(errno));
    }

    shutdown(src, SHUT_RD);
    shutdown(dst, SHUT_WR);
    close(src);
    close(dst);
    exit(0);
}


/**
 * Opens a connection to the destination.
 *
 * On any error, this dies and will kill the forked process.
 */
int open_forwarding_socket(char *forward_name,
                           int forward_port)
{
    int forward_socket;
    struct hostent *forward;
    struct sockaddr_in forward_address;

    forward = gethostbyname(forward_name);
    if (forward == NULL)
    {
        log_error("gethostbyname failed: %s\n", strerror(errno));
    }

    bzero((char *) &forward_address, sizeof(forward_address));
    forward_address.sin_family = AF_INET;
    // TODO No member h_addr in struct hostent
    bcopy((char *)forward->h_addr,
          (char *) &forward_address.sin_addr.s_addr,
          forward->h_length);
    forward_address.sin_port = htons(forward_port);

    // Create a new socket
    forward_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (forward_socket == -1)
    {
        log_error("Could not create socket with port %d: %s\n",
                 forward_port, strerror(errno));
    }

    if (connect(forward_socket,
                (struct sockaddr *) &forward_address,
                sizeof(forward_address)) == -1)
    {
        log_error("connect failed: %s\n", strerror(errno));
    }

    return forward_socket;
}


/**
 * Forwards all traffic from the client's socket to the destination
 * host/port.  This also initiates the connection to the destination.
 */
void forward_traffic(int client_socket,
                     char *forward_name,
                     int forward_port,
                     int server_port)
{
    int forward_socket;
    pid_t down_pid;

    forward_socket = open_forwarding_socket(forward_name, forward_port);

    // Fork - child forwards traffic back to client, parent sends from client
    // to forwarded port
    down_pid = fork();
    if (down_pid == -1)
    {
        log_error("fork failed: %s\n", strerror(errno));
    }

    if (down_pid == 0)
    {
        comms(forward_socket, client_socket, server_port);
    }
    else
    {
        comms(client_socket, forward_socket, server_port);
    }
    sleep(1);
}


/**
 * Opens the listening port or dies trying.
 */
int open_listening_port(int server_port)
{
    struct sockaddr_in server_address;
    int server_socket;

    log_info("Open port %d\n", server_port);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        log_error("socket failed: %s\n", strerror(errno));
    }

    bzero((char *) &server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(server_port);

    if (bind(server_socket,
             (struct sockaddr *) &server_address,
             sizeof(server_address)) == -1)
    {
        log_error("bind failed: %s\n", strerror(errno));
    }

    if (listen(server_socket, 40) == -1)
    {
        log_error("listen failed: %s\n", strerror(errno));
    }

    return server_socket;
}


/**
 * Handles a single connection.  Will fork in order to handle another
 * connection while the child starts to forward traffic.
 */
void accept_connection(int server_socket,
                       char *forward_name,
                       int forward_port,
                       int server_port)
{
    int client_socket;
    pid_t up_pid;

    client_socket = accept(server_socket, NULL, NULL);
    if (client_socket == -1)
    {
        log_error("accept failed: %s\n", strerror(errno));
    }

    // Fork - Child handles this connection, parent listens for another
    up_pid = fork();
    if (up_pid == -1)
    {
        log_error("fork failed: %s\n", strerror(errno));
    }

    if (up_pid == 0)
    {
        forward_traffic(client_socket, forward_name, forward_port, server_port);
        exit(EXIT_FAILURE);
    }

    close(client_socket);
}

/**
 * Coordinate everything
 */
void run_forward_server(int server_port,
                        int forward_port,
                        char *forward_name)
{
    int server_socket;
    signal(SIGCHLD, SIG_IGN);
    server_socket = open_listening_port(server_port);

    while (1)
    {
        accept_connection(server_socket, forward_name, forward_port, server_port);
        // sleep(1);
    }

    return;
}
