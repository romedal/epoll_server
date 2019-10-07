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
	srv->make_foo_func_threads();
	delete srv;
	return 0;
}
