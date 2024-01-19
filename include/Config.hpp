#pragma once

#include <iostream>
#include <string>
#include <vector>

class Config{
private:
	int port;
	std::string host;
	std::vector<std::string> serverNames;
	int errorLogFd;

};