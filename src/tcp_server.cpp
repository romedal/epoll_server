//============================================================================
// Name        : TCP_server.cpp
// Author      : romedal
// Version     :
// Copyright   : Your copyright notice
// Description : TCP server C++, Ansi-style
//============================================================================

#include "../inc/Server.hpp"
#include <memory>

using namespace std;


int main() {
	std::unique_ptr<Server> p2(new Server());
	p2->make_foo_func_threads();
	return 0;
}
