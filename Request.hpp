#pragma once

#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#define CRLF "\r\n"

enum ParseState
{
	parseMethod,
	parseURI,
	parseHTTPver,
	parseHeaders,
    parseMessageBody,
	reqestParseFAIL,
	requestERROR,
};

enum RequestMethod {
	GET,
	POST,
	DELETE,
	OTHER,
};

// REQUEST CLASS provides functions to parse the request and save the Request data.
class Request 
{
private:
	ParseState			state_;

	std::string			methodStr_;
	RequestMethod		method_;
	std::string			uri_;
	std::string			httpVer_;
	// std::map<std::string, std::string> headers_; //map does not allow access using index. To iterate through map I have to make the variable public
	std::vector<std::pair<std::string, std::string> > headers_;
	std::vector<char> body_;
	int					errorCode_; //is 0 if no error is found in 
	// Not std::string body_, because HTTP request can contain binary data. 
	// In the context of HTTP, "binary data" typically refers to non-textual data, 
	// which may include images, audio files, video files, or any other type of file that is not represented as plain text.

	Request(const Request& rhs);
	Request &operator=(const Request& rhs);

	void			setRequestMethod();
//	int				parseRequestLine(const char *start, const char *messageEnd);

public:
	Request();
	~Request();

	void			processRequest(const char* requestBuf, int messageLen);

	RequestMethod	getMethod() const;
	std::string		getUri() const;
	std::string 	getHttpVer()const;
	std::string		getHeaderKey(int index) const;
	std::string		getHeaderValue(int index) const;
};
