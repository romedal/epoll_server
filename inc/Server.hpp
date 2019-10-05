/*
 * Server.h
 *
 *  Created on: Oct 4, 2019
 *      Author: romedal
 */

#ifndef SERVER_H_
#define SERVER_H_

#define MAXEVENTS 10u
#define READ_SIZE 10u

using namespace std;

#include <iostream>
#include <unistd.h>    // for close()
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <errno.h>
#include <fstream>
#include <string>
#include <vector>

class Server {
private:
	int epoll_fd;
	int event_count;
	int running ,i;
	struct epoll_event event, events[MAXEVENTS];
	size_t bytes_read;
	char read_buffer[READ_SIZE + 1];

public:
	int socket_fd;
	Server();
	virtual ~Server();
	bool create_server();
	int make_socket_non_blocking(int sfd);
	int start_listen();

	bool create_multiplex();
	bool set_multiplex();
	void start_multiplex();

	bool csv_create();
	bool sort_csv();

private:
	void process_new_data(int fd);
	void accept_and_add_new();

};

#endif /* SERVER_H_ */
