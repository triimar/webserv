#include "../../include/Location.hpp"

Location::Location(){
	name = "";
	root = "";
	autoindex = true;
}

Location::Location(const Location &location) : name(location.name),
											   root(location.root), allowedMethods(location.allowedMethods),
											   index(location.index), autoindex(location.autoindex){}

Location &Location::operator=(const Location &location) {
	if (&location != this)
	{
		name = location.name;
		root = location.root;
		allowedMethods = location.allowedMethods;
		index = location.index;
		autoindex = location.autoindex;
	}
	return *this;
}

Location::~Location() {}

void Location::setName(std::string name) {
	if (!this->name.empty())
		throw std::runtime_error("Configuration file error: declared name twice.\n");
//	struct stat sb;

//	if (stat(root.c_str(), &sb) == 0)
		this->name = name;
//	else
//		throw std::runtime_error("Config file location error: location directory does not exist.\n");
}

void Location::setRoot(std::string root) {
	if (!this->root.empty())
		throw std::runtime_error("Configuration file error: declared root twice.\n");
	struct stat sb;

	if (stat(root.c_str(), &sb) == 0)
		this->root = root;
	else
		throw std::runtime_error("Config file error: root directory does not exist.\n");
	this->root = root;
}

void Location::setIndex(std::string index) {
//	struct stat sb;
//
//	if (stat(index.c_str(), &sb) == 0)
		this->index.push_back(index);
//	else
//		throw std::runtime_error("Config file error: index directory does not exist.\n");
}

void Location::setMethod(std::string method) {
	int methodNr = 3;
	std::string methods[] = {"GET", "POST", "DELETE"};
	int i;
	for (i = 0; i < methodNr; i++)
		if (method == methods[i])
			break;
	if (i == methodNr)
		throw std::runtime_error("Configuration file error: method does not exist.\n");
	this->allowedMethods.push_back(static_cast<RequestMethod>(i));
}

void Location::setAutoIndex(std::string autoindex) {
	if (autoindex == "true"){
		this->autoindex = true;
	}
	else if (autoindex == "false"){
		this->autoindex = false;
	}
	else
		throw std::runtime_error("Config file error: autoindex can only be set to true or false.\n");
}

void printListTab(std::string index)
{
	std::cout << "\t\t" << index << std::endl;
}
void printListMethods(RequestMethod meth)
{
	std::string method;
	if (meth == GET)
		method = "GET";
	else if (meth == POST)
		method = "POST";
	else
		method = "DELETE";
	std::cout << "\t\t" << method << std::endl;
}

void Location::printLocation(Location &location) {
	std::cout << "Location: " << location.name <<
			  "\n\tRoot: " << location.root << "\n\tIndex:\n";
	std::for_each(location.index.begin(), location.index.end(), printListTab);
	std::cout << "\tAllowed:\n";
	std::for_each(location.allowedMethods.begin(), location.allowedMethods.end(), printListMethods);
	std::cout << "\tAutoindex: " << location.autoindex << std::endl;
}

std::string Location::getName() {
	return this->name;
}