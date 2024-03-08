#include "../../include/webserv.hpp"

void Server::startServer() {
	int optval = 1;
	this->socketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketFd < 0)
		throw std::runtime_error("Error: unable to start server.");

	this->socketAddress.sin_family = AF_INET;
	this->socketAddress.sin_port = htons(port);
	this->socketAddress.sin_addr = host;

	if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
	{
		perror("Error setting up socket");
		throw std::runtime_error("Error: cannot set socket flags.");
	}

	if (bind(socketFd, (sockaddr *)&socketAddress, sizeof(socketAddress)) < 0)
	{
		perror("Error binding socket");
		throw std::runtime_error("Error: cannot connect socket to address.");
	}

	if (listen(socketFd, 20) < 0)
		throw std::runtime_error("Error: socket listen failed.");

	if (fcntl(socketFd, F_SETFL, O_NONBLOCK) == -1)
	{
		throw std::runtime_error("Error: cannot set socket to non-blocking.");
	}
}

bool Server::acceptConnection() {
	if (connectedClients < this->clientSize)
		this->connectedClients++;
	else
		return false;
	return true;
}

void Server::removeClient(){
	connectedClients--;
}

void Server::closeServer() {
	close(socketFd);
}

void printList(std::string index)
{
	std::clog << "\t" << index << std::endl;
}

void Server::printServer(Server &server) {
	std::clog << "Server:";
	std::for_each(server.serverName.begin(), server.serverName.end(), printList);
	std::clog << "IP: " << server.ipAddress <<
	"\nRoot: " << server.root <<
	"\nAutindex: ";
	if (server.autoindex)
		std::clog << "true";
	else
		std::clog << "false";
	std::clog << "\nIndex: ";
	std::for_each(server.index.begin(), server.index.end(), printList);
	std::clog << "Client Size: " << server.clientSize <<
	"\nError Pages:";
	for (std::map<int, std::string>::const_iterator it = server.errorPages.begin();
	it != server.errorPages.end(); it++)
	{
		std::clog << "\t" << it->first << ": " << it->second << std::endl;
	}
	std::clog << "CGI Info: ";
	std::for_each(server.cgi_info.begin(), server.cgi_info.end(), printList);
	for (std::map<std::string, Location>::iterator it = server.locations.begin(); it != server.locations.end(); ++it)
	{
		Location::printLocation(it->second);
	}
	std::clog << std::endl;
}

void Server::setLocation(std::string line, std::ifstream &stream) {
	Location location;

	std::stringstream ss(line);
	std::string word;
	std::string keywords[] = {"allow", "root", "index", "autoindex", "cgi_info"};
	while (ss >> word && word != "location")
		throw std::runtime_error("Config file error location a: invalid keyword format.");
	ss >> word;
	if (word.empty() || word == "{")
		throw std::runtime_error("Config file error location b: location should be followed by a directory.");
	location.setName(word);
	if (!(ss >> word) || word != "{")
		throw std::runtime_error("Config file error location c: location directory should be followed by a '{'.");
	std::getline(stream, line);
	while (stream.good() && line.find('}') == std::string::npos)
	{
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
					throw std::runtime_error("Config file error location: invalid keyword format.");
				word.erase(word.length() - 1);
				location.setMethod(word);
				if (lss >> word)
					throw std::runtime_error("Config file error location: command should end after ;.");
				break;
			}
			//ROOT
			case 1: {
				if (!(lss >> word) || word[word.length() - 1] != ';')
					throw std::runtime_error("Config file error location: invalid keyword format.");
				word.erase(word.length() - 1);
				location.setRoot(word);
				if (lss >> word)
					throw std::runtime_error("Config file error location: command should end after ;.");
				break;
			}
			//INDEX
			case 2: {
				while (lss >> word && word.find(";") == std::string::npos)
					location.setIndex(word);
				if (word.empty() || word[word.length() - 1] != ';')
					throw std::runtime_error("Config file error location: invalid keyword format.");
				word.erase(word.length() - 1);
				location.setIndex(word);
				if (lss >> word)
					throw std::runtime_error("Config file error location: command should end after ;.");
				break;
			}
			case 3: {
				if (!(lss >> word) || word[word.length() - 1] != ';')
					throw std::runtime_error("Config file error location: invalid keyword format.");
				word.erase(word.length() - 1);
				location.setAutoIndex(word);
				if (lss >> word)
					throw std::runtime_error("Config file error location: command should end after ;.");
				break;
			}
			case 4: {
				while (lss >> word && word.find(";") == std::string::npos)
					location.setCgiInfo(word);
				if (word.empty() || word[word.length() - 1] != ';')
					throw std::runtime_error("Config file error location: invalid keyword format.");
				word.erase(word.length() - 1);
				location.setCgiInfo(word);
				if (lss >> word)
					throw std::runtime_error("Config file error location: command should end after ;.");
				break;
			}
			default: {
				// std::clog << word << "\n";
				throw std::runtime_error("Config file error location: invalid keyword.");
			}
		}
		std::getline(stream, line);
	}
	if (!stream.eof()) {
		this->locations.insert(std::pair<std::string, Location>(location.getName(), location));
	}
}
