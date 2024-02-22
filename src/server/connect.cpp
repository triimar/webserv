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

void printList(std::string index)
{
	std::cout << "\t" << index << std::endl;
}

void Server::printServer(Server &server) {
	std::cout << "Server:";
	std::for_each(server.serverName.begin(), server.serverName.end(), printList);
	std::cout << "IP: " << server.ipAddress <<
	"\nRoot: " << server.root <<
	"\nAutindex: ";
	if (server.autoindex)
		std::cout << "true";
	else
		std::cout << "false";
	std::cout << "\nIndex: ";
	std::for_each(server.index.begin(), server.index.end(), printList);
	std::cout << "Client Size: " << server.clientSize <<
	"\nError Pages:";
	for (std::map<unsigned int, std::string>::iterator it = server.errorPages.begin();
	it != server.errorPages.end(); it++)
	{
		std::cout << "\t" << it->first << ": " << it->second << std::endl;
	}
//	std::for_each(server.locations.begin(), server.locations.end(), Location::printLocation);
	for (std::map<std::string, Location>::iterator it = server.locations.begin(); it != server.locations.end(); ++it)
	{
		Location::printLocation(it->second);
	}
}

void Server::setLocation(std::string line, std::ifstream &stream) {
	Location location;

	std::stringstream ss(line);
	std::string word;
	std::string keywords[] = {"allow", "root", "index", "autoindex", "cgi_info"};
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
		for (i = 0; i < 5; i++)
			if (word == keywords[i])
				break;
		switch (i){
			//ALLOW
			case 0: {
				while (lss >> word && word.find(";") == std::string::npos)
					location.setMethod(word);
				if (word.empty() || word[word.length() - 1] != ';')
					throw std::runtime_error("Config file error location: invalid keyword format.\n");
				word.erase(word.length() - 1);
				location.setMethod(word);
				if (lss >> word)
					throw std::runtime_error("Config file error location: command should end after ;.\n");
				break;
			}
			//ROOT
			case 1: {
				if (!(lss >> word) || word[word.length() - 1] != ';')
					throw std::runtime_error("Config file error location: invalid keyword format.\n");
				word.erase(word.length() - 1);
				location.setRoot(word);
				if (lss >> word)
					throw std::runtime_error("Config file error location: command should end after ;.\n");
				break;
			}
			//INDEX
			case 2: {
				while (lss >> word && word.find(";") == std::string::npos)
					location.setIndex(word);
				if (word.empty() || word[word.length() - 1] != ';')
					throw std::runtime_error("Config file error location: invalid keyword format.\n");
				word.erase(word.length() - 1);
				location.setIndex(word);
				if (lss >> word)
					throw std::runtime_error("Config file error location: command should end after ;.\n");
				break;
			}
			case 3: {
				if (!(lss >> word) || word[word.length() - 1] != ';')
					throw std::runtime_error("Config file error location: invalid keyword format.\n");
				word.erase(word.length() - 1);
				location.setAutoIndex(word);
				if (lss >> word)
					throw std::runtime_error("Config file error location: command should end after ;.\n");
				break;
			}
			case 4: {
				while (lss >> word && word.find(";") == std::string::npos)
					location.setCgiInfo(word);
				if (word.empty() || word[word.length() - 1] != ';')
					throw std::runtime_error("Config file error location: invalid keyword format.\n");
				word.erase(word.length() - 1);
				location.setCgiInfo(word);
				if (lss >> word)
					throw std::runtime_error("Config file error location: command should end after ;.\n");
				break;
			}
			default: {
				// std::cout << word << "\n";
				throw std::runtime_error("Config file error location: invalid keyword.\n");
			}
		}
		std::getline(stream, line);
	}
	if (!stream.eof())
	{
		this->locations.insert(std::pair<std::string, Location>(location.getName(), location));
//		this->locations.push_back(location);
	}
}

//void Server::pushLocation() {
//	Location location;
//	this->locations.push_back(location);
//}