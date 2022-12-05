#ifndef TCP_FORWARD_H
#define TCP_FORWARD_H

// #define on_error(...) { fprintf(stderr, __VA_ARGS__); fflush(stderr); exit(EXIT_FAILURE); }
//
// void log_data(char * buf, int len);

/**
 * Send the traffic from src socket to dst socket
 *
 * When done or on any error, this dies and will kill the forked process.
 */
void comms(int src, int dst);

/**
 * Opens a connection to the destination.
 *
 * On any error, this dies and will kill the forked process.
 */
int open_forwarding_socket(char *forward_name, int forward_port);


/**
 * Forwards all traffic from the client's socket to the destination
 * host/port.  This also initiates the connection to the destination.
 */
void forward_traffic(int client_socket, char *forward_name, int forward_port,
                     int server_port);


/**
 * Opens the listening port or dies trying.
 */
int open_listening_port(int server_port);


/**
 * Handles a single connection.  Will fork in order to handle another
 * connection while the child starts to forward traffic.
 */
void accept_connection(int server_socket, char *forward_name, int forward_port);

/**
 * Coordinates the effort
 */
int run_forward_server(int server_port, int forward_port, char *forward_name);

#endif
