#include "../../include/webserv.hpp"

bool running = true;

// Config::Config() {}

/**
 * Constructor for the configuration class. It will only accept file names
 * with the ".config" postfix and subsequently open an ifstream. If the
 * ifstream doesn't open, or the name of the file is wrong, it will throw
 * an error.
 * @param file address of the file that will be parsed*/
Config::Config(const char *file) {
	const char *conf = strstr(file, ".conf");
	if (!conf || strlen(file) == 5 || strcmp(conf, &file[strlen(file) - 5]))
		throw std::runtime_error("Error: wrong file format.\n");
	str.open(file);
	if (!str.is_open())
		throw std::runtime_error("Error: cannot open config file.\n");
	createServers();
}

/**This should not be called. It doesn't do anything*/
Config::Config(const Config &file) : serverList(file.serverList), clientList(file.clientList){
	if (file.str.is_open())
		return;
}

/**
 * Destructor that closes the ifstream file.
 */
Config::~Config() {
	if (str.is_open())
		str.close();
	// std::cout << "Deconstructor called!\n";
}

/**
 * Checks if a line contains any of the characters that would deem the line
 * as "empty": new line, tab, space.
 * @param line the line to check for the characters
 * @return true if the line contains only the previously specified
 * characters or if it is empty, false otherwise
 */
bool Config::isEmptyLine(std::string line) {
	std::string allowedChars = " \n\t";
	for (std::string::const_iterator it = line.begin(); it != line.end(); ++it)
		if (allowedChars.find(*it) == std::string::npos)
			return false;
	return true;
}

/**
 * This function takes a line from the server block of the config file and parses the intended attribute of the server.
 * @param server
 * @param line
 */
void Config::parseServerLine(Server &server, std::string line) {
	if (isEmptyLine(line))
		return;
	unsigned long keySize = 10;
	std::string keywords[] = {"listen", "server_name", "client_size", "autoindex", "error_page", "root", "host", "location", "index", "cgi_info"};
	unsigned long i;
	for (i = 0; i < keySize; i++)
	{
		if (line.find(keywords[i]) != std::string::npos)
			break;
	}
	if (i == keySize)
		throw std::runtime_error("Config file error: invalid keyword.\n");
	std::stringstream ss(line);
	std::string word;
	switch (i) {
		//LISTEN
		case 0: {
			while (ss >> word && word != keywords[i])
				throw std::runtime_error("Config file error: invalid keyword format on listen command.\n");
			ss >> word;
			if (word.empty() || word[word.length() - 1] != ';')
				throw std::runtime_error("Config file error: invalid keyword format on listen command.\n");
			word.erase(word.length() - 1);
			std::stringstream port_ss(word);
			unsigned short port;
			if (!(port_ss >> port))
				throw std::runtime_error("Config file error: invalid port format on listen command.\n");
			server.setPort(port);
			if (ss >> word)
				throw std::runtime_error("Config file error: line should end after \";\".\n");
			break;
		}
		//SERVER_NAME
		case 1: {
			while (ss >> word && word != keywords[i])
				throw std::runtime_error("Config file error: invalid keyword format on server_name command.\n");
			while (ss >> word && word[word.length() - 1] != ';')
				server.setName(word);
			if (word.empty() || word[word.length() - 1] != ';')
				throw std::runtime_error("Config file error: invalid keyword format on server_name command.\n");
			word.erase(word.length() - 1);
			server.setName(word);
			if (ss >> word)
				throw std::runtime_error("Config file error: line should end after \";\"\n");
			break;
		}
		//CLIENT_SIZE
		case 2: {
			while (ss >> word && word != keywords[i])
				throw std::runtime_error("Config file error: invalid keyword format on client_size command.\n");
			ss >> word;
			if (word.empty() || word[word.length() - 1] != ';')
				throw std::runtime_error("Config file error: invalid keyword format on client_size command.\n");
			word.erase(word.length() - 1);
			std::stringstream client_ss(word);
			unsigned long client;
			if (!(client_ss >> client))
				throw std::runtime_error("Config file error: invalid port format on client_size command.\n");
			server.setClientSize(client);
			if (ss >> word)
				throw std::runtime_error("Config file error: line should end after \";\"\n");
			break;
		}
		//AUTOINDEX
		case 3: {
			while (ss >> word && word != keywords[i])
				throw std::runtime_error("Config file error: invalid keyword format on autoindex command.\n");
			ss >> word;
			if (word.empty() || word[word.length() - 1] != ';')
				throw std::runtime_error("Config file error: invalid keyword format on autoindex command.\n");
			word.erase(word.length() - 1);
			server.setAutoIndex(word);
			if (ss >> word)
				throw std::runtime_error("Config file error: line should end after \";\" .\n");
			break;
		}
		//ERROR_PAGE
		case 4: {
			std::vector<int> errorCodes;
			while (ss >> word && word != keywords[i])
				throw std::runtime_error("Config file error: invalid keyword format on error_page command.\n");
			while (ss >> word && word[word.length() - 1] != ';')
				errorCodes.push_back(atoi(word.c_str()));
			if (word.empty() || word[word.length() - 1] != ';')
				throw std::runtime_error("Config file error: invalid keyword format on error_page command.\n");
			word.erase(word.length() - 1);
			for(std::vector<int>::iterator it = errorCodes.begin();
			it != errorCodes.end(); it++)
				server.setErrorPage(*it, word);
			if (ss >> word)
				throw std::runtime_error("Config file error: line should end after \";\".\n");
			break;
		}
		//ROOT
		case 5: {
			while (ss >> word && word != keywords[i])
				throw std::runtime_error("Config file error: invalid keyword format on root command.\n");
			ss >> word;
			if (word.empty() || word[word.length() - 1] != ';')
				throw std::runtime_error("Config file error: invalid keyword format on root command.\n");
			word.erase(word.length() - 1);
			server.setRoot(word);
			if (ss >> word)
				throw std::runtime_error("Config file error: line should end after \";\" .\n");
			break;
		}
		//HOST
		case 6: {
			while (ss >> word && word != keywords[i])
				throw std::runtime_error("Config file error: invalid keyword format on host command.\n");
			ss >> word;
			if (word.empty() || word[word.length() - 1] != ';')
				throw std::runtime_error("Config file error: invalid keyword format on host command.\n");
			word.erase(word.length() - 1);
			server.setHost(word);
			if (ss >> word)
				throw std::runtime_error("Config file error: line should end after \";\" .\n");
			break;
		}
		//LOCATION
		case 7: {
			server.setLocation(line, str);
			break;
		}
		//INDEX
		case 8: {
			while (ss >> word && word != keywords[i])
				throw std::runtime_error("Config file error: invalid keyword format on index command.\n");
			while (ss >> word && word[word.length() - 1] != ';')
				server.setIndex(word);
			if (word.empty() || word[word.length() - 1] != ';')
				throw std::runtime_error("Config file error: invalid keyword format on index command.\n");
			word.erase(word.length() - 1);
			server.setIndex(word);
			if (ss >> word)
				throw std::runtime_error("Config file error: line should end after \";\"\n");
			break;
		}
		case 9: {
			while (ss >> word && word != keywords[i])
				throw std::runtime_error("Config file error: invalid keyword format on cgi_info command.\n");
			while (ss >> word && word[word.length() - 1] != ';')
				server.setCgiInfo(word);
			if (word.empty() || word[word.length() - 1] != ';')
				throw std::runtime_error("Config file error: invalid keyword format on cgi_info command.\n");
			word.erase(word.length() - 1);
			server.setCgiInfo(word);
			if (ss >> word)
				throw std::runtime_error("Config file error: line should end after \";\"\n");
			break;
		}
		default:
			std::cout << line << "\n";
			break;
	}
}

void Config::createServers() {
	std::vector<Server> list;
	std::string line;
	std::getline(str, line);

	while (!str.eof())
	{
		if (line.find("server") != std::string::npos)
		{
			std::stringstream ss(line);
			std::string word;
			ss >> word;
			if (word != "server")
			{
				std::cout << word << "\n";
				throw std::runtime_error("Config file error: invalid format.\n");
			}
			if (ss >> word && word != "{")
			{
				std::cout << word << std::endl;
				throw std::runtime_error("Config file error: invalid format.\n");
			}
			if (word != "{")
			{
				std::getline(str, line);
				while (!str.eof() && line.find('{') == std::string::npos)
				{
					if (!isEmptyLine(line))
						throw std::runtime_error("Config file error: wrong format.\n");
					std::getline(str, line);
				}
			}
			if (str.eof() || line.find('{') != line.length() - 1)
				throw std::runtime_error("Config file error: invalid format.\n");
			Server server;
			std::getline(str, line);
			while (!str.eof() && line.find('}') == std::string::npos)
			{
				parseServerLine(server, line);
				std::getline(str, line);
			}
			server.setIP();
			server.autoCompleteLocations();
			list.push_back(server);
		}
		else if (!isEmptyLine(line))
			throw std::runtime_error("Config file error: invalid format.\n");
		if (!str.eof())
			std::getline(str, line);
	}
//	close(str);
	this->serverList = list;
//	return list;
}

void Config::printServers() {
	std::for_each(this->serverList.begin(), this->serverList.end(), Server::printServer);
}

void Config::addFdToPoll(int fd) {
	struct pollfd newPollFd;
	newPollFd.fd = fd;
	newPollFd.events =  POLLIN;
	this->fds.push_back(newPollFd);
}

std::vector <Server> Config::getServerList() {
	return this->serverList;
}

std::map<int, Client> Config::getClientMap() {
	return this->clientList;
}

void Config::closeTimeoutClients() {
	int i = 0;
	for (std::map<int, Client>::iterator it = clientList.begin(); it != clientList.end();) {
        // loop through fds[].events & POLL
		if (it->second.isTimeout()){
			std::cout << "Client timeout\n";
			close(it->second.getClientFd());
			fds.erase(fds.begin() + serverList.size() + i);
			std::map<int, Client>::iterator tmp = it;
			tmp++;
			// it = clientList.erase(it);
			clientList.erase(it);
			it = tmp;
		}
		else
		{
			it++;
			i++;
		}
	}
}

void Config::startServers() {
	for(std::vector<Server>::iterator it = this->serverList.begin(); it != this->serverList.end(); it++)
	{
		it->startServer();
		addFdToPoll(it->getSocketFd());
	}
}

void Config::sigintHandler(int signum) {
	(void) signum;
	running = false;
}

void Config::closeClient(int fd, size_t &index) {
	this->clientList.erase(fd);
	fds.erase(fds.begin() + index);
	index--;
}

void Config::runServers() {
	if (signal(SIGINT, Config::sigintHandler) == SIG_ERR) {
		perror("signal");
		exit(EXIT_FAILURE);
	}

	while (running) {
		this->closeTimeoutClients();
		int eventNr = poll(fds.data(), fds.size(), POLL_TIMEOUT * 1000);
		std::cout << fds.size() << " size\n";
		if (eventNr == -1) {
			if (!running)
				break;
			throw std::runtime_error("Poll error.\n");
		}
		if (eventNr == 0)
			continue;
		for (size_t i = 0; i < fds.size() && eventNr; i++) {
			int current_fd = fds[i].fd;
			std::cout << i << " server, " << fds.size() << "\n";

			//ACCEPTING CONNECTIONS

			if (i < serverList.size() && (fds[i].revents & POLLIN)) {
				try {
					std::cout << "Setting up new client\n";
					Client newClient(serverList[i]);
					this->clientList.insert(std::pair<int, Client>(newClient.getClientFd(), newClient));
					addFdToPoll(newClient.getClientFd());
					eventNr--;
					std::cout << "New client set up\n";
				}
				catch (std::exception &e) {
					std::cout << "Error accepting new client\n";
					continue;
				}
			}

			//HANDLING REQUESTS

			else if (i >= serverList.size() && fds[i].revents & POLLIN) { // Check if the file descriptor has data to read
				char buf[1024];
				ssize_t num_read = read(current_fd, buf, sizeof(buf));
                std::cout << "GOT " << num_read << " bytes" << std::endl;
				if (num_read == -1) {
					perror("Could not read from client");
					closeClient(current_fd, i);
					continue;
				}
				if (num_read == 0) {
					// Connection closed by client
					std::cout << "Client with fd " << fds[i].fd << " closed the connection." << std::endl;
					closeClient(current_fd, i);
					eventNr--;
					continue;
				}
				Client &currentClient = clientList.at(current_fd);
				currentClient.getRequest().processRequest(buf, num_read);
                // std::cout << currentClient.getRequest();
				if (currentClient.getRequest().requestComplete()) {
					currentClient.confirmKeepAlive();
					fds[i].events = POLLOUT;
				} else
					fds[i].events = POLLIN;
				eventNr--;
			}

			//HANDLING RESPONSES

			else if (i >= serverList.size() && fds[i].revents & POLLOUT) {
				Client &currentClient = clientList.at(current_fd);
				std::cout << currentClient.getRequest();
				std::vector<char> &currentResponse = currentClient.getResponse();

				if (currentResponse.empty() || currentClient.getFinishedChunked())
				{
					Response response(currentClient.getServer(), currentClient.getRequest());
					currentClient.setResponse(response.getResponse());
					currentResponse = currentClient.getResponse();
				}
                std::cout << "------RESPONSE-------------------" << std::endl;
                std::cout << "Response size: " << currentResponse.size() << std::endl;
                // for (std::vector<char>::const_iterator it  = currentResponse.begin(); it != currentResponse.end(); ++it) {
                //     std::cout << *it;
                // }
                std::cout << "------END RESPONSE---------------" << std::endl;
				ssize_t sentSize = send(current_fd, currentResponse.data(), currentResponse.size(), 0);
				std::cout << "SENT SIZE: " << sentSize << std::endl;
				if (sentSize < 0)
				{
					perror("Could not write in client socket");
					closeClient(current_fd, i);
					continue;
				} else if (sentSize < static_cast<ssize_t>(currentResponse.size()))
				{
					currentResponse.erase(currentResponse.begin(), currentResponse.begin() + sentSize);
					currentClient.setChunkedUnfinished();
					fds[i].revents = POLLOUT;
				}
				else {
					currentClient.setChunkedFinished();
				}
                std::cout << "Response size after: " << currentResponse.size() << std::endl;
                // std::cout << "finished? " << std::boolalpha << currentClient.getFinishedChunked() << " & keep-alive? " << std::boolalpha << currentClient.getKeepAlive() << std::endl;
				if (!currentClient.getKeepAlive() && currentClient.getFinishedChunked()) {
					closeClient(current_fd, i);
					eventNr--;
					continue;
				} else if (currentClient.getKeepAlive() && currentClient.getFinishedChunked()){
					currentClient.updateTime();
					fds[i].events = POLLIN;
					currentClient.getRequest().resetRequest();
				}
				eventNr--;
			}

			//HANDLING ERRORS

			else if (i >= serverList.size() && fds[i].revents & POLLERR) {
				std::cout << "Poll error encountered\n";
				closeClient(current_fd, i);
				continue;
			}

			//HANDLING HANGUPS

			else if (i >= serverList.size() && fds[i].revents & POLLHUP) {
				std::cout << "Client hang up\n";
				closeClient(current_fd, i);
				continue;
			}
		}
	}
	for (std::vector<pollfd>::iterator it = fds.begin(); it != fds.end(); it++) {
		close(it->fd);
	}
	fds.erase(fds.begin(), fds.end());
}