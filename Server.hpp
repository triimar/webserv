#pragma once

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <exception>
#include <vector>

class Server
{
private:
	unsigned short				port;
	in_addr_t					host;
	std::string					serverName;
	std::string					root;
	std::string					index;
	int							socketFd;
	std::string					ipAddress;
	unsigned long				clientSize;
	std::vector<std::string>	errorPages;
//	uint32_t ipAddress;

public:
	Server();
	Server(const Server& server);
	Server &operator=(const Server& server);
	~Server();

	void startServer();
	void closeServer();

};