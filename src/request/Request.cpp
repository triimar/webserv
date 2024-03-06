#include "../../include/webserv.hpp"

Request::Request(): state_(stateGetHeaderData), rhstate_(stateParseMethod), \
					headersLen_(0), skip_(4), method_(OTHER), contentLen_(0), statusCode_(0) {
}

Request::~Request() {}

Request::Request(const Request& rhs): state_(rhs.state_), rhstate_(rhs.rhstate_), \
								buffer_(rhs.buffer_), headersLen_(rhs.headersLen_), skip_(rhs.skip_), \
								methodStr_(rhs.methodStr_), method_(rhs.method_), \
								uri_(rhs.uri_), path_(rhs.path_), httpVer_(rhs.httpVer_), \
								headers_(rhs.headers_), body_(rhs.body_), contentLen_(rhs.contentLen_), \
								statusCode_(rhs.statusCode_), errorMsg_(rhs.errorMsg_) {
}
Request& Request::operator=(const Request& rhs) {
	if (this != &rhs) {
		state_ = rhs.state_;
		rhstate_ = rhs.rhstate_;
		buffer_ = rhs.buffer_;
		headersLen_ = rhs.headersLen_;
		skip_ = rhs.skip_;
		methodStr_ = rhs.methodStr_;
		method_ = rhs.method_;
		uri_ = rhs.uri_;
		path_ = rhs.path_;
		query_ = rhs.query_;
		fragment_ = rhs.fragment_;
		httpVer_ = rhs.httpVer_;
		headers_ = rhs.headers_;
		body_ = rhs.body_;
		contentLen_ = rhs.contentLen_;
		statusCode_ = rhs.statusCode_;
		errorMsg_ = rhs.errorMsg_;
	}
	return *this;
}

/* ************************************************************************** */
/*                                  HELPERS                                   */
/* ************************************************************************** */
void Request::setError(ParseState type, int statusCode, const char *message) {
	state_ = type;
	statusCode_ = statusCode;
	errorMsg_.assign(message);
	// clearRequest();
}

// Clears data in case of invalid request, state_, rhstate_, statusCode_ and errorMsg_ are not cleared
void Request::clearRequest() {
	buffer_.clear();
	buffer_ = "";
	headersLen_ = 0;
	skip_ = 4;
	methodStr_.clear();
	methodStr_ = "";
	method_ = OTHER;
	uri_.clear();
	uri_ = "";
	path_.clear();
	path_ = "";
	query_.clear();
	query_ = "";
	fragment_.clear();
	fragment_ = "";
	httpVer_.clear();
	httpVer_ = "";
	headers_.clear();
	body_.clear();
	contentLen_ = 0;
}

const char *Request::extractHeaders(const char *requestBuf, int &messageLen) {
	buffer_.append(requestBuf, messageLen);
	size_t foundPos = buffer_.find(CRLFCRLF);
	if (foundPos == std::string::npos) {
		headersLen_ += messageLen; 
		messageLen = 0;
		if (headersLen_ > MAX_HEADER_SIZE)
			setError(requestERROR, 413, "Content too long");
		return requestBuf;
	}
	if (headersLen_ + foundPos  > MAX_HEADER_SIZE) {
		setError(requestERROR, 413, "Content too long");
		return requestBuf;
	}
	buffer_.erase(foundPos);	
	state_ = stateParseRequestHeaders;
	int headerChunk = foundPos - headersLen_;
	if (headerChunk < 0) {
		skip_ += headerChunk;
		return (requestBuf);
	}
	headersLen_ += foundPos;
	messageLen -= headerChunk;
	return (requestBuf + headerChunk);
}

/* ************************************************************************** */
/*                                  PARSERS                                   */
/* ************************************************************************** */
void	Request::parseRequestHeaders() {
	std::istringstream 	headersStream(buffer_);
	buffer_.clear();
	buffer_ = "";
	std::string rl;
	if (!std::getline(headersStream, rl, '\r'))
		return setError(requestERROR, 500, "Failure to extract request line");
	if (!headersStream.eof() && headersStream.get() != '\n')
		return setError(requestERROR, 400, "Syntax error in request line");
	std::istringstream requestLineStream(rl);
	while (state_ != requestERROR && state_ != requestParseFAIL && rhstate_ != requestHeadersOK)  {
		switch (rhstate_)
		{
			case stateParseMethod: parseMethod(requestLineStream);
				break;
			case stateParseUri: parseURI(requestLineStream);
				break;
			case stateParseHTTPver: parseHTTPver(requestLineStream);
				if (rhstate_ == stateParseHTTPHeaders && headersStream.eof() && (method_ != POST)) {
					rhstate_ = requestHeadersOK;
					state_ = requestOK;
				}
				break;
			case stateParseHTTPHeaders: parseHTTPHeader(headersStream);
				break;
			case requestHeadersOK:
				break;
		}
	}
	
	return ;
}

void	Request::parseMethod(std::istringstream& requestLine) {
	requestLine >> methodStr_;
	if (requestLine.fail() || methodStr_.empty())
		return setError(requestParseFAIL, 500, "Failure to extract method from request line");
	const std::string methods[4] = {"GET", "POST", "DELETE", "HEAD"};
	for (int i = 0; i < 4; i++) {
		if (methods[i] == methodStr_) {
			method_ = static_cast<RequestMethod>(i);
			break ;
		}
	}
	if (method_ == OTHER)
		return setError(requestERROR, 501, "Method not implemented");
	rhstate_ = stateParseUri;
	return ;
}

void	Request::parseURI(std::istringstream& requestLine) {
	if (requestLine.eof() || requestLine.get() != ' ') ////
		return setError(requestERROR, 400, "Invalid request line syntax");
	std::string tmp;
	std::getline(requestLine, tmp, ' '); /////
	if (requestLine.fail() || tmp.empty())
		return setError(requestParseFAIL, 500, "Failure to extract URI from request line");
	if (containsControlChar(tmp))
		return setError(requestERROR, 400, "URI contains control character(s)");
	uri_ = tmp;
	size_t componentStart = tmp.find('#');
	if (componentStart != std::string::npos) {
		fragment_ = tmp.substr(componentStart + 1);
		tmp.erase(componentStart);
	}
	componentStart = tmp.find('?');
	if (componentStart != std::string::npos) {
		query_ = tmp.substr(componentStart + 1);
		tmp.erase(componentStart);
	}
	if 	(tmp.compare(0, 7, "http://") == 0) {
		if (!std::isalpha(tmp.at(8)))
			return setError(requestERROR, 400, "Invalid URI");
		componentStart = tmp.find('/', 8);
		if (componentStart != std::string::npos)
			tmp = tmp.substr(componentStart);
	}
	if (tmp.at(0) == '/')
		path_ = tmp;
	else
		return setError(requestERROR, 400, "Invalid path");
	if (state_ != requestOK && state_ != requestERROR && state_ != requestParseFAIL)
		rhstate_ = stateParseHTTPver;
	return ;
}

void	Request::parseHTTPver(std::istringstream& requestLine) {
	if (requestLine.eof())
		return setError(requestERROR, 505, "Invalid version syntax");
	requestLine >> httpVer_;
	if (requestLine.fail() || httpVer_.empty())
		return setError(requestERROR, 500, "Failure to extract HTTP version from request line");
	if (!requestLine.eof() || httpVer_.compare(0, 5, "HTTP/") != 0)
		return setError(requestERROR, 505, "Invalid version syntax");
	std::stringstream verStr(httpVer_.substr(5));
	int	major, minor;
	char dot;
	verStr >> major >> dot >> minor;
	if (verStr.fail())
		return setError(requestERROR, 500, "Failure to extract HTTP version from request line");
	if (dot != '.' || verStr.peek() != EOF)
		return setError(requestERROR, 400, "Bad Request Version");
	if (major != 1 || minor != 1)
		return setError(requestERROR, 505, "Unsupported HTTP version");
	rhstate_ = stateParseHTTPHeaders;
	return ;
}

void Request::parseHTTPHeader(std::istringstream& headersStream) {
	if ((!headersStream.eof() && (headersStream.peek() == ' ' || headersStream.peek() == '\t')))
		return setError(requestERROR, 400, "Syntax error: obsolete line folding in headers");
	std::string line;
	std::string key, value;
	if (!std::getline(headersStream, line, '\r'))
		return setError(requestERROR, 500, "Failure to extract line from headers");
	if (!headersStream.eof() && headersStream.get() != '\n') // for final when no crlf
		return setError(requestERROR, 400, "Syntax error in Headers");
	std::istringstream iss(line);
	if (!std::getline(iss, key, ':') || std::isspace(key.at(key.size() - 1)) || !std::getline(iss, value) \
		|| containsControlChar(key) || containsControlChar(value))
		return setError(requestERROR, 400, "Syntax error in Headers 2");
	trimString(key);
	strToLower(key);
	trimString(value);
	if (!headersStream)
		return setError(requestERROR, 500, "Failure extracting header data");
	std::map<std::string, std::string>::iterator found = headers_.find(key);
	if (found == headers_.end())
		headers_.insert(std::make_pair(key, value));
	else
		found->second.append(", " + value);
	if (headersStream.eof()) {
		state_ = stateCheckBody;
		rhstate_ = requestHeadersOK;
	}
	return ;
}

const char	*Request::checkForBody(const char *start, const char *msgEnd, int &messageLen) {
	for (size_t i = 0; i < skip_ && messageLen > 0 ; ++i) {
		start++;
		messageLen--;
	}
	if (method_ != POST) {
		state_ = requestOK;
		return msgEnd;
	}
	if (isTransferEncodingChunked())
		state_ = stateParseChunkHeader;
	else {
		std::string bodyLenStr = getHeaderValueForKey("content-length");
		if (bodyLenStr.empty()) {
			setError(requestERROR, 411, "Content-length header missing");
			return msgEnd;
		}
		char *endptr;
		contentLen_ = strtol(bodyLenStr.c_str(), &endptr, 10);
		state_ = stateParseMessageBody;
	}	
	return start;
}

const char *Request::storeBody(const char *bodyStart, int& messageLen) {

	while (contentLen_ > 0 && messageLen > 0) {
		body_.push_back(*bodyStart);
		contentLen_--;
		messageLen--;
		bodyStart++;
	}
	if (contentLen_ == 0 && !isTransferEncodingChunked())
		state_ = requestOK;
	else if (contentLen_ == 0) {
		while (skip_ > 0 && messageLen > 0) {
			if ((skip_ == 2 && *bodyStart != '\r') || (skip_ == 1 && *bodyStart != '\n'))
				setError(requestERROR, 400, "Requiest syntac error (CRLF)");
			bodyStart++;
			skip_--;
			messageLen--;
		}
		if (skip_ == 0) {
			skip_ = 2;
			state_ = stateParseChunkHeader;
		}
	}
	return bodyStart;
}

const char *Request::skipFoundCRLF(const char *start, int& messageLen) {
	for (int i = 0; i < 2; i++) {
		messageLen--;
		if (messageLen != 0)
			start++;
	}
	return start;
}

const char *Request::getChunkSize(const char *start, int& messageLen) {
	int	bufSizeBefore = buffer_.size();
	if (!buffer_.empty() && buffer_.at(buffer_.size()- 1) == '\r' && *start == '\n') {
		messageLen--;
		start++;
		buffer_.erase(buffer_.size()- 1);
	}
	else {
		buffer_.append(start, messageLen);
		size_t foundPos = buffer_.find(CRLF);
		if (foundPos == std::string::npos) {
			messageLen -= messageLen;
			if (buffer_.size() > 4)
				setError(requestERROR, 400, "Syntax error in chunk");
			return NULL;
		}
		buffer_.erase(foundPos);
		int move = foundPos - bufSizeBefore;
		messageLen -= move;
		start += move;
		start = skipFoundCRLF(start, messageLen);
	}	
	std::stringstream hexStream(buffer_);
	buffer_.clear();
	buffer_ = "";
	hexStream >> std::hex >> contentLen_;
	if (!hexStream || !hexStream.eof()) {
		if (!hexStream.eof())
			setError(requestERROR, 400, "Syntax error in chunk");
		else
			setError(requestParseFAIL, 500, "std::stringstream failure chunk header");
		return NULL;
	}
	if (contentLen_ == 0)
		state_ = requestOK;
	else {
		state_ = stateParseMessageBody;
		skip_ = 2;
	}
	return start;
}


void	Request::processRequest(const char* requestBuf, int messageLen) {
	const char *msgEnd = &requestBuf[messageLen - 1];
	const char *start = requestBuf;
	while (messageLen > 0 && state_ != requestERROR && state_ != requestParseFAIL && state_ != requestOK) {
		switch (state_) {
			case stateGetHeaderData: start = extractHeaders(requestBuf, messageLen);
				break;
			case stateParseRequestHeaders: parseRequestHeaders();
				break;
			case stateCheckBody: start = checkForBody(start, msgEnd, messageLen);
				break;
			case stateParseMessageBody: start = storeBody(start, messageLen);
				break;
			case stateParseChunkHeader: start = getChunkSize(start, messageLen);
				break;
			case requestParseFAIL:
			case requestERROR:
			case requestOK:
				break;
		}
	}
}

/* ************************************************************************** */
/*                                 RESET FOR NEW PARSING                      */
/* ************************************************************************** */
void	Request::resetRequest() {
	clearRequest();
	state_ = stateGetHeaderData;
	rhstate_ = stateParseMethod;
	statusCode_ = 0;
	errorMsg_.clear();

}

/* ************************************************************************** */
/*                                  GETTERS                                   */
/* ************************************************************************** */
const RequestMethod&	Request::getMethod() const {
	return method_;
}

const std::string&  Request::getMethodStr() const {
    return methodStr_;
}

const std::string&		Request::getUri() const {
	return uri_;
}

const std::string&		Request::getPath() const {
	return path_;
}

const std::string&		Request::getQuery() const {
	return query_;
}

const std::string&  Request::getFragment() const {
	return fragment_;
}

const std::string&		Request::getHttpVer()const {
	return httpVer_;
}

const int&				Request::getErrorCode() const {
	return statusCode_;
}

const std::string&		Request::getErrorMsg() const {
	return errorMsg_;
}


std::string	Request::getHeaderValueForKey(const std::string& key) const {
	std::map<std::string, std::string>::const_iterator it;
	it = headers_.find(key);
	if (it == headers_.end())
		return ("");
	return it->second;
}

const std::vector<char>& Request::getBody() const {
    return body_;
}

std::map<std::string, std::string>::const_iterator	Request::getHeadersBegin() const {
	return headers_.begin();
}

std::map<std::string, std::string>::const_iterator	Request::getHeadersEnd() const {
	return headers_.end();
}

std::vector<char>::const_iterator	Request::getBodyBegin() const {
	return body_.begin();
}

std::vector<char>::const_iterator	Request::getBodyEnd() const {
	return body_.end();
}

/* ************************************************************************** */
/*                                  CHECKERS                                  */
/* ************************************************************************** */

bool	Request::requestComplete() const{
	if (state_ == requestERROR || state_ == requestParseFAIL || state_ == requestOK)
		return true;
	return false;
}

bool	Request::isTransferEncodingChunked() const {
	std::map<std::string, std::string>::const_iterator it;
	it = headers_.find("transfer-encoding");
	if (it != headers_.end() && it->second == "chunked")
		return true;
	return false;
}

bool	Request::isKeepAlive() const {
	std::map<std::string, std::string>::const_iterator it;
	it = headers_.find("connection");
	if (it != headers_.end() && it->second == "close")
		return false;
	return true;
}

/* ************************************************************************** */
/*                                  DEBUG PRINT                               */
/* ************************************************************************** */
std::ostream& operator<<(std::ostream& out, const Request& rhs) { 
	std::cout << "------REQUEST LINE---------------" << std::endl;
	std::cout << "Method: " << rhs.getMethodStr() << std::endl;
	std::cout << "URI: " << rhs.getUri() << std::endl;
	std::cout << "path: " << rhs.getPath() << std::endl;
	std::cout << "query: " << rhs.getQuery() << std::endl;
	std::cout << "fragment: " << rhs.getFragment() << std::endl;
	std::cout << "Http version: " << rhs.getHttpVer() << std::endl;
	std::cout << "------HEADERS(key : value)-------" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = rhs.getHeadersBegin(); it != rhs.getHeadersEnd(); ++it) {
		std::cout << it->first << " : " << it->second << std::endl;
	}
	if (rhs.getBodyBegin() != rhs.getBodyEnd()) {
		std::cout << "------BODY-----------------------" << std::endl;
		for (std::vector<char>::const_iterator it  = rhs.getBodyBegin(); it != rhs.getBodyEnd(); ++it) {
			std::cout << *it;
		}
		std::cout << std::endl;
	}
	std::cout << "------ERROR_STATUS---------------" << std::endl;
	std::cout << "Status code: " << rhs.getErrorCode();
	if (rhs.getErrorMsg() != "")
		std::cout << " : " << rhs.getErrorMsg();
	std::cout << std::endl;
	return out;	
}
