#pragma once

#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

// REQUEST CLASS provides functions to parse the request and save the Request data. Request-Line+Headers should be parsed first. Then decided if there is a body and what size to be read.
enum RequestMethod { //same enum in Location
	GET,
	POST,
	DELETE,
	OTHER,
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
	RequestState		state_;

	RequestMethod		method_;
	std::string			methodStr_;
	std::string			uri_;
	std::string			httpVer_;
	int					errorCode_; //set in constructor
	// unsigned int		versionMinor_; is according to HTTP/1.1, but as rw only support 1.1 then not needed
	// unsigned int		versionMajor_;
	// std::map<std::string, std::string> headers_; //map does not allow access using index. To iterate through map I have to make the variable public
	std::vector<std::pair<std::string, std::string> > headers_;
	std::vector<char> body_;
	//std::istringstream	body_; 
	// or std::vector<char> body_; HAVE to decide, which would be better
	// Not std::string, because HTTP request can contain binary data. 
	// In the context of HTTP, "binary data" typically refers to non-textual data, 
	// which may include images, audio files, video files, or any other type of file that is not represented as plain text.

	Request(const Request& rhs);
	Request &operator=(const Request& rhs);

	std::string&	trimWhitespaces(std::string& str);
	void			setRequestMethod();

public:
	Request();
	~Request();

	

	int				processRequestLine(char* bufRequestLine);
	// int				processHeaders(char* bufHeaders);
	// int				processBody();

	RequestMethod	getMethod() const;
	std::string		getUri() const;
	std::string 	getHttpVer()const;
//	getters for header info about the body?
	std::string		getHeaderKey(int index) const;
	std::string		getHeaderValue(int index) const;
};


// The typical order of processing an HTTP request involves reading the request line first, followed by reading and parsing the headers, and then, based on the headers,
//  making decisions about how to handle the body. The body may be received and processed afterward.

// Here's a common step-by-step approach:

// Read Request Line:
// Read the request line from the socket. The request line contains the HTTP method, the target URI, and the HTTP version.
// Parse the request line to extract the method, URI, and version.

// Read Headers:
// Read headers from the socket until you encounter a blank line ("\r\n\r\n"), indicating the end of headers.
// Parse and process each header individually.
// Process Headers:
// Based on the headers received, make decisions about how to handle the request.
// For example, check the Content-Length header to determine the size of the body, or check the Transfer-Encoding header for chunked transfer encoding.

// Read Body (If Applicable):
// If the request has a body (e.g., for POST or PUT requests), read the body based on the information obtained from the headers.
// Process Body:
// Parse and process the body content according to the determined content type or other relevant information.