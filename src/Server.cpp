/*
 * Server.cpp
 *
 *  Created on: Oct 4, 2019
 *      Author: romedal
 */

#include "../inc/Server.hpp"

Server::Server(): epoll_fd{0}, event_count{0}, running{1}, i{0}, bytes_read{0}, socket_fd{0}
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

	  event.data.fd = socket_fd;
	  event.events = EPOLLIN | EPOLLET;
	  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &event) == -1) {
		  perror("epoll_ctl");
		  exit(1);
	  }

	  return EXIT_SUCCESS;
}

void Server::start_multiplex()
{
	while(running) {
		int n, i;
		n = epoll_wait(epoll_fd, events, MAXEVENTS, -1);
		for (i = 0; i < n; i++) {
			if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) ||
			    !(events[i].events & EPOLLIN)) {
				/* An error on this fd or socket not ready */
				perror("epoll error");
				close(events[i].data.fd);
			} else if (events[i].data.fd == socket_fd) {
				/* New incoming connection */
				accept_and_add_new();
			} else {
				/* Data incoming on fd */
				process_new_data(events[i].data.fd);
			}
		}
	}
}

bool Server::create_server()
{
	struct sockaddr_in server_addr;
	int opt = 1;
	int ret = 0;

	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket errro");
		exit(1);
	}

	if (setsockopt(socket_fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(int)) == -1) {
		perror("Setsockopt errro");
		exit(1);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(5000);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(server_addr.sin_zero),8);
	ret = bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
	if (ret	== -1) {
		perror("Unable to bind");
		exit(1);
	}
	cout<<"ret - "<<ret<<endl;

	return EXIT_SUCCESS;
}

int Server::make_socket_non_blocking(int sfd)
{
	int flags;

	flags = fcntl(sfd, F_GETFL, 0);
	if (flags == -1) {
		perror("fcntl");
		return -1;
	}

	flags |= O_NONBLOCK;
	if (fcntl(sfd, F_SETFL, flags) == -1) {
		perror("fcntl error");
		return -1;
	}

	return 0;
}

int Server::start_listen()
{
	if (listen(socket_fd, 5) == -1) {
		perror("Listen error");
		exit(1);
	}
	else {
		printf("\nTCPServer Waiting for client on port 5000\n");
		fflush(stdout);
	}

	return 0;
}

void Server::process_new_data(int fd)
{
	ssize_t count;
	char buf[512]={0};
	int c1=0, c3=0;
	float c2=0;
	string str={0};

	while ((count = read(fd, buf, sizeof(buf) - 1))) {
		if (count == -1) {
			if (errno == EAGAIN)
				return;

			perror("read error");
			break;
		}

		buf[count] = '\0';
		str.assign(buf);
		cout<<"this is"<<str<<endl;
//		printf("%s \n", buf);
	}

	printf("Close connection on descriptor: %d\n", fd);
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
			printf("Accepted connection on descriptor %d (host=%s, port=%s)\n", infd, hbuf, sbuf);
		}
		/* Make the incoming socket non-block
		 * and add it to list of fds to
		 * monitor*/
		if (make_socket_non_blocking(infd) == -1) {
			abort();
		}

		event.data.fd = infd;
		event.events = EPOLLIN | EPOLLET;
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, infd, &event) == -1) {
			perror("epoll_ctl");
			abort();
		}
		in_len = sizeof(in_addr);
	}

	if (errno != EAGAIN && errno != EWOULDBLOCK)
		perror("accept error");
}

bool Server::csv_create()
{
    // file pointer
    fstream fout;

    // opens an existing csv file or creates a new file.
    fout.open("reportcard.csv", ios::out | ios::app);

    cout<<"Enter the details of 5 students:"<<" roll name maths phy chem bio"<< endl;

    int i, roll, phy, chem, math, bio;
    string name;

    // Read the input
//    for (i = 0; i < 5; i++) {

        cin >> roll
            >> name
            >> math
            >> phy
            >> chem
            >> bio;

        // Insert the data to file
        fout << roll << ", "
             << name << ", "
             << math << ", "
             << phy << ", "
             << chem << ", "
             << bio
             << "\n";
//    }
    return 0;
}

bool Server::sort_csv()
{


}
