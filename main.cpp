#include <iostream>
#include "Server.hpp"
#include "Config.hpp"

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
		std::vector<Server> list;
		Config config(argv[1]);
		list = config.createServers();
		std::for_each(list.begin(), list.end(), Server::printServer);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what();
	}
}