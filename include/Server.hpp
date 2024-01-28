#pragma once

#include "Request.hpp"
#include "Status.hpp"
#include "utils.hpp"

class Server
{
private:

    static CGIList              supportedCGI;
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

	void setPort(unsigned short port);
	void setHost(in_addr_t host);
	void setName(std::string name);
	void setRoot(std::string root);
	void setIndex(std::string index);
	void setIP();
	void setClientSize(unsigned long clientSize);
	void setErrorPage(std::string errorPage);

    static std::string getCGIInterpreter(std::string &extension);
    static const char *getStatusMessage(uint16_t status) const;
    std::string getRoot() const;
    std::vector<std::string> getIndex() const;
    std::string getServerName() const;
};