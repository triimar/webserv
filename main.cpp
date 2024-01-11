#include <iostream>
#include "Server.hpp"

int main(int argc, char **argv)
{
	if (argc > 2)
	{
		std::cerr << "Wrong number of arguments\n";
		return 1;
	}
//	(void) argv;
//	Server server;
//	std::string parameter = "0.0.0.0";
//	server.setPort(4242);
//	server.setHost(inet_addr(parameter.data()));
//	server.setIP();
//	server.startServer();
//	server.startListen();

	try{
		Config config(argv[1]);
		config.createServers();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what();
	}
}