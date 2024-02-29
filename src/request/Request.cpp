#include "../../include/webserv.hpp"

Request::Request(): state_(stateGetHeaderData), rlstate_(stateParseMethod), headersStream_(""), \
					headersLen_(0), skip_(4), method_(OTHER), contentLen_(0), statusCode_(0) {
}

Request::~Request() {}

Request::Request(const Request& rhs): state_(rhs.state_), rlstate_(rhs.rlstate_), \
								buffer_(rhs.buffer_), headersLen_(rhs.headersLen_), skip_(rhs.skip_), \
								methodStr_(rhs.methodStr_), method_(rhs.method_), \
								uri_(rhs.uri_), path_(rhs.path_), httpVer_(rhs.httpVer_), \
								headers_(rhs.headers_), body_(rhs.body_), contentLen_(rhs.contentLen_), \
								statusCode_(rhs.statusCode_), errorMsg_(rhs.errorMsg_) {
	if (!rhs.headersStream_.str().empty())
		headersStream_ << rhs.headersStream_.str();
}

Request& Request::operator=(const Request& rhs) {
	if (this != &rhs) {
		state_ = rhs.state_;
		rlstate_ = rhs.rlstate_;
		buffer_ = rhs.buffer_;
		headersStream_.str("");
		headersStream_.clear();
		if (!rhs.headersStream_.str().empty())
			headersStream_ << rhs.headersStream_.str();
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
//	clearRequest();
}

// Clears data in case of invalid request, state_, rlstate_, statusCode_ and errorMsg_ are not cleared
void Request::clearRequest() {
	buffer_.clear();
	buffer_ = "";
	headersStream_.str("");
	headersStream_.clear();
	skip_ = 4;
	headersLen_ = 0;
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

const char *Request::extractHeadersStream(const char *requestBuf, int messageLen) {
	const char *msgEnd = &requestBuf[messageLen - 1];
	buffer_.append(requestBuf, messageLen);
	size_t foundPos = buffer_.find(CRLFCRLF);
	if (foundPos == std::string::npos) {
		headersLen_ += messageLen;
		if (headersLen_ > MAX_HEADER_SIZE)
			setError(requestERROR, 413, "Content too long");
		return msgEnd;
	}
	if (headersLen_ + foundPos  > MAX_HEADER_SIZE)
		setError(requestERROR, 413, "Content too long");
	buffer_.erase(foundPos);	
	headersStream_ << buffer_;
	state_ = stateParseRequestLine;
	int headerChunk = foundPos - headersLen_;
	if (headerChunk < 0) {
		skip_ += headerChunk;
		return (requestBuf);
	}
	headersLen_ += foundPos;
	return (requestBuf + headerChunk);
}

/* ************************************************************************** */
/*                                  PARSERS                                   */
/* ************************************************************************** */
void	Request::parseRequestLine() {
	std::string rl;
	if (!std::getline(headersStream_, rl, '\r'))
		return setError(requestERROR, 500, "Failure to extract request line");
	if (!headersStream_.eof() && headersStream_.get() != '\n') // for final when no crlf
		return setError(requestERROR, 400, "Syntax error in request line");
	std::istringstream requestLineStream(rl);
	while (state_ != requestERROR && state_ != requestParseFAIL && rlstate_ != requestLineOK)  {
		switch (rlstate_)
		{
			case stateParseMethod: parseMethod(requestLineStream);
				break;
			case stateParseUri: parseURI(requestLineStream);
				break;
			case stateParseHTTPver: parseHTTPver(requestLineStream);
				break;
			case requestLineOK:
				break;
		}
	}
	if (rlstate_ == requestLineOK) {
		if (headersStream_.eof() && (method_ == GET || method_ == DELETE))
			state_ = requestOK;
		else
			state_ = stateParseHeaders;
	}
	return ;
}

void	Request::parseMethod(std::istringstream& requestLine) {
	requestLine >> methodStr_;
	if (requestLine.fail() || methodStr_.empty())
		return setError(requestParseFAIL, 500, "Failure to extract method from request line");
	const std::string methods[3] = {"GET", "POST", "DELETE"};
	for (int i = 0; i < 3; i++) {
		if (methods[i] == methodStr_) {
			method_ = static_cast<RequestMethod>(i);
			break ;
		}
	}
	if (method_ == OTHER)
		return setError(requestERROR, 501, "Method not implemented");
	rlstate_ = stateParseUri;
	return ;
}

//has to be public for the CGI
void	Request::parseURI(std::istringstream& requestLine) {
	if (requestLine.eof())
		return setError(requestERROR, 400, "Invalid version syntax");
	std::string tmp;
	requestLine >> tmp;
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
		rlstate_ = stateParseHTTPver;
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
	rlstate_ = requestLineOK;
	return ;
}

void Request::parseHeader() {
	if ((!headersStream_.eof() && (headersStream_.peek() == ' ' || headersStream_.peek() == '\t')))
		return setError(requestERROR, 400, "Syntax error: obsolete line folding in headers");
	std::string line;
	std::string key, value;
	if (!std::getline(headersStream_, line, '\r'))
		return setError(requestERROR, 500, "Failure to extract line from headers");
	if (!headersStream_.eof() && headersStream_.get() != '\n') // for final when no crlf
		return setError(requestERROR, 400, "Syntax error in Headers");
	std::istringstream iss(line);
	if (!std::getline(iss, key, ':') || std::isspace(key.at(key.size() - 1)) || !std::getline(iss, value) \
		|| containsControlChar(key) || containsControlChar(value))
		return setError(requestERROR, 400, "Syntax error in Headers 2");
	trimString(key);
	strToLower(key);
	trimString(value);
	if (!headersStream_)
		return setError(requestERROR, 500, "Failure extracting header data");
	std::map<std::string, std::string>::iterator found = headers_.find(key);
	if (found == headers_.end())
		headers_.insert(std::make_pair(key, value));
	else
		found->second.append(", " + value);
	if (headersStream_.eof())
		state_ = stateCheckBody;
	return ;
}

const char	*Request::checkForBody(const char *start, const char *msgEnd) {
	for (size_t i = 0; i < skip_ && start != msgEnd; ++i) {
		start++;
	}
	if (method_ == GET || method_ == DELETE) {
		state_ = requestOK;
		return msgEnd;
	}
	if (isTransferEncodingChunked())
		state_ = stateParseChunkedBody;
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

const char *Request::storeBody(const char *bodyStart, const char *msgEnd) {
	if (bodyStart == msgEnd)
			return msgEnd ;
	while (contentLen_ > 0) {
		body_.push_back(*bodyStart);
		contentLen_--;
		if (bodyStart == msgEnd)
			break ;
		bodyStart++;
	}
	if (contentLen_ == 0 && !isTransferEncodingChunked())
		state_ = requestOK;
	return bodyStart;
}

const char *Request::skipCRLF(const char *start, const char *msgEnd) {
	start += 1;
	if (start != msgEnd)
		start += 1;
	return start;
}

const char *Request::decodeChunked(const char *start, const char *msgEnd) {
	if (contentLen_ == 0 && state_ == stateParseChunkedBody)
	{
		char lineEnd[] = CRLF;
		const char* found = std::search(start, msgEnd, lineEnd, lineEnd + 1);
		if (found == msgEnd) {
			setError(requestERROR, 400, "Syntax error in chunk");
			return msgEnd;
		}
		std::string lenLine(start, found);
		std::istringstream iss(lenLine);
		iss >> std::hex >> contentLen_;
		if (!iss || !iss.eof()) {
			if (!iss.eof())
				setError(requestERROR, 400, "Syntax error in chunk");
			else
				setError(requestParseFAIL, 500, "std::stringstream failure in body");
			return msgEnd;
		}
		start = found;
		if (contentLen_ == 0) {
			state_ = requestOK;
			return start;
		}
		start = skipCRLF(start, msgEnd);
	}
	if ((start = storeBody(start, msgEnd)) == msgEnd)
		return msgEnd;
	if (*start != '\r' && *(start + 1) != '\n') {
		setError(requestERROR, 400, "Syntax error in chunk");
		return msgEnd;
	}
	start = skipCRLF(start, msgEnd);
	return start;
}

void	Request::processRequest(const char* requestBuf, int messageLen) {
	if (messageLen == 1)
		setError(requestERROR, 400, "Request length 1 not accepted");
	const char *msgEnd = &requestBuf[messageLen - 1];
	const char *start = requestBuf;
	while (start != msgEnd && state_ != requestERROR && state_ != requestParseFAIL && state_ != requestOK) {
		switch (state_) {
			case stateGetHeaderData: start = extractHeadersStream(requestBuf, messageLen);
				break;
			case stateParseRequestLine: parseRequestLine();
				break;
			case stateParseHeaders: parseHeader();
				break;
			case stateCheckBody: start = checkForBody(start, msgEnd);
				break;
			case stateParseMessageBody: start = storeBody(start, msgEnd);
				break;
			case stateParseChunkedBody: start = decodeChunked(start, msgEnd);
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
	rlstate_ = stateParseMethod;
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
