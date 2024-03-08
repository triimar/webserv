#pragma once

#include "utils.hpp"

// REQUEST CLASS provides functions to parse the request and save the Request data.
// STATUS CODE (statusCode_) gets modified when a problem is found.
// If no problem is found in parsing statusCode_ it will stay 0.
// PARSE-STATE: requestError indicates a problem with the recieved request, 
// requestParseFail indicate an internal error.

enum ParseState {
	stateGetHeaderData,
	stateParseRequestHeaders,
	stateCheckBody,
	stateParseMessageBody,
	stateParseChunkHeader,
	requestParseFAIL,
	requestERROR,
	requestOK
};

enum RequestHeadersState {
	stateParseMethod,
	stateParseUri,
	stateParseHTTPver,
	stateParseHTTPHeaders,
	requestHeadersOK
};

class Request
{
private:
	unsigned long		maxBodySize_;
	
	ParseState			state_;
	RequestHeadersState	rhstate_;

	std::string			buffer_;
	int					headersLen_;
	size_t				skip_;

	std::string			methodStr_;
	RequestMethod		method_;
	std::string			uri_; 
	std::string         path_;
	std::string     	query_; 
	std::string         fragment_;
	std::string			httpVer_;
	std::map<std::string, std::string> headers_;
	
	std::vector<char> 	body_;
	unsigned long		contentLen_;

	int					statusCode_;

	Request();

	
	const char *extractHeaders(const char *requestBuf, int& MessageLen);
	void		parseRequestHeaders();
	void		parseMethod(std::istringstream& requestLine);
	void		parseHTTPver(std::istringstream& requestLine);
	void		parseHTTPHeader(std::istringstream& headersStream);

	const char *checkForBody(const char *bodyStart, int& MessageLen);
	const char *storeBody(const char *bodyStart, int& MessageLen);
	const char *getChunkSize(const char *start, int& messageLen);
	const char *skipFoundCRLF(const char *start, int& messageLen);
	
	void 		setError(ParseState type, int errorCode);
	void		clearRequest();

public:
	Request(unsigned long maxBodySize);
	~Request();
	Request(const Request& rhs);
	Request &operator=(const Request& rhs);

	void		processRequest(const char* requestBuf, int messageLen);
	void		resetRequest();
	
	void		parseURI(std::istringstream& requestLine);

	const RequestMethod& getMethod() const;
	const std::string&	getMethodStr() const;
	const std::string&	getUri() const;
	const std::string& 	getHttpVer() const;
	const std::string&	getPath() const;
	const std::string&	getQuery() const;
	const std::string&  getFragment() const;
	const int&			getErrorCode() const;
	std::string			getHeaderValueForKey(const std::string& key) const;
    const std::vector<char>& getBody() const;

	std::map<std::string, std::string>::const_iterator	getHeadersBegin() const;
	std::map<std::string, std::string>::const_iterator	getHeadersEnd() const;
	std::vector<char>::const_iterator					getBodyBegin() const;
	std::vector<char>::const_iterator					getBodyEnd() const;

	bool				isTransferEncodingChunked() const;
	bool 				isKeepAlive() const;
	bool				requestComplete() const;
};

std::ostream& operator<<(std::ostream& out, const Request& rhs);
