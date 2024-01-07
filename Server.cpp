#include "Server.hpp"

Server::Server() {

}

Server::Server(char *configFile) : configFile(configFile){

}

Server::Server(const Server &server) : configFile(server.configFile), socketFd(server.socketFd){

}

Server &Server::operator=(const Server &server) {
	if (server != this)
	{
		this->configFile = server.configFile;
		this->socketFd = server.socketFd;
	}
	return *this;
}

Server::~Server() {
	closeServer();
}

void Server::startServer() {
	this->socketFd = socket(AF_LOCAL, SOCK_STREAM, 0);
	if (!socketFd)
		throw std::runtime_error("Error: unable to start server.\n");
}


void Server::closeServer() {
	close(socketFd);
}