#include "Server.hpp"

Server::Server() {
	port = 0;
	host.s_addr = 0;
	serverName = "";
	root = "";
	socketFd = -1;
	ipAddress = "";
	clientSize = 0;
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
		return;//Throw exception?
	this->ipAddress = inet_ntoa(this->host);
	this->ipAddress += ":";
	this->ipAddress += this->port;
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