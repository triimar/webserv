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
		throw std::runtime_error("Error: wrong file format.");
	str.open(file);
	if (!str.is_open())
		throw std::runtime_error("Error: cannot open config file.");
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
	unsigned long keySize = 11;
	std::string keywords[] = {"listen", "server_name", "client_size", "autoindex", "error_page", "root", "host", "location", "index", "cgi_info", "client_body_size"};
	unsigned long i;
	for (i = 0; i < keySize; i++)
	{
		if (line.find(keywords[i]) != std::string::npos)
			break;
	}
	if (i == keySize)
		throw std::runtime_error("Config file error: invalid keyword.");
	std::stringstream ss(line);
	std::string word;
	switch (i) {
		//LISTEN
		case 0: {
			while (ss >> word && word != keywords[i])
				throw std::runtime_error("Config file error: invalid keyword format on listen command.");
			ss >> word;
			if (word.empty() || word[word.length() - 1] != ';')
				throw std::runtime_error("Config file error: invalid keyword format on listen command.");
			word.erase(word.length() - 1);
			std::stringstream port_ss(word);
			unsigned short port;
			if (!(port_ss >> port))
				throw std::runtime_error("Config file error: invalid port format on listen command.");
			server.setPort(port);
			if (ss >> word)
				throw std::runtime_error("Config file error: line should end after \";\".");
			break;
		}
		//SERVER_NAME
		case 1: {
			while (ss >> word && word != keywords[i])
				throw std::runtime_error("Config file error: invalid keyword format on server_name command.");
			while (ss >> word && word[word.length() - 1] != ';')
				server.setName(word);
			if (word.empty() || word[word.length() - 1] != ';')
				throw std::runtime_error("Config file error: invalid keyword format on server_name command.");
			word.erase(word.length() - 1);
			server.setName(word);
			if (ss >> word)
				throw std::runtime_error("Config file error: line should end after \";\"");
			break;
		}
		//CLIENT_SIZE
		case 2: {
			while (ss >> word && word != keywords[i])
				throw std::runtime_error("Config file error: invalid keyword format on client_size command.");
			ss >> word;
			if (word.empty() || word[word.length() - 1] != ';')
				throw std::runtime_error("Config file error: invalid keyword format on client_size command.");
			word.erase(word.length() - 1);
			std::stringstream client_ss(word);
			unsigned long client;
			if (!(client_ss >> client))
				throw std::runtime_error("Config file error: invalid size format on client_size command.");
			server.setClientSize(client);
			if (ss >> word)
				throw std::runtime_error("Config file error: line should end after \";\"");
			break;
		}
		//AUTOINDEX
		case 3: {
			while (ss >> word && word != keywords[i])
				throw std::runtime_error("Config file error: invalid keyword format on autoindex command.");
			ss >> word;
			if (word.empty() || word[word.length() - 1] != ';')
				throw std::runtime_error("Config file error: invalid keyword format on autoindex command.");
			word.erase(word.length() - 1);
			server.setAutoIndex(word);
			if (ss >> word)
				throw std::runtime_error("Config file error: line should end after \";\" .");
			break;
		}
		//ERROR_PAGE
		case 4: {
			std::vector<int> errorCodes;
			while (ss >> word && word != keywords[i])
				throw std::runtime_error("Config file error: invalid keyword format on error_page command.");
			while (ss >> word && word[word.length() - 1] != ';')
				errorCodes.push_back(atoi(word.c_str()));
			if (word.empty() || word[word.length() - 1] != ';')
				throw std::runtime_error("Config file error: invalid keyword format on error_page command.");
			word.erase(word.length() - 1);
			for(std::vector<int>::iterator it = errorCodes.begin();
			it != errorCodes.end(); it++)
				server.setErrorPage(*it, word);
			if (ss >> word)
				throw std::runtime_error("Config file error: line should end after \";\".");
			break;
		}
		//ROOT
		case 5: {
			while (ss >> word && word != keywords[i])
				throw std::runtime_error("Config file error: invalid keyword format on root command.");
			ss >> word;
			if (word.empty() || word[word.length() - 1] != ';')
				throw std::runtime_error("Config file error: invalid keyword format on root command.");
			word.erase(word.length() - 1);
			server.setRoot(word);
			if (ss >> word)
				throw std::runtime_error("Config file error: line should end after \";\" .");
			break;
		}
		//HOST
		case 6: {
			while (ss >> word && word != keywords[i])
				throw std::runtime_error("Config file error: invalid keyword format on host command.");
			ss >> word;
			if (word.empty() || word[word.length() - 1] != ';')
				throw std::runtime_error("Config file error: invalid keyword format on host command.");
			word.erase(word.length() - 1);
			server.setHost(word);
			if (ss >> word)
				throw std::runtime_error("Config file error: line should end after \";\" .");
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
				throw std::runtime_error("Config file error: invalid keyword format on index command.");
			while (ss >> word && word[word.length() - 1] != ';')
				server.setIndex(word);
			if (word.empty() || word[word.length() - 1] != ';')
				throw std::runtime_error("Config file error: invalid keyword format on index command.");
			word.erase(word.length() - 1);
			server.setIndex(word);
			if (ss >> word)
				throw std::runtime_error("Config file error: line should end after \";\"");
			break;
		}
		//CGI INFO
		case 9: {
			while (ss >> word && word != keywords[i])
				throw std::runtime_error("Config file error: invalid keyword format on cgi_info command.");
			while (ss >> word && word[word.length() - 1] != ';')
				server.setCgiInfo(word);
			if (word.empty() || word[word.length() - 1] != ';')
				throw std::runtime_error("Config file error: invalid keyword format on cgi_info command.");
			word.erase(word.length() - 1);
			server.setCgiInfo(word);
			if (ss >> word)
				throw std::runtime_error("Config file error: line should end after \";\"");
			break;
		}
		//CLIENT BODY SIZE
		case 10: {
			while (ss >> word && word != keywords[i])
				throw std::runtime_error("Config file error: invalid keyword format on client_body_size command.");
			ss >> word;
			if (word.empty() || word[word.length() - 1] != ';')
				throw std::runtime_error("Config file error: invalid keyword format on client_body_size command.");
			word.erase(word.length() - 1);
			std::stringstream client_ss(word);
			unsigned long client;
			if (!(client_ss >> client))
				throw std::runtime_error("Config file error: invalid size format on client_body_size command.");
			server.setClientBody(client);
			if (ss >> word)
				throw std::runtime_error("Config file error: line should end after \";\"");
			break;
		}
		default:
			std::cerr << line << "\n";
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
				std::cerr << word << "\n";
				throw std::runtime_error("Config file error: invalid format.");
			}
			if (ss >> word && word != "{")
			{
				std::cerr << word << std::endl;
				throw std::runtime_error("Config file error: invalid format.");
			}
			if (word != "{")
			{
				std::getline(str, line);
				while (!str.eof() && line.find('{') == std::string::npos)
				{
					if (!isEmptyLine(line))
						throw std::runtime_error("Config file error: wrong format.");
					std::getline(str, line);
				}
			}
			if (str.eof() || line.find('{') != line.length() - 1)
				throw std::runtime_error("Config file error: invalid format.");
			Server server;
			std::getline(str, line);
			while (!str.eof() && line.find('}') == std::string::npos)
			{
				parseServerLine(server, line);
				std::getline(str, line);
			}
            server.setDefaultClientSize();
			server.setIP();
			server.autoCompleteLocations();
			list.push_back(server);
		}
		else if (!isEmptyLine(line))
			throw std::runtime_error("Config file error: invalid format.");
		if (!str.eof())
			std::getline(str, line);
	}
	this->serverList = list;
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

void Config::startServers() {
    if (this->serverList.size() == 1) {
        std::cout << "Starting server..." << std::endl;
    } else {
        std::cout << "Starting servers..." << std::endl;
    }
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
	this->clientList.at(fd).getServer()->removeClient();
	this->clientList.erase(fd);
    close(fd);
	fds.erase(fds.begin() + index);
	index--;
}

void Config::closeTimeoutClients() {
	int i = 0;
	for (std::map<int, Client>::iterator it = clientList.begin(); it != clientList.end();) {
		if (it->second.isTimeout()){
			close(it->second.getClientFd());
			it->second.getServer()->removeClient();
			fds.erase(fds.begin() + serverList.size() + i);
			std::map<int, Client>::iterator tmp = it;
			it++;
			clientList.erase(tmp);
		}
		else
		{
			it++;
			i++;
		}
	}
}

void Config::runServers() {
	if (signal(SIGINT, Config::sigintHandler) == SIG_ERR) {
		perror("signal");
		exit(EXIT_FAILURE);
	}
    if (this->serverList.size() == 1) {
        std::cout << "Server is running!" << std::endl;
    } else {
        std::cout << "Servers are running!" << std::endl;
    }
	while (running) {
		this->closeTimeoutClients();
        this->printClientsInfo();
		int eventNr = poll(fds.data(), fds.size(), POLL_TIMEOUT * 1000);
		if (eventNr == -1) {
			if (!running)
				break;
			throw std::runtime_error("Poll error.");
		}
		if (eventNr == 0)
			continue;
		for (size_t i = 0; i < fds.size() && eventNr; i++) {
            this->printClientsInfo();

			int current_fd = fds[i].fd;

			//ACCEPTING CONNECTIONS

			if (i < serverList.size() && (fds[i].revents & POLLIN)) {
				try {
                    if (fds.size() + 1 > FD_LIMIT) {
                        throw std::runtime_error("Client starting error: file descriptor limit exceeded.");
                    }
					Client newClient(&serverList[i]);
					this->clientList.insert(std::pair<int, Client>(newClient.getClientFd(), newClient));
					addFdToPoll(newClient.getClientFd());
					eventNr--;
                    this->clientList.at(newClient.getClientFd()).setActivity("Connection accepted");
				}
				catch (std::exception &e) {
					eventNr--;
					continue;
				}
			}

			//HANDLING REQUESTS
			
			else if (i >= serverList.size() && fds[i].revents & POLLIN) { // Check if the file descriptor has data to read
				char buf[BUFFER_SIZE];
				ssize_t num_read = read(current_fd, buf, BUFFER_SIZE);
				if (num_read == -1) {
					printClientsInfo("Could not read from client");
					closeClient(current_fd, i);
					continue;
				}
				if (num_read == 0) {
					// Connection closed by client
					closeClient(current_fd, i);
					eventNr--;
					continue;
				}
				Client &currentClient = clientList.at(current_fd);
				currentClient.getRequest().processRequest(buf, num_read);

				if (currentClient.getRequest().requestComplete()) {
					currentClient.confirmKeepAlive();
                    currentClient.setActivity("Request received");
					fds[i].events = POLLOUT;
				} else {
                    currentClient.setActivity("Reading request...");
					fds[i].events = POLLIN;
                }
				eventNr--;
			}

			//HANDLING RESPONSES

			else if (i >= serverList.size() && fds[i].revents & POLLOUT) {
				Client &currentClient = clientList.at(current_fd);

				if (currentClient.getResponse().empty() || currentClient.getFinishedChunked())
				{
                    currentClient.setActivity("Processing request...");
                    printClientsInfo();
					Response response(*currentClient.getServer(), currentClient.getRequest());
					currentClient.setResponse(response.getResponse());
				}
				std::vector<char> &currentResponse = currentClient.getResponse();

				ssize_t sentSize = send(current_fd, currentResponse.data(), currentResponse.size(), 0);

				if (sentSize < 0)
				{
					printClientsInfo("Could not write in client socket");
					closeClient(current_fd, i);
					continue;
				} else if (sentSize < static_cast<ssize_t>(currentResponse.size()))
				{
					currentResponse.erase(currentResponse.begin(), currentResponse.begin() + sentSize);
					currentClient.setChunkedUnfinished();
                    currentClient.setActivity("Sending response...");
					fds[i].revents = POLLOUT;
				}
				else {
					currentClient.setChunkedFinished();
                    currentClient.setActivity("Response sent");
				}
				if (!currentClient.getKeepAlive() && currentClient.getFinishedChunked()) {
					closeClient(current_fd, i);
					eventNr--;
					continue;
				} else if (currentClient.getKeepAlive() && currentClient.getFinishedChunked()){
                    currentClient.setActivity("Being kept alive...");
					currentClient.updateTime();
					fds[i].events = POLLIN;
					currentClient.getRequest().resetRequest();
				}
				eventNr--;
			}

			//HANDLING ERRORS

			else if (i >= serverList.size() && fds[i].revents & POLLERR) {
				printClientsInfo("Poll error encountered");
				closeClient(current_fd, i);
				continue;
			}

			//HANDLING HANGUPS

			else if (i >= serverList.size() && fds[i].revents & POLLHUP) {
				printClientsInfo("Client hang up");
				closeClient(current_fd, i);
				continue;
			}
		}
	}
	for (std::vector<pollfd>::iterator it = fds.begin(); it != fds.end(); it++) {
		close(it->fd);
	}
	fds.clear();
    this->printClientsInfo();
    clientList.clear();
    if (this->serverList.size() == 1) {
        std::cout << "Server shutdown!" << std::endl;
    } else {
        std::cout << "Servers shutdown!" << std::endl;
    }
}

void Config::printClientsInfo(std::string error) {
    static size_t prevAmount;
    for (size_t i = 0; i < prevAmount; ++i) {
        std::cout << "\033[A\033[K\r";
    }
    if (running == false) {
        if (clientList.size() != 0) {
            std::cout << "Disconnected all clients" << std::endl;
        }
        return ;
    }
    if (error.empty() == false) {
        std::cerr << error << std::endl;
    }
    if (clientList.size() == 0) {
        std::cout << "Waiting for clients..." << std::endl;
        prevAmount = 1;
    } else {
        for (std::map<int, Client>::iterator it = clientList.begin(); it != clientList.end(); ++it) {
            std::cout << "Client on fd " << it->first << ": " << it->second.getActivity() << std::endl;
        }
        prevAmount = clientList.size();
    }
}
