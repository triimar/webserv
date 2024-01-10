#pragma once

#include <map>
#include <string>
#include <sstream>
#include <vector>

enum RequestMethod { //same enum in Location
	GET,
	POST,
	DELETE,
	ALL,
};

enum RequestState {
	requestOK,
	reqestParseFAIL,
	requestERROR,
};

//parses the request into needed data, will set stare to requestParseFAIL in case of invalid file & 
//requesrERROR in case of some other error (*not sure if this is necessary);
class Request 
{
private:
	const int		socketFd_;
	RequestState	state_;

	RequestMethod	method_;
	std::string		url_;
	std::string		httpVer_;

	std::vector<std::pair<std::string, std::string>> headers_;
	std::vector<char> body_;
	//std::istringstream	body_; 
	// or std::vector<char> body_; HAVE to decide, which would be better
	// Not std::string, because HTTP request can contain binary data. 
	// In the context of HTTP, "binary data" typically refers to non-textual data, 
	// which may include images, audio files, video files, or any other type of file that is not represented as plain text.

	Request();
	Request(const Request& rhs);
	Request &operator=(const Request& rhs);


public:
	Request(int	socketFd);
	~Request();

	RequestMethod	getMethod() const;
	std::string		getUrl() const;
	std::string		getHttpVer()const;
	std::string		getHeaderKey(int index) const;
	std::string		getHeaderValue(int index) const;

};
