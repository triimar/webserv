#include <iostream>
<<<<<<< HEAD
#include "../include/Server.hpp"
#include "../include/Config.hpp"

int main(int argc, char **argv)
{
	if (argc > 2)
	{
		std::cerr << "Wrong number of arguments\n";
		return 1;
	}
	(void) argv;
	Server server;
	std::string parameter = "0.0.0.0";
	server.setPort(4242);
	server.setHost(parameter.data());
	server.setIP();
	server.startServer();
	server.startListen();

//	try{
//		std::vector<Server> list;
//		Config config(argv[1]);
//		list = config.createServers();
//		std::for_each(list.begin(), list.end(), Server::printServer);
//	}
//	catch (std::exception &e)
//	{
//		std::cerr << e.what();
//	}
=======
#include "Server.hpp"
#include "Request.hpp"

// int main(int argc, char **argv)
// {
// 	if (argc > 2)
// 	{
// 		std::cerr << "Wrong number of arguments\n";
// 		return 1;
// 	}
// 	(void) argv;
// 	Server server;
// 	std::string parameter = "0.0.0.0";
// 	server.setPort(4242);
// 	server.setHost(inet_addr(parameter.data()));
// 	server.setIP();
// 	server.startServer();
// 	server.startListen();
// }

//testing request parsing
int main() 
{
// 	const char* buffer = "\r\n\r\n\r\n\r\nGET http://www.example.com/path/to/resource?query#fragment HTTP/1.1\r\nhost:localhost\r\nkey key: that and that\
//  hallo ballo     \r\n falloyallo  	\r\nwhat: vvaaaalue\r\n\r\n";
	
	// const char* postRequest = "POST http://ajeeojeenothub/oi?queque#frag HTTP/1.1\r\n"
    //                       "Host: example.com\r\n"
    //                       "Content-Type: application/x-www-form-urlencoded\r\n"
    //                       "Content-Length: 15\r\n"
    //                       "\r\n"
    //                       "name=John&age=25\r\n";
	
	const char* postRequest = 
    "POST /example HTTP/1.1\r\n"
    "Host: example.com\r\n"
    "Transfer-Encoding: chunked\r\n"
    "\r\n"
    "1a\r\nThis is the data to be sent in chunks.\r\n"
    "15\r\nAdditional data in chunk 2.\r\n"
    "0\r\n\r\n";

	Request R;
	// R.processRequest(postRequest, strlen(postRequest) + 1);
	R.processRequest(postRequest, strlen(postRequest) + 1);
	std::cout << " ------REQUEST LINE-------" << std::endl;
	std::cout << "Method enum|" << R.getMethod() << "|\n";
	std::cout << "uri|" << R.getUri() << "|\n";
	std::cout << "path|" << R.getPath() << "|\n";
	std::cout << "query|" << R.getQuery() << "|\n";
	std::cout << "fragment|" << R.getFragment() << "|\n";
	std::cout << "http|" << R.getHttpVer() << "|\n";
	std::cout << " ------HEADERS-------" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = R.getHeadersBegin(); it != R.getHeadersEnd(); ++it) {
		std::cout << "|" << it->first << "|: |" << it->second << "|" << std::endl;
	}
	std::cout << " -------------" << std::endl;
	for (std::deque<char>::iterator it  = R.body_.begin(); it != R.body_.end(); ++it) {
		std::cout << *it;
	}
	std::cout << "|" << std::endl << " -------------" << std::endl;
	std::cout << "ERROR: " << R.getErrorCode() << " : " << R.getErrorMsg() << std::endl;
>>>>>>> origin/request_branch
}