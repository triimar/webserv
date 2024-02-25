#include "../../include/Server.hpp"

Server::Server() {
	port = 0;
	host.s_addr = 0;
	root = "";
	socketFd = -1;
	ipAddress = "";
	clientSize = 0;
	autoindex = true;
    // translates SUPPORTED_CGI macro to usable map
	//TODO move to more global so not to have it every server
    std::vector<std::string> cgiPairs = splitString(SUPPORTED_CGI, '&');
    for (size_t i = 0; i < cgiPairs.size(); ++i) {
        std::vector<std::string> pair = splitString(cgiPairs[i], '=');
        supportedCGI[pair[0]] = pair[1];
    }
}

Server::Server(const Server &server) : port(server.port), host(server.host),
serverName(server.serverName), root(server.root), index(server.index),
ipAddress(server.ipAddress), clientSize(server.clientSize), errorPages(server.errorPages),
locations(server.locations), autoindex(server.autoindex), cgi_info(server.cgi_info), socketFd(server.socketFd){
	return;
}

Server &Server::operator=(const Server &server) {
	if (&server != this)
	{
		this->port = server.port;
		this->host = server.host;
		this->serverName = server.serverName;
		this->root = server.root;
		this->index = server.index;
		this->ipAddress = server.ipAddress;
		this->clientSize = server.clientSize;
		this->errorPages = server.errorPages;
		this->autoindex = server.autoindex;
		this->cgi_info = server.cgi_info;
		this->socketFd = server.socketFd;
	}
	return *this;
}

Server::~Server() {
	closeServer();
}

void Server::setPort(unsigned short port) {
	if (!port)
		throw std::runtime_error("Config file error: port cannot be initialized to 0.\n");
	if (this->port)
		throw std::runtime_error("Config file error: server's port was initialized twice.\n");
	this->port = port;
}

void Server::setHost(std::string host) {
	if (this->host.s_addr != 0)
		throw std::runtime_error("Config file error: server's host was initialized twice.\n");
	if (!inet_aton(host.c_str(), &this->host))
		throw std::runtime_error("Config file error: invalid host name.\n");
}

void Server::setName(std::string name) {
	serverName.push_back(name);
}

void Server::setRoot(std::string root) {
	struct stat sb;

	if (stat(root.c_str(), &sb) == 0)
		this->root = root;
	else
		throw std::runtime_error("Config file error: root directory does not exist.\n");
}

void Server::setIndex(std::string index) {
//	struct stat sb;
//
//	if (stat(index.c_str(), &sb) == 0)
	this->index.push_back(index);
//	else
//		throw std::runtime_error("Config file error: index directory does not exist.\n");
}

void Server::setIP() {
	if (!host.s_addr || !port)
		return;//Throw exception?
	this->ipAddress = inet_ntoa(this->host);
	this->ipAddress += ":";
	std::stringstream ss;
	ss << this->port;
	this->ipAddress += ss.str();
}

void Server::setClientSize(unsigned long clientSize) {
	if (!clientSize)
		throw std::runtime_error("Config file error: client size can't be 0.\n");
	if (this->clientSize)
		throw std::runtime_error("Config file error: client size was initialized twice.\n");
	this->clientSize = clientSize;
}

void Server::setErrorPage(unsigned int key, std::string errorPage) {
	if (key > 511 || key < 400)
		throw std::runtime_error("Config file error: error codes from 400 to 511.\n");
	this->errorPages.insert(std::make_pair(key, errorPage));
}

void Server::setAutoIndex(std::string autoindex) {
	if (autoindex == "true"){
		this->autoindex = true;
	}
	else if (autoindex == "false"){
		this->autoindex = false;
	}
	else
		throw std::runtime_error("Config file error: autoindex can only be set to true or false.\n");
}

void Server::setCgiInfo(std::string info) {
	this->cgi_info.push_back(info);
}

//void Server::pushLocation() {
//	Location location;
//	this->locations.push_back(location);
//}

// ???????
//	this->clientSize = clientSize;
//}

/* ************************************************************************** */
/*                                  GETTERS                                   */
/* ************************************************************************** */

std::string Server::getRoot() const {
    return (this->root);
}

std::vector<std::string> Server::getIndex() const{
    return (this->index);
}

bool Server::getAutoIndex() const {
	return this->autoindex;
}

std::vector <std::string> Server::getCgiInfo() const {
	return this->cgi_info;
}

std::string Server::getServerName() const {
    return (this->serverName.back());
}

std::string Server::getCGIInterpreter(const std::string &extension) {
    CGIList::iterator it = supportedCGI.find(extension);
    if (it != supportedCGI.end()) {
        return (it->second);
    } else {
        return ("");
    }
}

/**
 * This function returns the location that holds the directory given in the path.
 * It checks the location directory with the path up until the location name ends.
 * @param path the path from which to extract a location
 * @return the location, provided it exists. Otherwise, it will return a Location with
 * the same values as the server.
 */
Location Server::getLocation(std::string &path) {
//	std::vector<std::string> paths = splitString(path, '/');
	for (std::map<std::string, Location>::iterator it = this->locations.begin(); it != this->locations.end(); it++)
	{
		if (path.compare(0, it->first.size(), it->first) == 0)
			return it->second;
	}
	Location location;
	location.autoCompleteFromServer(*this);
	return location;
}

void Server::autoCompleteLocations() {
	for (std::map<std::string, Location>::iterator it = this->locations.begin(); it != this->locations.end(); it++) {
		it->second.autoCompleteFromServer(*this);
	}
}

int Server::getSocketFd() const {
	return this->socketFd;
}