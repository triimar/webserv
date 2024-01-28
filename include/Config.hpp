#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "Server.hpp"

class Config{
private:
	int port;
	std::string host;
	std::vector<std::string> serverNames;
	int errorLogFd;

public:
	Config(char *file);
	Config(const Config& file);
	~Config();
	Config &operator=(const Config& config);

	std::vector<Server> createServers();
};