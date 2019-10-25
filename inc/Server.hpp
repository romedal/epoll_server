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
#include <thread>
#include "Csv.hpp"
#include <mutex>



typedef enum operation{
	UND = 0,  //undef
	INC = 1,  //increase value by 1
	DEC = 2,  //decrease value by 1
	LST = 3   //last
} op;



class Server {

public:
	int socket_fd;
	Csv* csvTool;
private:
	int epoll_fd, event_count, running, i;
	char read_buffer[READ_SIZE + 1];
	size_t bytes_read;
	struct epoll_event event, events[MAXEVENTS];
	int numPeers, numPacks;
	std::vector<std::thread> some_threads;

public:
	Server();
	virtual ~Server();

	bool create_server();
	bool make_socket_non_blocking(int sfd);
	bool create_multiplex();
	bool set_multiplex();

	int start_listen();
	int getPeerNum();
	int getPackNum();

	void start_multiplex();
	void setPeerNum(op);
	void setPackNum(op);
	void updateInfo();
	void make_threads();
	void prepareServer();

private:
	void process_new_data(int fd);
	void accept_and_add_new();
	void split1(char* input, int fd);

};

#endif /* SERVER_H_ */
