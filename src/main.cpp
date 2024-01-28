#include "../include/Server.hpp"
#include "../include/utils.hpp"
#include "../include/Config.hpp"

int main(int argc, char **argv)
{
	Server server;

    if (argc > 2 || !argv) {
        std::exit(ft_perror(INVARGS, NULL));
    }

	std::string parameter = "0.0.0.0";
	server.setPort(4242);
	server.setHost(parameter);
	server.setIP();
    // above is config template

	server.startServer();
	server.startListen();
}