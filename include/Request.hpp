#pragma once

#include "utils.hpp"

enum ParseState {
	stateGetHeaderData,
	stateParseRequestHeaders,
	stateCheckBody,
	stateParseMessageBody,
	stateParseChunkHeader,
	requestParseFAIL, //internal error
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

// REQUEST CLASS provides functions to parse the request and save the Request data. 
class Request
{
private:
	ParseState			state_;
	RequestHeadersState	rhstate_;

	std::string			buffer_;
	int					headersLen_;
	size_t				skip_;

	std::string			methodStr_;
	RequestMethod		method_;
	std::string			uri_; //can be either abs URI or abs path. We don't handle '*' or authority
	std::string         path_;
	std::string     	query_; 
	std::string         fragment_;
	std::string			httpVer_;

	std::map<std::string, std::string> headers_;
	
	std::vector<char> 	body_;
	long				contentLen_;

	int					statusCode_; //is 0 if no problem is found
	std::string			errorMsg_;

	const char *extractHeaders(const char *requestBuf, int& MessageLen);
	void		parseRequestHeaders();
	void		parseMethod(std::istringstream& requestLine);
	void		parseHTTPver(std::istringstream& requestLine);
	void		parseHTTPHeader(std::istringstream& headersStream);

	const char *checkForBody(const char *bodyStart, const char *msgEnd, int& MessageLen);
	const char *storeBody(const char *bodyStart, int& MessageLen);
	const char *getChunkSize(const char *start, int& messageLen);
	const char *skipFoundCRLF(const char *start, int& messageLen);
	
	void 		setError(ParseState type, int errorCode, const char *message);
	void		clearRequest();

public:
	Request();
	~Request();
	Request(const Request& rhs);
	Request &operator=(const Request& rhs);

	void		processRequest(const char* requestBuf, int messageLen);
	void		parseURI(std::istringstream& requestLine);
	void		resetRequest();

	const RequestMethod& getMethod() const;
	const std::string&	getMethodStr() const;

	const std::string&	getUri() const;
	const std::string& 	getHttpVer() const;
	const std::string&	getPath() const;
	const std::string&	getQuery() const;
	const std::string&  getFragment() const;
	const int&			getErrorCode() const;
	const std::string&	getErrorMsg() const;
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
