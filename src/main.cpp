#include "../include/Server.hpp"
#include "../include/utils.hpp"
#include "../include/Config.hpp"

int main(int argc, char **argv)
{
	if (argc > 2)
	{
		std::cerr << "Wrong number of arguments\n";
		return 1;
	}
	(void) argv;
	// Server server;
	// std::string parameter = "0.0.0.0";
	// server.setPort(4242);
	// server.setHost(parameter.data());
	// server.setIP();
	// server.startServer();
	// server.startListen();

	try{
		std::vector<Server> list;
		std::string file = DEFAULT_CONFIG;
		if (argc > 1)
			file = argv[1];
		Config config(file.c_str());
		list = config.createServers();
		std::for_each(list.begin(), list.end(), Server::printServer);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what();
	}
}