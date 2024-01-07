#include "Server.hpp"

Server::Server() {

}

Server::Server(const Server &server) : port(server.port), host(server.host),
serverName(server.serverName), root(server.root), index(server.index),
socketFd(server.socketFd), ipAddress(server.ipAddress), clientSize(server.clientSize),
errorPages(server.errorPages) {
	return;
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
	if (socketFd < 0)
		throw std::runtime_error("Error: unable to start server.\n");
}


void Server::closeServer() {
	close(socketFd);
}