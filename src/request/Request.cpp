#include "Request.hpp"

Request::Request(): state_(stateParseRequestLine), rlstate_(stateParseMethod), method_(OTHER), statusCode_(0) {
}

Request::~Request() {}

Request::Request(const Request& rhs): state_(rhs.state_), rlstate_(rhs.rlstate_), methodStr_(rhs.methodStr_), method_(rhs.method_), \
								uri_(rhs.uri_), path_(rhs.path_), httpVer_(rhs.httpVer_), \
								headers_(rhs.headers_), body_(rhs.body_), statusCode_(rhs.statusCode_), errorMsg_(rhs.errorMsg_) {
}

Request& Request::operator=(const Request& rhs) {
	if (this != &rhs) {
		state_ = rhs.state_;
		rlstate_ = rhs.rlstate_;
		methodStr_ = rhs.methodStr_;
		method_ = rhs.method_;
		uri_ = rhs.uri_;
		path_ = rhs.path_;
		params_ = rhs.params_;
		query_ = rhs.query_;
		fragment_ = rhs.fragment_;
		httpVer_ = rhs.httpVer_;
		headers_ = rhs.headers_;
		body_ = rhs.body_;
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

void Request::clearRequest() {
	methodStr_.clear();
	method_ = OTHER;
	uri_.clear();
	path_.clear();
	query_.clear();
	fragment_.clear();
	httpVer_.clear();
	headers_.clear();
	body_.clear();
	//state_, rlstate_, errorCode_ and ErrorMsg_ stay.
}

std::string& Request::trimString(std::string& str) {
	size_t start = str.find_first_not_of(" \t\r\n");
	if (start != std::string::npos)
		str.erase(0, start);
	else {
		// if the string is all whitespaces, clear it(just in case)
		str.clear();
		return str;
	}
	size_t end = str.find_last_not_of(" \t\r\n");
	if (end != std::string::npos)
		str.erase(end + 1);
	return str;
}

bool Request::containsControlChar(std::string& str) const {
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
		if (std::iscntrl(static_cast<unsigned char>(*it))) {
			return true;
		}
	}
	return false;
}

// Extracts from buffer request-line + headers  as a stringstream.
// Returns a pointer to the start of the body or NULL in case empty line is not found.
const char *Request::extractHeadersStream(std::stringstream& headersStream, const char *requestBuf, const char *msgEnd) {

	while (requestBuf != msgEnd && std::isspace(*requestBuf))
		requestBuf++;
	if (requestBuf == msgEnd)
		return NULL;
	const char* headersEnd = std::strstr(requestBuf, "\r\n\r\n"); // can we guarantee that it's \0 terminated?
	if (!headersEnd)
		return NULL;
	std::size_t len = static_cast<std::size_t>(headersEnd - requestBuf);
	char tmp[len + 1];
	std::memcpy(tmp, requestBuf, len);
	tmp[len] = '\0';
	const char *bodyStart = headersEnd + 4; //moving pointer past CRLFCRLF; if now (bodyStart == msgEnd) -> no body
	headersStream << tmp;
	return bodyStart;
}

/* ************************************************************************** */
/*                                  PARSERS                                   */
/* ************************************************************************** */
void	Request::parseMethod(std::stringstream& requestLine) {
	requestLine >> methodStr_;
	if (requestLine.fail() || methodStr_.empty())
		return setError(requestERROR, 400, "Faulty method");
	const std::string methods[3] = {"GET", "POST", "DELETE"};
	for (int i = 0; i < 3; i++) {
		if (methods[i] == methodStr_) {
			method_ = static_cast<RequestMethod>(i);
			break ;
		}
	}
	if (method_ == OTHER)
		return setError(requestERROR, 501, "");
	rlstate_ = stateParseUri;
	return ;
}

void	Request::parseURI(std::stringstream& requestLine) {
	std::string tmp;
	requestLine >> tmp;
	if (requestLine.fail() || tmp.empty() || containsControlChar(tmp))
		return setError(requestERROR, 400, "");
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
			return setError(requestERROR, 400, "");
		componentStart = tmp.find('/', 8);
		if (componentStart != std::string::npos)
			tmp = tmp.substr(componentStart);
	}
	if (tmp.at(0) == '/')
		path_ = tmp;
	else
		return setError(requestERROR, 400, "");
	rlstate_ = stateParseHTTPver;
	return ;
}

void	Request::parseHTTPver(std::stringstream& requestLine) {
	requestLine >> httpVer_;
	if (requestLine.fail() || httpVer_.empty() || !requestLine.eof() || httpVer_.compare(0, 5, "HTTP/") != 0)
		return setError(requestERROR, 400, "");
	std::stringstream verStr(httpVer_.substr(5));
	int	major, minor;
	char dot;
	verStr >> major >> dot >> minor;
	if (verStr.fail() || dot != '.' || verStr.peek() != EOF)
		return setError(requestERROR, 400, "Bad Request Version");
	if (major != 1 || minor != 1)
		return setError(requestERROR, 505, "");
	rlstate_ = requestLineOK;
	return ;
}

void	Request::parseRequestLine(std::stringstream& headersStream) {
	std::string rl;
	std::getline(headersStream, rl);
	if (!headersStream || rl.empty())
		return setError(requestParseFAIL, 400, ""); //???
	else if (rl.back() != '\r')
		return setError(requestERROR, 400, "");
	rl.pop_back();
	std::stringstream requestLineStream(rl);
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
	if (state_ != requestERROR && state_ != requestParseFAIL)
		state_ = stateParseHeaders;
	return ;
}

void Request::parseHeader(std::stringstream& headersStream) {
	std::string line;
	std::string key, value;

	if ((!headersStream.eof() && (headersStream.peek() == 32 || headersStream.peek() == 10)))
		return setError(requestERROR, 400, "");
	if (std::getline(headersStream, line)) {
		std::istringstream iss(line);
		if (!std::getline(iss, key, ':') || std::isspace(key.back()) || !std::getline(iss, value, '\r'))
			return setError(requestERROR, 400, "");
		trimString(key);
		trimString(value);
	}
	while ((headersStream && (headersStream.peek() == 32 || headersStream.peek() == 10))) {
		std::string moreValue;
		std::getline(headersStream, line);
		std::stringstream more(line);
		if ( !std::getline(more, moreValue, '\r'))
			return setError(requestERROR, 400, "");
		value.append(moreValue);
		trimString(value);
	}
	headers_.insert(std::make_pair(key, value));
	if (headersStream.eof())
		state_ = stateCheckBody;
}

void	Request::checkForBody(const char *bodyStart, const char *msgEnd) {
	if (method_ == GET || method_ == DELETE) {
		if (bodyStart == msgEnd) {
			state_ = requestOK;
			return ;
		}
		else
			return setError(requestERROR, 400, "Request method contains a message body"); //maybe not an error?
	}
	else if (isTransferEncodingChunked())
		state_ = stateParseChunkedBody;
	else
		state_ = stateParseMessageBody;
}

void	Request::storeBody(const char *bodyStart, const char *msgEnd) {

	std::string bodyLenStr = getHeaderValueForKey("Content-Length");
	if (bodyLenStr.empty())
		return setError(requestERROR, 411, "");
	char *endptr;
	long contentLen = strtol(bodyLenStr.c_str(), &endptr, 10);
	if (contentLen != msgEnd - bodyStart - 3) { //-2 when I assume it is not 0 terminated
		// std::cout << bodyLenStr << "   " << contentLen << " **** " << (msgEnd - bodyStart - 3) << std::endl;
		return setError(requestERROR, 400, "Mismatch between actual and declared Content-Length");
	}
	while (bodyStart != msgEnd && contentLen > 0) {
		body_.push_back(*bodyStart);
		bodyStart++;
		contentLen--;
	}
	state_ = requestOK;
}

const char *Request::decodeChunked(const char *chunkStart, const char *msgEnd) {
	unsigned long len;
	const char *content = std::strstr(chunkStart, "\r\n");
	std::string lenLine(chunkStart, content);
	content = content + 2;
	std::stringstream ss(lenLine);
    ss >> std::hex >> len;
	if (ss.bad()) {
		setError(requestERROR, 400, "");
		return content;
	}
	if (len == 0) {
		state_ = requestOK;
		return content;
	}
	while (content != msgEnd && len > 0) {
		body_.push_back(*content);
		content++;
		len--;
	}
	if (content == msgEnd && len != 0) {
		setError(requestERROR, 400, "");
		return content;
	}
	content = content + 2;
	return content;
}

void	Request::processRequest(const char* requestBuf, int messageLen) { //what if len == 0?
	std::stringstream headersStream;
	const char *msgEnd = &requestBuf[messageLen - 1];
	const char *bodyStart = extractHeadersStream(headersStream, requestBuf, msgEnd);
	if (!bodyStart)
		return setError(requestERROR, 400, "Syntax error : no empty line"); 	//there is no CRLFCRLF -> bad Request (syntax error)
	while (state_ != requestERROR && state_ != requestParseFAIL && state_ != requestOK) {
		switch (state_) {
			case stateParseRequestLine: parseRequestLine(headersStream);
				break;
			case stateParseHeaders: parseHeader(headersStream);
				break;
			case stateCheckBody: checkForBody(bodyStart, msgEnd);
				break;
			case stateParseMessageBody:storeBody(bodyStart, msgEnd);
				break;
			case stateParseChunkedBody: bodyStart = decodeChunked(bodyStart, msgEnd);
				break;
			case requestParseFAIL:
			case requestERROR:
			case requestOK:
				break;
		}
	}
}

/* ************************************************************************** */
/*                                  GETTERS                                   */
/* ************************************************************************** */
const RequestMethod&	Request::getMethod() const {
	return method_;
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

std::map<std::string, std::string>::const_iterator	Request::getHeadersBegin() const {
	return headers_.begin();
}

std::map<std::string, std::string>::const_iterator	Request::getHeadersEnd() const {
	return headers_.end();
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

std::vector<char>::const_iterator	Request::getBodyBegin() const {
	return body_.begin();
}

std::vector<char>::const_iterator	Request::getBodyEnd() const {
	return body_.end();
}

/* ************************************************************************** */
/*                                  CHECKERS                                  */
/* ************************************************************************** */
bool	Request::isTransferEncodingChunked() const {
	std::map<std::string, std::string>::const_iterator it;
	it = headers_.find("Transfer-Encoding");
	if (it != headers_.end() && it->second == "chunked")
		return true;
	return false;
}

bool	Request::isConnectionClose() const {
	std::map<std::string, std::string>::const_iterator it;
	it = headers_.find("Connection");
	if (it != headers_.end() && it->second == "close")
		return true;
	return false;
}

/* ************************************************************************** */
/*                                  DEBUG PRINT                               */
/* ************************************************************************** */
std::ostream& operator<<(std::ostream& out, const Request& rhs) { 
	std::cout << "------REQUEST LINE---------------" << std::endl;
	std::cout << "Method enum: " << rhs.getMethod() << std::endl;
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
