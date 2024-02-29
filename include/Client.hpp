#pragma once

#include "Request.hpp"
#include "utils.hpp"

class Server;

class Client {
private:
	int					_clientfd;
	Server				&_server;
	Request				_request;
	std::vector<char>	_responseMsg;

	time_t				_connectionStart;
	bool				_keepAlive;
	bool				_finishedChunked;

public:
	// Client();
	Client(Server &server);
	// Client(const Client &client);
	// Client &operator=(Client &client);
	~Client();

	void	confirmKeepAlive();
	void	updateTime();
	bool	isTimeout();
	void	setResponse(std::vector<char> response);
	void	setChunkedUnfinished();
	void	setChunkedFinished();

	int					&getClientFd();
	Server				&getServer();
	Request				&getRequest();
	std::vector<char>	&getResponse();
	time_t				&getConnectionStart();
	bool				&getKeepAlive();
	bool				&getFinishedChunked();

};
