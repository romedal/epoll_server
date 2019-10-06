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
				create_headers_csv(events[i].data.fd, 0);
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
		cout<<"this is "<<str<<endl;
		split1(buf, fd);
//		printf("%s \n", buf);
	}
	printf("Close connection on descriptor: %d\n", fd);
	this->sort_csv(fd);
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

bool Server::csv_create(char** arr, int fd)
{
	// file pointer
	fstream fout;
	char name[10];
	sprintf(name, "connection%d.csv", fd);
	// opens an existing csv file or creates a new file.
//	if(!fout.is_open())
//	{
//
//	}
	fout.open(name, ios::out | ios::app);

//	cout<<"Enter the details of 5 students:"<<" roll name maths phy chem bio"<< endl;

//	int i, roll, phy, chem, math, bio;
//	string name;

	fout <<arr[0] << ","
			<< arr[1] << ","
			<< arr[2] << ","
			<< "\r\n";

	fout.close();
	return 0;
}

bool Server::csv_create(int c1, float c2, int c3, int fd)
{
	fstream fout;
	char name[10];
	sprintf(name, "connection%d_sorted.csv", fd);
	fout.open(name, ios::out | ios::app);

	fout <<c1<< ","
			<<c2<< ","
			<<c3<< ","
			<< "\r\n";
	fout.close();
	return 0;
}


bool Server::sort_csv(int fd)
{
	char name[10];
	sprintf(name, "connection%d.csv", fd);
	char delim =',';
	std::string orbits="";
	std::string::size_type sz;

	cout<<"sorting "<<name<<endl;
	// File pointer
	fstream fin;

	// Open an existing file
	fin.open(name, ios::in);
	vector<string> row;
	string line, word, temp;
	vector<string> words;
	vector<tuple<int, float, int>> wideVector;
	int C1=0; float C2=0; int C3=0;
	//	map<float, vector<int, int>> bigMap;

	while (fin >> temp) {
		row.clear();
		// read an entire row and
		// store it in a string variable 'line'
		getline(fin, line);
		// used for breaking words
		//		cout << "line: "<<line<< "\n";
		stringstream s(line);


		// read every column data of a row and
		// store it in a string variable, 'word'
		char delim = ','; // Ddefine the delimiter to split by
		while (std::getline(fin, line, delim)) {
			// Provide proper checks here for tmp like if empty
			// Also strip down symbols like !, ., ?, etc.
			// Finally push it.
			if(line != "")
				words.push_back(line);
		}
	}
	int r = 1;
	std::cout<<"size -"<<words.size()<<std::endl;
	int size = words.size() - 1;
	int init_size = 0;
	//while(init_size < size){
	for(auto it = words.begin(); it != words.end(); it++) {
		//	cout<<"r "<<r<<endl;
		orbits.assign(*it);
		orbits.erase(std::remove(orbits.begin(), orbits.end(), ' '), orbits.end());
		orbits.erase(std::remove(orbits.begin(), orbits.end(), '\r'), orbits.end());
		orbits.erase(std::remove(orbits.begin(), orbits.end(), '\n'), orbits.end());
		if(orbits != "")
		{
			if (r == 3)
			{
				r=1;
				C3=stoi(orbits);
				wideVector.push_back(std::make_tuple(C1, C2, C3));
			}else
				if (r == 2)
				{
					C2=stof(orbits);
					++r;
				}else
					if (r == 1)
					{
						C1=stoi(orbits);
						++r;
					}

		}
	}
	//	init_size +=3;
	//}
	fin.close();

	for ( const auto& i : wideVector ) {
		cout <<"fucking tuple  "<< get<0>(i)<<"  "<< get<1>(i)<<"  "<< get<2>(i) << endl;
	}

	create_headers_csv(fd, 1);
	cout<<"after sorting"<<endl;
	sort(wideVector.begin(), wideVector.end(), sortbysec);
	for ( const auto& i : wideVector ) {
		this->csv_create(get<0>(i), get<1>(i), get<2>(i), fd);
		cout <<"fucking tuple  "<< get<0>(i)<<"  "<< get<1>(i)<<"  "<< get<2>(i) << endl;
	}

	wideVector.clear();
}


void Server::split1(char* input, int fd)
{
    char* arr[3]={0};
    int i = 0;
	char *token = std::strtok(input, " ");

    while (token != NULL) {
        std::cout<<"my token " << token << '\n';
        arr[i]=token;
        token = std::strtok(NULL, " ");
        ++i;
    }
    this->csv_create(arr, fd);
}
