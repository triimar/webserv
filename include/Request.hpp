#pragma once

#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <deque>
#include <cctype>
#include <cstdlib>
#include "utils.hpp"

#define CRLF "\r\n"
#define CRLFCRLF "\r\n\r\n"

enum ParseState {
	stateParseRequestLine,
	stateParseHeaders,
	stateCheckBody,
	stateParseMessageBody,
	stateParseChunkedBody,
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

//enum RequestMethod {GET, POST, DELETE, OTHER};

// REQUEST CLASS provides functions to parse the request and save the Request data.
class Request
{
private:
	ParseState			state_;
	RequestLineState	rlstate_;

	std::string			methodStr_;
	RequestMethod		method_;
	std::string			uri_; //can be either abs URI or abs path. I dont handle '*' or authority
	std::string         path_; //is absolute path in relation to the root of the server, extracted from URI
	std::string			params_;
	std::string     	query_;
	std::string         fragment_;
	std::string			httpVer_;

	std::map<std::string, std::string> headers_;


	int					errorCode_; //is 0 if no error is found in
	std::string			errorMsg_;

	Request(const Request& rhs);
	Request &operator=(const Request& rhs);

	void		parseMethod(std::stringstream& requestLine);
	void		parseURI(std::stringstream& requestLine);
	void		parseHTTPver(std::stringstream& requestLine);

	// void 		createHeadersStream(std::stringstream& headersStream, const char *requestBuf, const char *msgEnd);
	const char *extractHeadersStream(std::stringstream& headersStream, const char *requestBuf, const char *msgEnd);
	void		parseRequestLine(std::stringstream& headersStream);
	void		parseHeader(std::stringstream& headersStream);
	// void		checkForBody(char *requestBuf, char *msgEnd);
	void		checkForBody(const char *bodyStart, const char *msgEnd);

	void 		setError(ParseState type, int errorCode, const char *message);
	std::string& trimString(std::string& str);
	bool 		containsControlChar(std::string& str) const;



public:
	Request();
	~Request();

	// void				processHeaders(const char* requestBuf, int messageLen);
	void				processRequest(const char* requestBuf, int messageLen);
	void				storeBody(const char *bodyStart, const char *msgEnd);
	void 				decodeChunked(const char *bodyStart, const char *msgEnd);
	void				clearRequest();

	const RequestMethod& getMethod() const;
	const std::string&	getUri() const;

	const std::string& 	getHttpVer() const;
	const std::string&	getPath() const;
	const std::string&	getQuery() const;
	const std::string&  getFragment() const;
	bool				isTransferEncodingChunked() const;
	bool 				isConnectionClose() const;
	std::map<std::string, std::string>::const_iterator	getHeadersBegin() const;
	std::map<std::string, std::string>::const_iterator	getHeadersEnd() const;
	std::string			getHeaderValueForKey(const std::string& key) const;

	const int&			getErrorCode() const;
	const std::string&	getErrorMsg() const;
	//can be used to get Content-Type. Will return empty string if its not found in headers
	std::deque<char> 	body_; //temporarily in public, will go to private with access functions
	// Not std::string body_, because HTTP request can contain binary data.
	// In the context of HTTP, "binary data" typically refers to non-textual data,
	// which may include images, audio files, video files, or any other type of file that is not represented as plain text.
};