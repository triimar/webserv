#pragma once


#include "Location.hpp"

#define BUFFER_SIZE 30720
#include "Request.hpp"
#include "Status.hpp"
#include "utils.hpp"


class Server
{
private:

    std::string                 configPath;
	unsigned short				port;
	in_addr						host;
	std::vector<std::string>	serverName;
	std::string					root;
	std::vector<std::string>	index;
	std::string					ipAddress;
	unsigned long				clientSize;

	//Change to map for error code as key
	std::vector<std::string>	errorPages;
	//Map with name
	std::vector<Location>		locations;
	bool						autoindex;
	std::vector<std::string>	cgi_info;

	int							socketFd;
	sockaddr_in					socketAddress;
	int 						newSocket;
//	long 						incomingMsg;
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
	void setHost(std::string host);
	void setName(std::string name);
	void setRoot(std::string root);
	void setIndex(std::string index);
	void setIP();
	void setClientSize(unsigned long clientSize);
	void setErrorPage(std::string errorPage);
	void setLocation(std::string line, std::ifstream &stream);
//	void pushLocation();

	static void printServer(Server &server);
    static const char *getStatusMessage(uint16_t status);
    std::string getRoot() const;
    std::string getServerName() const;
    unsigned short getPort() const;

};