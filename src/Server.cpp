/*
 * Server.cpp
 *
 *  Created on: Oct 4, 2019
 *      Author: romedal
 */

#include "../inc/Server.hpp"

Server::Server(): i{0}, event_count{0}, epoll_fd{0}, running{1}, bytes_read{0}, listenfd{0}
{
	cout<<"server creating ..."<<endl;
	memset(&event, 0x00, sizeof(struct epoll_event));
	memset(events, 0x00, sizeof(events));
	memset(read_buffer, 0x00, sizeof(read_buffer));
	event.events = EPOLLIN;
	event.data.fd = 0;
	cout<<"server was created"<<endl;
}

Server::~Server()
{
	cout<<"server destroying ..."<<endl;
	if(close(epoll_fd))
	{
		fprintf(stderr, "Failed to close epoll file descriptor\n");
	}
	cout<<"server was destroyed"<<endl;
}

bool Server::create_multiplex()
{
	  epoll_fd = epoll_create1(0);

	  if(epoll_fd == -1)
	  {
	    fprintf(stderr, "Failed to create epoll file descriptor\n");
	    return EXIT_FAILURE;
	  }

	  if(close(epoll_fd))
	  {
	    fprintf(stderr, "Failed to close epoll file descriptor\n");
	    return EXIT_FAILURE;
	  }

	  return EXIT_SUCCESS;
}

bool Server::set_multiplex()
{
	  if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, 0, &event))
	  {
	    fprintf(stderr, "Failed to add file descriptor to epoll\n");
	    close(epoll_fd);
	    return EXIT_FAILURE;
	  }

	  return EXIT_SUCCESS;
}

void Server::start_multiplex()
{
	while(running)
	{
		printf("\nPolling for input...\n");
		event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, 30000);
		printf("%d ready events\n", event_count);
		for(i = 0; i < event_count; i++)
		{
			printf("Reading file descriptor '%d' -- ", events[i].data.fd);
			bytes_read = read(events[i].data.fd, read_buffer, READ_SIZE);
			printf("%zd bytes read.\n", bytes_read);
			read_buffer[bytes_read] = '\0';
			printf("Read '%s'\n", read_buffer);

			if(!strncmp(read_buffer, "stop\n", 5))
				running = 0;
		}
	}
}

bool Server::create_server()
{
	struct sockaddr_in server_addr;
	int opt = 1;
	int ret = 0;

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket errro");
		exit(1);
	}

	if (setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(int)) == -1) {
		perror("Setsockopt errro");
		exit(1);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(5000);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(server_addr.sin_zero),8);
	ret = bind(listenfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
	if (ret	== -1) {
		perror("Unable to bind");
		exit(1);
	}
	cout<<"ret - "<<ret<<endl;

	return EXIT_SUCCESS;
}

int Server::make_socket_non_blocking()
{
	int flags;

	flags = fcntl(listenfd, F_GETFL, 0);
	if (flags == -1) {
		perror("fcntl");
		return -1;
	}

	flags |= O_NONBLOCK;
	if (fcntl(listenfd, F_SETFL, flags) == -1) {
		perror("fcntl");
		return -1;
	}

	return 0;
}

int Server::start_listen()
{
	if (listen(listenfd, 5) == -1) {
		perror("Listen error");
		exit(1);
	}else
	{


		printf("\nTCPServer Waiting for client on port 5000\n");
		fflush(stdout);
	}

	return 0;
}
