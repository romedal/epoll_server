/*
 * Server.h
 *
 *  Created on: Oct 4, 2019
 *      Author: romedal
 */

#ifndef SERVER_H_
#define SERVER_H_

#define MAX_EVENTS 10u
#define READ_SIZE 10u

using namespace std;

#include <iostream>
#include <unistd.h>    // for close()
#include <sys/epoll.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <errno.h>


class Server {
private:
	int epoll_fd, listenfd;
	int running , event_count, i;
	struct epoll_event event, events[MAX_EVENTS];
	size_t bytes_read;
	char read_buffer[READ_SIZE + 1];

public:
	Server();
	virtual ~Server();
	bool create_server();
	int make_socket_non_blocking();
	int start_listen();
private:
	bool create_multiplex();
	bool set_multiplex();
	void start_multiplex();
};

#endif /* SERVER_H_ */
