#pragma once

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <exception>

class Server
{
private:
	int socketFd;
	std::string ipAddress;
	int port;
	char *configFile;

public:
	Server();
	Server(char *configFile);
	Server(const Server& server);
	Server &operator=(const Server& server);
	~Server();

	void startServer();
	void closeServer();

};