#pragma once

#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <cctype>

#define CRLF "\r\n"

enum ParseState {
	stateParseRequestLine,
	stateParseHeaders,
    StateParseMessageBody,
	requestParseFAIL, //indicates falure of parser as opposed to problem with request
	requestERROR, // indicates a problem in the request
	requestOK,
};

enum RequestLineState {
	stateParseMethod,
	stateParseUri,
	stateParseHTTPver,
	requestLineOK,
};

enum RequestMethod {GET, POST, DELETE, OTHER};

// REQUEST CLASS provides functions to parse the request and save the Request data.
class Request 
{
private:
	ParseState			state_;
	RequestLineState	rlstate_;

	std::string			methodStr_;
	RequestMethod		method_;
	std::string			uri_;
	std::string			httpVer_;
	// std::map<std::string, std::string> headers_; //map does not allow access using index. To iterate through map I have to make the variable public
	std::map<std::string, std::string> headers_;
	std::vector<char> 	body_;
	// Not std::string body_, because HTTP request can contain binary data. 
	// In the context of HTTP, "binary data" typically refers to non-textual data, 
	// which may include images, audio files, video files, or any other type of file that is not represented as plain text.

	int					errorCode_; //is 0 if no error is found in 
	std::string			errorMsg_;
	
	Request(const Request& rhs);
	Request &operator=(const Request& rhs);

	void		parseMethod(std::stringstream& requestLine);
	void		parseURI(std::stringstream& requestLine);
	void		parseHTTPver(std::stringstream& requestLine);

	const char *extractHeadersStream(std::stringstream& headersStream, const char *requestBuf, const char *msgEnd);
	
	void		parseRequestLine(std::stringstream& headersStream);
	void		parseHeader(std::stringstream& headersStream);

	void 		setError(ParseState type, int errorCode, const char *message);
	std::string& trimString(std::string& str);
	bool		containsDelmiter(std::string& str);
	bool 		containsControlChar(std::string& str) const;

public:
	Request();
	~Request();

	void				processRequest(const char* requestBuf, int messageLen);
	void				clearRequest();

	RequestMethod		getMethod() const;
	const std::string	getUri() const;
	const std::string 	getHttpVer()const;
	bool				isTransferEncodingChunked() const;
	bool 				isConnectionClose() const;
	std::map<std::string, std::string>::const_iterator	getHeadersBegin() const;
	std::map<std::string, std::string>::const_iterator	getHeadersEnd() const;
	const std::string&	getHeaderValueForKey(const std::string& key) const;

};
