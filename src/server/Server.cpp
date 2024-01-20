#include "../../include/Server.hpp"

/* ************************************************************************** */
/*                              SPECIAL METHODS                               */
/* ************************************************************************** */

Server::Server() {
    configPath = DEFAULT_CONFIG;
	port = 0;
	host.s_addr = 0;
	serverName = "";
	root = "";
	socketFd = -1;
	ipAddress = "";
	clientSize = 0;
    // translates SUPPORTED_CGI macro to usable map
    std::vector<std::string> cgiPairs = splitString(SUPPORTED_CGI, '&');
    for (size_t i = 0; i < cgiPairs.size(); ++i) {
        std::vector<std::string> pair = splitString(cgiPairs[i], '=');
        supportedCGI[pair[0]] = pair[1];
    }
}

Server::Server(const Server &server) : port(server.port), host(server.host),
serverName(server.serverName), root(server.root), index(server.index),
ipAddress(server.ipAddress), clientSize(server.clientSize), errorPages(server.errorPages),
socketFd(server.socketFd){
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
		this->socketFd = server.socketFd;
	}
	return *this;
}

Server::~Server() {
	closeServer();
}

/* ************************************************************************** */
/*                                  SETTERS                                   */
/* ************************************************************************** */

void Server::setConfigPath(std::string path) {
    this->configPath = path;
}

void Server::setPort(unsigned short port) {
	this->port = port;
}

void Server::setHost(in_addr_t host) {
	this->host.s_addr = host;
}

void Server::setName(std::string name) {
	this->serverName = name;
}

void Server::setRoot(std::string root) {
	this->root = root;
}

void Server::setIndex(std::string index) {
	this->index.push_back(index);
}

void Server::setIP() {
	if (!host.s_addr || !port)
		return;
	this->ipAddress = inet_ntoa(this->host);
	this->ipAddress += ":";
	this->ipAddress += this->port;
}

void Server::setClientSize(unsigned long clientSize) {
	this->clientSize = clientSize;
}

/* ************************************************************************** */
/*                                  GETTERS                                   */
/* ************************************************************************** */

std::string getRoot() {
    return (this->root);
}

std::vector<std::string> getIndex() {
    return (this->index);
}

std::string Server::getCGIInterpreter(std::string &extension) {
    CGIList::iterator it = supportedCGI.find(extension);
    if (it != supportedCGI.end()) {
        return (it->second);
    } else {
        return ("")
    }
}
