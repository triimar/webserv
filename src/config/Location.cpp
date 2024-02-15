#include "../../include/Location.hpp"

Location::Location(){
	name = "";
	root = "";
	autoindex = true;
}

Location::Location(const Location &location) : name(location.name),
											   root(location.root), allowedMethods(location.allowedMethods),
											   index(location.index){}

Location &Location::operator=(const Location &location) {
	if (&location != this)
	{
		name = location.name;
		root = location.root;
		allowedMethods = location.allowedMethods;
		index = location.index;
	}
	return *this;
}

Location::~Location() {}

void Location::setName(std::string name) {
	if (!this->name.empty())
		throw std::runtime_error("Configuration file error: declared name twice.\n");
	this->name = name;
}

void Location::setRoot(std::string root) {
	if (!this->root.empty())
		throw std::runtime_error("Configuration file error: declared root twice.\n");
	this->root = root;
}

void Location::setIndex(std::string index) {
	this->index.push_back(index);
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
}