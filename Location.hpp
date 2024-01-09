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
	std::string	name;
	std::string	root;
	std::vector<Methods> allowedMethods;

public:
	Location();
	Location(const Location& location);
	Location &operator=(const Location& location);
	~Location();

};