#pragma once

#include "Server.hpp"
#include "Client.hpp"
#include "utils.hpp"

class Config {

private:
//	int port;
//	std::string host;
//	std::vector<std::string> serverNames;
//	int errorLogFd;
	std::ifstream			str;
	std::vector<Server>		serverList;
	std::map<int, Client>	clientList;
	std::vector<pollfd>		fds;

	Config();

	Server getServer();
	bool isEmptyLine(std::string line);
	void parseServerLine(Server &server, std::string line);
	void createServers();
	void addFdToPoll(int fd);
	void closeTimeoutClients();
	void closeClient(int fd, size_t &index);
	static void sigintHandler(int signum);

public:
	Config(const char *file);
	Config(const Config& file);
	~Config();
	Config &operator=(const Config& config);
	void printServers();

	std::vector<Server> getServerList();
	std::map<int, Client> getClientMap();

	void startServers();
	void runServers();
};