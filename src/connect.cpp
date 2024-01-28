#include "../../include/Server.hpp"

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
