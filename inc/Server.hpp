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
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include "Csv.hpp"

class Server {

public:
	int socket_fd;
	Csv* csvTool;
private:
	int epoll_fd;
	int event_count;
	int running ,i;
	struct epoll_event event, events[MAXEVENTS];
	size_t bytes_read;
	char read_buffer[READ_SIZE + 1];

public:
	Server();
	virtual ~Server();
	bool create_server();
	int make_socket_non_blocking(int sfd);
	int start_listen();

	bool create_multiplex();
	bool set_multiplex();
	void start_multiplex();


	void sort_csv(int fd);

private:
	void process_new_data(int fd);
	void accept_and_add_new();
	void split1(char* input, int fd);

};

#endif /* SERVER_H_ */
