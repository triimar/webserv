#pragma once

#include "utils.hpp"
#include "Server.hpp"

class Server;

class Location
{
private:
	std::string					name;
	std::string					root;
	std::vector<RequestMethod>	allowedMethods;
	std::vector<std::string>	index;
	bool						autoindex;
	std::vector<std::string>	cgi_info;
	bool						autoindexSet;

public:
	Location();
	Location(const Location& location);
	Location &operator=(const Location& location);
	~Location();

	void setName(std::string name);
	void setRoot(std::string root);
	void setMethod(std::string method);
	void setIndex(std::string index);
	void setAutoIndex(std::string autoindex);
	void setCgiInfo(std::string cgiInfo);
	void changeAutoIndex(bool ai);

	void autoCompleteFromServer(const Server &server);

	const std::string 	&getName();

	static void printLocation(Location &location);
};