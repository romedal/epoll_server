/*
 * Server.cpp
 *
 *  Created on: Oct 4, 2019
 *      Author: romedal
 */

#include "../inc/Server.hpp"
#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>
#include <ctype.h>
#include <map>
#include <cstdlib>
#include <thread>


Server::Server(): socket_fd{0}, epoll_fd{0}, event_count{0}, running{1}, i{0}, bytes_read{0}, numPeers{0}, numPacks{0}
{
	csvTool = new Csv();
	memset(&event, 0x00, sizeof(struct epoll_event));
	memset(events, 0x00, sizeof(events));
	memset(read_buffer, 0x00, sizeof(read_buffer));
	event.events = EPOLLIN;
	event.data.fd = 0;
//	make_foo_func_threads();
}

Server::~Server()
{

#ifdef DEBUG
	cout<<"server destroying ..."<<endl;
#endif

	if(close(epoll_fd))
	{
		cerr<<"Failed to close epoll file descriptor"<<endl;
	}

	delete this->csvTool;

#ifdef DEBUG
	cout<<"server was destroyed"<<endl;
#endif

}

bool Server::create_multiplex()
{
	bool ret = true;

	do{
		epoll_fd = epoll_create1(0);

		if(epoll_fd == -1)
		{
			cerr<<"Failed to create epoll file descriptor"<<endl;
			ret = false;
			break;
		}

	}while(false);

	return ret;
}

bool Server::set_multiplex()
{
	bool ret = true;

	do{
		if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, 0, &event))
		{
			cerr<<"Failed to add file descriptor to epoll"<<endl;
			close(epoll_fd);
			ret = false;
			break;
		}

		event.data.fd = socket_fd;
		event.events = EPOLLIN | EPOLLET;
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &event) == -1) {
			cerr<<"epoll_ctl error"<<endl;
			ret = false;
			break;
		}
	}while(false);

	return ret;
}

void Server::start_multiplex()
{
	while(running) {
		int n, i;
		n = epoll_wait(epoll_fd, events, MAXEVENTS, -1);
		for (i = 0; i < n; i++) {
			if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) ||
			    !(events[i].events & EPOLLIN)) {
				cerr<<"epoll error"<<endl;
				close(events[i].data.fd);
			} else if (events[i].data.fd == socket_fd) {
				accept_and_add_new();
			} else {
				csvTool->create_headers_csv(events[i].data.fd, 0);
				process_new_data(events[i].data.fd);
			}
		}
	}
}

bool Server::create_server()
{
	struct sockaddr_in server_addr;
	int opt = 1, retBind = 0;
	bool ret = true;

	do{

		if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
			cerr<<"Socket errro"<<endl;
			ret = false;
			break;
		}

		if (setsockopt(socket_fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(int)) == -1) {
			cerr<<"Setsockopt errro"<<endl;
			ret = false;
			break;
		}

		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(5000);
		server_addr.sin_addr.s_addr = INADDR_ANY;
		bzero(&(server_addr.sin_zero),8);
		retBind = bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));

		if (retBind == -1) {
			cerr<<"Unable to bind"<<endl;
			ret = false;
			break;
		}

	}while(false);

	return ret;
}

bool Server::make_socket_non_blocking(int sfd)
{
	int flags;
	bool ret = true;

	do{

		flags = fcntl(sfd, F_GETFL, 0);
		if (flags == -1) {
			cerr<<"fcntl get errro"<<endl;
			ret = false;
			break;
		}

		flags |= O_NONBLOCK;
		if (fcntl(sfd, F_SETFL, flags) == -1) {
			cerr<<"fcntl set error"<<endl;
			ret = false;
			break;
		}

	}while(false);

	return ret;
}

int Server::start_listen()
{
	if (listen(socket_fd, 5) == -1) {
		cerr<<"Listen error"<<endl;
		exit(1);
	}
	else {
		cout<<"TCPServer Waiting for client on port 5000\n"<<std::flush;
//		make_foo_func_threads();
	}

	return 0;
}

void Server::process_new_data(int fd)
{
	ssize_t count;
	char buf[512]={0};
	string str={0};

	while ((count = read(fd, buf, sizeof(buf) - 1))) {
		if (count == -1) {
			if (errno == EAGAIN)
				return;

			cerr<<"read error"<<endl;
			break;
		}

		buf[count] = '\0';
		str.assign(buf);
		this->setPackNum(INC);
		split1(buf, fd);
	}
//	cout<<"Close connection on descriptor: "<<endl;
	this->setPeerNum(DEC);
	csvTool->sort_csv(fd);
	close(fd);
}

void Server::accept_and_add_new()
{
	struct epoll_event event;
	struct sockaddr in_addr;
	socklen_t in_len = sizeof(in_addr);
	int infd;
	char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

	while ((infd = accept(socket_fd, &in_addr, &in_len)) != -1) {

		if (getnameinfo(&in_addr, in_len, hbuf, sizeof(hbuf), sbuf, sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICHOST) == 0) {
//			cout<<"Accepted connection on descriptor "<< infd<< "(host = "<< hbuf<< "port = " << sbuf <<endl;
			this->setPeerNum(INC);
		}
		if (!make_socket_non_blocking(infd)) {
			abort();
		}

		event.data.fd = infd;
		event.events = EPOLLIN | EPOLLET;
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, infd, &event) == -1) {
			cerr<<"epoll_ctl"<<endl;
			abort();
		}
		in_len = sizeof(in_addr);
	}

	if (errno != EAGAIN && errno != EWOULDBLOCK)
		cerr<<"accept error"<<endl;
}

void Server::split1(char* input, int fd)
{
    char* arr[3]={0};
    int i = 0;
	char *token = std::strtok(input, " ");

    while (token != NULL) {
        arr[i]=token;
        token = std::strtok(NULL, " ");
        ++i;
    }
    csvTool->csv_create(arr, fd);
}

int Server::getPeerNum()
{
	int ret = 0;
	mtx.lock();
	ret = this->numPeers;
	mtx.unlock();
	return ret;
}

int Server::getPackNum()
{
	int ret = 0;
	mtx.lock();
	ret = this->numPacks;
	mtx.unlock();
	return ret;
}

void Server::setPeerNum(op operation)
{
	if (operation == INC){
		 mtx.lock();
		++(this->numPeers);
		 mtx.unlock();
	}else if(operation == DEC){
		 mtx.lock();
		--(this->numPeers);
		 mtx.unlock();
	}else{
		cerr<<"undefined operation"<<endl;
	}
}

void Server::setPackNum(op operation)
{
	if (operation == INC){
		 mtx.lock();
		++(this->numPacks);
		 mtx.unlock();
	}else if(operation == DEC){
		 mtx.lock();
		--(this->numPacks);
		 mtx.unlock();
	}else{
		cerr<<"undefined operation"<<endl;
	}
}

void Server::make_foo_func_threads()
{
	some_threads.push_back(std::thread(&Server::updateInfo, this));
	some_threads.push_back(std::thread(&Server::prepareServer, this));
	for (auto& t: some_threads) t.join();
}

void Server::prepareServer()
{
	this->create_server();
	this->make_socket_non_blocking(this->socket_fd);
	this->start_listen();
	this->create_multiplex();
	this->set_multiplex();
	this->start_multiplex();
}

void Server::updateInfo(){
	while(running){
		sleep(1);
		cout<<"Peers number: "<<getPeerNum()<<" Packet number: "<<getPackNum()<<"\r"<<std::flush;
	}
}

