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
	const char* buffer = "\r\n\r\n\r\n\r\nGET http://www.example.com/path/to/resource?query#fragment HTTP/1.1\r\nhost:localhost\r\nkey key: that and that\
 hallo ballo     \r\n falloyallo  	\r\nwhat: vvaaaalue\r\n\r\n";
	Request line;
	line.processRequest(buffer, std::strlen(buffer) + 1);
	std::cout << std::strlen(buffer);
}