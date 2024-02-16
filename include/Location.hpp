#pragma once

//#include <iostream>
//#include <string>
//#include <vector>
#include "utils.hpp"

//enum Methods{
//	GET,
//	POST,
//	DELETE,
//};

class Location
{
private:
	std::string					name;
	std::string					root;
	std::vector<RequestMethod>		allowedMethods;
	std::vector<std::string>	index;
	bool						autoindex;
	std::vector<std::string>	cgi_info;

public:
	Location();
	Location(const Location& location);
	Location &operator=(const Location& location);
	~Location();

	void setName(std::string name);
	void setRoot(std::string root);
	void setMethod(std::string method);
	void setIndex(std::string index);

    const std::vector<RequestMethod> &getAllowedMethods() const;

	static void printLocation(Location &location);
};