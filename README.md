# epoll_server
epoll server with csv parser
using cmake generator "Eclipse CDT4 - Unix Makefiles"


epoll_server

epoll server with csv parser

Client in C and Server in C++ This programming assignment consists of two simple programs: a client and a server and one script in any scripting language .

    A TCP based server console program in C++ that accepts many client connections. It should be based on epoll ( it is a mandatory condition ). Server gets data from clients and saves this data to the CSV file. For each connection it should be a separate file. CSV file has 3 columns corresponding to received data. After a client is disconnected the server has to sort data in ascending order by second column.

The server must output every second statistic information in the same line. Information should contain amount of connected clients and total number of received packets.

    A TCP based client console program in C that connects to the server and sends every 500 ms 3 randomly generated numbers. The first and the third numbers are integers, the second one is a float. The user must be able to specify the remote host’s address and port, the maximum value for generated numbers, the maximum number of packets to send, after which the client disconnects (optional parameter ).

    A python script for running N clients in parallel. The script’s input parameters are following:

    The remote host’s address and port
    number of clients
    the maximum value for generated numbers
    the maximum number of packets to send, after which the client disconnects ( optional ).


