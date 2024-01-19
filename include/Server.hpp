#pragma once

#include "Request.hpp"
#include "utils.hpp"

/* ************************************************************************** */
/*                                  DEFINES                                   */
/* ************************************************************************** */

#define BUFFER_SIZE 30720
#define DEFAULT_CONFIG "webserv.conf"

// Define macro for CGI extensions and interpreters
// define a pair separated by a '='
// define as many pairs as you want separated by '&'
// make sure the interpreter exists at the given path
#define SUPPORTED_CGI ".sh=/bin/sh&.py=/usr/bin/python3&.perl=/usr/bin/perl"
typedef std::map<std::string, std::string> CGIList;

class Server
{
private:
    CGIList                     supportedCGI;
    std::string                 configPath;
	unsigned short				port;
	in_addr						host;
	std::string					serverName;
	std::string					root;
	std::vector<std::string>	index;
	std::string					ipAddress;
	unsigned long				clientSize;
	std::vector<std::string>	errorPages;
	int							socketFd;
	sockaddr_in					socketAddress;
	int 						newSocket;
	long 						incomingMsg;
	unsigned int				socketLen;
//	uint32_t ipAddress;

public:
	Server();
    Server(const Server& server);
	Server &operator=(const Server& server);
	~Server();

	void startServer();
	void closeServer();
	void startListen();
	void acceptConnection(int &newSocket);
    bool tryCGI(Request &req);

	void setPort(unsigned short port);
	void setHost(in_addr_t host);
	void setName(std::string name);
	void setRoot(std::string root);
	void setIndex(std::string index);
	void setIP();
	void setClientSize(unsigned long clientSize);
	void setErrorPage(std::string errorPage);
};