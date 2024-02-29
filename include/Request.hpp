#pragma once

#include "utils.hpp"

enum ParseState {
	stateGetHeaderData,
	stateParseRequestLine,
	stateParseHeaders,
	stateCheckBody,
	stateParseMessageBody,
	stateParseChunkedBody,
	requestParseFAIL, //internal error
	requestERROR,
	requestOK
};

enum RequestLineState {
	stateParseMethod,
	stateParseUri,
	stateParseHTTPver,
	requestLineOK
};

// REQUEST CLASS provides functions to parse the request and save the Request data. 
class Request
{
private:
	ParseState			state_;
	RequestLineState	rlstate_;

	std::string			buffer_;
	std::stringstream 	headersStream_;
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

	const char *extractHeadersStream(const char *requestBuf, int MessageLen);
	void		parseRequestLine();
	void		parseMethod(std::istringstream& requestLine);
	void		parseHTTPver(std::istringstream& requestLine);

	void		parseHeader();
	const char *checkForBody(const char *bodyStart, const char *msgEnd);
	const char *storeBody(const char *bodyStart, const char *msgEnd);
	const char *decodeChunked(const char *chunkStart, const char *msgEnd);
	const char *skipCRLF(const char *start, const char *msgEnd);
	
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
