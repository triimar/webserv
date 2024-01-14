#pragma once

#include <iostream>
#include <string>
#include <vector>

enum Methods{
	GET,
	POST,
	DELETE,
};

class Location
{
private:
	std::string					name;
	std::string					root;
	std::vector<Methods>		allowedMethods;
	std::vector<std::string>	index;

public:
	Location();
	Location(const Location& location);
	Location &operator=(const Location& location);
	~Location();

	void setName(std::string name);
	void setRoot(std::string root);
	void setMethod(std::string method);
	void setIndex(std::string index);

	static void printLocation(Location &location);
};