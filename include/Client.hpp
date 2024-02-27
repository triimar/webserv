#pragma once

#include "utils.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "Response.hpp"

class Server;
class Request;

class Client {
private:
	int					clientfd;
	Server				*server;
	Request				request;
	std::vector<char>	responseMsg;

	time_t				connectionStart;
	bool				keepAlive;
	bool				isActive;

public:
	Client();
	Client(Server *server);
	Client(const Client &client);
	Client &operator=(Client &client);
	~Client();

	void	confirmKeepAlive();
	void	updateTime();
	bool	isTimeout();
	void	setResponse(std::vector<char> response);

	int		&getClientFd();
	Server	*getServer();
	Request	&getRequest();
	time_t	getConnectionStart();
	bool	getKeepAlive();
	bool	getIsActive();

};