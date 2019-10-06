//============================================================================
// Name        : TCP_server.cpp
// Author      : romedal
// Version     :
// Copyright   : Your copyright notice
// Description : TCP server C++, Ansi-style
//============================================================================

#include "../inc/Server.hpp"

using namespace std;

int main() {
	Server* srv = new Server();
	srv->create_server();
	srv->make_socket_non_blocking(srv->socket_fd);
	srv->start_listen();
	srv->create_multiplex();
	srv->set_multiplex();
	srv->start_multiplex();

	delete srv;
	return 0;
}
