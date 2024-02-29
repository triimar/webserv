#pragma once

#include "Location.hpp"
#include "Status.hpp"
#include "utils.hpp"

class Server
{
private:

    std::string                 		configPath;
	unsigned short						port;
	in_addr								host;
	std::vector<std::string>			serverName;
	std::string							root;
	std::vector<std::string>			index;
	std::string							ipAddress;
	unsigned long						clientSize;

	std::map<int, std::string>          errorPages;
	std::map<std::string, Location> 	locations;
	bool								autoindex;
	std::vector<std::string>			cgi_info;

	int									socketFd;
	sockaddr_in							socketAddress;
	int 								newSocket;
	unsigned int						socketLen;

//	std::map<int, Client>				clients;
//	long 								incomingMsg;
//	uint32_t							ipAddress;

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
	void setErrorPage(int key, std::string errorPage);
	void setLocation(std::string line, std::ifstream &stream);
	void setAutoIndex(std::string autoindex);
	void setCgiInfo(std::string info);
//	void pushLocation();

	static void printServer(Server &server);

    static const char			*getStatusMessage(int status);
    std::string					getRoot() const;
    std::string					getServerName() const;
	std::vector<std::string>	getIndex() const;
	bool						isAutoIndex() const;
	Location					getLocation(const std::string &path) const;
	std::vector<std::string>	getCgiInfo() const;
    unsigned short				getPort() const;
    const std::string			&getIpAddr() const;
    std::string                 getErrorPage(int status) const;
	int							getSocketFd() const;
	void				        autoCompleteLocations();

};