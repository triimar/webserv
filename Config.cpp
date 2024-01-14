#include "Config.hpp"

Config::Config() {}

Config::Config(char *file) {
	char *conf = strstr(file, ".conf");
	if (!conf || strlen(file) == 5 || strcmp(conf, &file[strlen(file) - 5]))
		throw std::runtime_error("Error: wrong file format.\n");
	str.open(file);
	if (!str.is_open())
		throw std::runtime_error("Error: cannot open config file.\n");
}

Config::Config(const Config &file) {
	if (file.str.is_open())
		return;
}

Config::~Config() {
	if (str.is_open())
		str.close();
}


bool Config::isEmptyLine(std::string line) {
	std::string allowedChars = " \n\t";
	for (std::string::const_iterator it = line.begin(); it != line.end(); ++it)
		if (allowedChars.find(*it) == std::string::npos)
			return false;
	return true;
}

void Config::parseServerLine(Server &server, std::string line) {
	if (isEmptyLine(line))
		return;
	unsigned long keySize = 8;
	std::string keywords[] = {"listen", "server_name", "client_size", "index", "error_page", "root", "host", "location"};
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
				throw std::runtime_error("Config file error: invalid keyword format.\n");
			ss >> word;
			if (word.empty() || word[word.length() - 1] != ';')
				throw std::runtime_error("Config file error: invalid keyword format.\n");
			word.erase(word.length() - 1);
			std::stringstream port_ss(word);
			unsigned short port;
			if (!(port_ss >> port))
				throw std::runtime_error("Config file error: invalid port format.\n");
			server.setPort(port);
			if (ss >> word)
				throw std::runtime_error("Config file error: line should end after \";\" .\n");
			break;
		}
		//SERVER_NAME
		case 1: {
			while (ss >> word && word != keywords[i])
				throw std::runtime_error("Config file error: invalid keyword format.\n");
			ss >> word;
			if (word.empty() || word[word.length() - 1] != ';')
				throw std::runtime_error("Config file error: invalid keyword format.\n");
			word.erase(word.length() - 1);
			server.setName(word);
			if (ss >> word)
				throw std::runtime_error("Config file error: line should end after \";\" .\n");
			break;
		}
		//CLIENT_SIZE
		case 2: {
			while (ss >> word && word != keywords[i])
				throw std::runtime_error("Config file error: invalid keyword format.\n");
			ss >> word;
			if (word.empty() || word[word.length() - 1] != ';')
				throw std::runtime_error("Config file error: invalid keyword format.\n");
			word.erase(word.length() - 1);
			std::stringstream client_ss(word);
			unsigned long client;
			if (!(client_ss >> client))
				throw std::runtime_error("Config file error: invalid port format.\n");
			server.setClientSize(client);
			if (ss >> word)
				throw std::runtime_error("Config file error: line should end after \";\" .\n");
			break;
		}
		//INDEX
		case 3: {
			while (ss >> word && word != keywords[i])
				throw std::runtime_error("Config file error: invalid keyword format.\n");
			while (ss >> word && word[word.length() - 1] != ';')
				server.setIndex(word);
			if (word.empty() || word[word.length() - 1] != ';')
				throw std::runtime_error("Config file error: invalid keyword format.\n");
			word.erase(word.length() - 1);
			server.setIndex(word);
			if (ss >> word)
				throw std::runtime_error("Config file error: line should end after \";\" .\n");
			break;
		}
		//ERROR_PAGE
		case 4: {
			while (ss >> word && word != keywords[i])
				throw std::runtime_error("Config file error: invalid keyword format.\n");
			ss >> word;
			if (word.empty() || word[word.length() - 1] != ';')
				throw std::runtime_error("Config file error: invalid keyword format.\n");
			word.erase(word.length() - 1);
			server.setErrorPage(word);
			if (ss >> word)
				throw std::runtime_error("Config file error: line should end after \";\" .\n");
			break;
		}
		//ROOT
		case 5: {
			while (ss >> word && word != keywords[i])
				throw std::runtime_error("Config file error: invalid keyword format.\n");
			ss >> word;
			if (word.empty() || word[word.length() - 1] != ';')
				throw std::runtime_error("Config file error: invalid keyword format.\n");
			word.erase(word.length() - 1);
			server.setRoot(word);
			if (ss >> word)
				throw std::runtime_error("Config file error: line should end after \";\" .\n");
			break;
		}
		//HOST
		case 6: {
			while (ss >> word && word != keywords[i])
				throw std::runtime_error("Config file error: invalid keyword format.\n");
			ss >> word;
			if (word.empty() || word[word.length() - 1] != ';')
				throw std::runtime_error("Config file error: invalid keyword format.\n");
			word.erase(word.length() - 1);
			server.setHost(word);
			if (ss >> word)
				throw std::runtime_error("Config file error: line should end after \";\" .\n");
			break;
		}
		//LOCATION
		case 7: {
//			server.pushLocation();
//			Location location;
			server.setLocation(line, str);
//			location.setName("HELLO");
//			server.pushLocation(location);
			break;
		}
		default:
			std::cout << line << "\n";
			break;
	}
}

std::vector <Server> Config::createServers() {
	std::vector<Server> list;
	std::string line;
	std::getline(str, line);

	while (!str.eof())
	{
		int spos = line.find("server");
		if (line.find("server") != std::string::npos)
		{
			while (!str.eof() && line.find('{', spos) == std::string::npos)
			{
				if (!isEmptyLine(line))
				{
					std::cout << line <<"\n";
					throw std::runtime_error("Config file error: wrong format.\n");
				}
				std::getline(str, line);
				spos = 0;
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
			list.push_back(server);
		}
		if (!str.eof())
			std::getline(str, line);
	}
//	close(str);
	return list;
}