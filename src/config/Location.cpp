#include "../../include/webserv.hpp"

Location::Location(){
	name = "";
	root = "";
	autoindex = true;
	autoindexSet = false;
}

Location::Location(const Location &location) : name(location.name),
											   root(location.root), allowedMethods(location.allowedMethods),
											   index(location.index), autoindex(location.autoindex), cgi_info(location.cgi_info),
											   autoindexSet(location.autoindexSet){}

Location &Location::operator=(const Location &location){
	if (&location != this)
	{
		name = location.name;
		root = location.root;
		allowedMethods = location.allowedMethods;
		index = location.index;
		autoindex = location.autoindex;
		cgi_info = location.cgi_info;
		autoindexSet = location.autoindexSet;
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

	if (stat(root.c_str(), &sb) != 0)
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
	this->autoindexSet = true;
}

void Location::setCgiInfo(std::string cgiInfo){
	this->cgi_info.push_back(cgiInfo);
}

void Location::changeAutoIndex(bool ai) {
	this->autoindex = ai;
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
	std::cout << "\tCGI Info:\n";
	std::for_each(location.cgi_info.begin(), location.cgi_info.end(), printListTab);
	std::cout << "\tAllowed:\n";
	std::for_each(location.allowedMethods.begin(), location.allowedMethods.end(), printListMethods);
	std::cout << "\tAutoindex: " << location.autoindex << std::endl;
}

const std::string &Location::getName() const{
	return this->name;
}

const std::vector<RequestMethod> &Location::getAllowedMethods() const{
	return this->allowedMethods;
}

bool Location::isAutoIndex() const{
	return this->autoindex;
}

std::vector <std::string> Location::getCgiInfo() const{
	return this->cgi_info;
}

const std::vector <std::string> &Location::getIndex() const{
	return this->index;
}

const std::string &Location::getRoot() const{
	return this->root;
}

void Location::autoCompleteFromServer(const Server &server) {
	if (root.empty())
		this->root = server.getRoot();
	if (index.empty())
		this->index = server.getIndex();
	if (!autoindexSet)
		this->autoindex = server.isAutoIndex();
	if (this->cgi_info.empty())
		this->cgi_info = server.getCgiInfo();
    if (this->allowedMethods.empty()) {
        this->allowedMethods.push_back(GET);
        this->allowedMethods.push_back(POST);
        this->allowedMethods.push_back(DELETE);
    }
}
