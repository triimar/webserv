#include "../../include/Server.hpp"

Server::Server() {
	port = 0;
	host.s_addr = 0;
	root = "";
	socketFd = -1;
	ipAddress = "";
	clientSize = 0;
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

void Server::startServer() {
	this->socketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketFd < 0)
		throw std::runtime_error("Error: unable to start server.\n");

	socketLen = sizeof(socketAddress);
	memset(&socketAddress, 0, sizeof(socketAddress));
	this->socketAddress.sin_family = AF_INET;
	this->socketAddress.sin_port = htons(port);
	this->socketAddress.sin_addr = host;

//	std::cout << "Socketfd = " << socketFd << "\nSocket address fam = " << (int) socketAddress.sin_family <<
//	"\nSocket port = " << socketAddress.sin_port << "\nSocket addr = " << socketAddress.sin_addr.s_addr <<
//	"\n Socket length = " << socketLen << std::endl;
//
//	std::cout << "Port: " << port << std::endl;
	if (bind(socketFd, (sockaddr *)&socketAddress, (socklen_t)socketLen) < 0)
	{
		perror("Error binding socket");
		throw std::runtime_error("Error: cannot connect socket to address.\n");
	}
}

void Server::startListen() {
	if (listen(socketFd, 20) < 0)
		throw std::runtime_error("Error: socket listen failed.\n");
	std::ostringstream ss;
	ss << "\n*** Listening on ADDRESS: "
	   << inet_ntoa(socketAddress.sin_addr)
	   << " PORT: " << ntohs(socketAddress.sin_port)
	   << " ***\n\n";
	std::cout << ss.str() << std::endl;

	int bytesReceived;

	while (true)
	{
		std::cout << ("====== Waiting for a new connection ======\n\n\n");
		acceptConnection(newSocket);

		char buffer[BUFFER_SIZE] = {0};
		bytesReceived = read(newSocket, buffer, BUFFER_SIZE);
		if (bytesReceived < 0)
		{
			perror("Failed to read bytes from client socket connection");
			exit(1);
		}

		std::ostringstream ss;
		ss << "------ Received Request from client ------\n\n";
		std::cout << ss.str() << std::endl << buffer;

//		sendResponse();

		close(newSocket);
	}
}

void Server::acceptConnection(int &newSocket) {
	newSocket = accept(socketFd, (sockaddr *)&socketAddress, &socketLen);
	if (newSocket < 0)
	{
		std::ostringstream ss;
		ss << "Server failed to accept incoming connection from ADDRESS: " << inet_ntoa(socketAddress.sin_addr) << "; PORT: " << ntohs(socketAddress.sin_port);
		std::cerr << ss.str() << std::endl;
		exit(1);
	}
}

void Server::closeServer() {
	close(socketFd);
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
	this->root = root;
}

void Server::setIndex(std::string index) {
	this->index.push_back(index);
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

void Server::setErrorPage(std::string errorPage) {
	this->errorPages.push_back(errorPage);
}

void printList(std::string index)
{
	std::cout << "\t" << index << std::endl;
}

void Server::printServer(Server &server) {
	std::cout << "Server:";
	std::for_each(server.serverName.begin(), server.serverName.end(), printList);
	std::cout << "IP: " << server.ipAddress <<
	"\nRoot: " << server.root <<
	"\nIndex: ";
	std::for_each(server.index.begin(), server.index.end(), printList);
	std::cout << "Client Size: " << server.clientSize <<
	"\nError Pages:";
	std::for_each(server.errorPages.begin(), server.errorPages.end(), printList);
	std::for_each(server.locations.begin(), server.locations.end(), Location::printLocation);
}

void Server::setLocation(std::string line, std::ifstream &stream) {
	Location location;

	std::stringstream ss(line);
	std::string word;
	std::string keywords[] = {"allow", "root", "index"};
	while (ss >> word && word != "location")
		throw std::runtime_error("Config file error location a: invalid keyword format.\n");
	ss >> word;
	if (word.empty() || word == "{")
		throw std::runtime_error("Config file error location b: location should be followed by a directory.\n");
	location.setName(word);
	if (!(ss >> word) || word != "{")
		throw std::runtime_error("Config file error location c: location directory should be followed by a '{'.\n");
	std::getline(stream, line);
	while (stream.good() && line.find('}') == std::string::npos)
	{
//		std::cout << line << "\n";
		if (line.empty())
		{
			std::getline(stream, line);
			continue;
		}
		std::stringstream lss(line);
		lss >> word;
		int i;
		for (i = 0; i < 3; i++)
			if (word == keywords[i])
				break;
		switch (i){
			//ALLOW
			case 0: {
				while (lss >> word && word.find(";") == std::string::npos)
					location.setMethod(word);
				if (word.empty() || word[word.length() - 1] != ';')
					throw std::runtime_error("Config file error location d: invalid keyword format.\n");
				word.erase(word.length() - 1);
				location.setMethod(word);
				break;
			}
			//ROOT
			case 1: {
				if (!(lss >> word) || word[word.length() - 1] != ';')
					throw std::runtime_error("Config file error location e: invalid keyword format.\n");
				word.erase(word.length() - 1);
				location.setRoot(word);
				break;
			}
			//INDEX
			case 2: {
				while (lss >> word && word.find(";") == std::string::npos)
					location.setIndex(word);
				if (word.empty() || word[word.length() - 1] != ';')
					throw std::runtime_error("Config file error location f: invalid keyword format.\n");
				word.erase(word.length() - 1);
				location.setIndex(word);
				break;
			}
			default:
			{
				std::cout << word << "\n";
				throw std::runtime_error("Config file error location g: invalid keyword format.\n");
			}
		}
		std::getline(stream, line);
	}
	if (!stream.eof())
	{
		this->locations.push_back(location);
	}
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

//std::vector<std::string> Server::getIndex(std::string location) const{
//    return (this->index);
//}

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
