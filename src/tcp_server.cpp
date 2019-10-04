//============================================================================
// Name        : TCP_server.cpp
// Author      : romedal
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "../inc/Server.hpp"

using namespace std;

int main() {
	Server* srv = new Server();
	srv->create_server();
	srv->make_socket_non_blocking();
	srv->start_listen();
	for(;;){

	}

	delete srv;
	return 0;
}
