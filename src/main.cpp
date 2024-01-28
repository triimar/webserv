#include "../include/Server.hpp"

int main(int argc, char **argv)
{
	Server server;

    if (argc > 2) {
        std::exit(ft_perror(INVARGS, NULL));
    } else if (argc == 2) {
        server.setConfigPath(argv[1]);
    }

	std::string parameter = "0.0.0.0";
	server.setPort(4242);
	server.setHost(inet_addr(parameter.data()));
	server.setIP();
    // above is config template

	server.startServer();
	server.startListen();
}