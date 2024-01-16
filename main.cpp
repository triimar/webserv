#include <iostream>
#include "Server.hpp"
#include "Request.hpp"

// int main(int argc, char **argv)
// {
// 	if (argc > 2)
// 	{
// 		std::cerr << "Wrong number of arguments\n";
// 		return 1;
// 	}
// 	(void) argv;
// 	Server server;
// 	std::string parameter = "0.0.0.0";
// 	server.setPort(4242);
// 	server.setHost(inet_addr(parameter.data()));
// 	server.setIP();
// 	server.startServer();
// 	server.startListen();
// }

//testing request parsing
int main() 
{
	const char* buffer = "\r\n\r\n\r\n\r\nGET /path/to/resource HTTP/1.1\r\nhost:localhost\r\nwhat : that and that\r\n\r\n";
	Request line;
	line.processRequest(buffer, 63);
}