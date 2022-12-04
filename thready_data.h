#ifndef THREADY_DATA_H
#define THREADY_DATA_H

#define MESSAGE_SIZE 1024
#define SLEEP_TIME 4

struct thread_msg
{
	int thread_num;
	int sleep_time;
	int port_num;
	char message[MESSAGE_SIZE+1];
};

struct thread_ports
{
	int rx_port;
	int tx_port;
};

#endif
