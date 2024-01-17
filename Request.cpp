#include "Request.hpp"

//Orthodox canonical form
Request::Request(): state_(stateParseRequestLine), rlstate_(stateParseMethod), method_(OTHER), errorCode_(0) {
}

Request::~Request() {

}

Request::Request(const Request& rhs): state_(rhs.state_), rlstate_(rhs.rlstate_), methodStr_(rhs.methodStr_), method_(rhs.method_), \
								uri_(rhs.uri_), httpVer_(rhs.httpVer_), \
								headers_(rhs.headers_), body_(rhs.body_), errorCode_(rhs.errorCode_) {
}

Request& Request::operator=(const Request& ) {
	return *this;
}

void Request::setError(ParseState type, int errorCode, const char *message) {
	state_ = type;
	errorCode_ = errorCode;
	errorMsg_.assign(message);
	//should it clear all?
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

//deimiters in char representation are: DQUOTE and "(),/:;<=>?@[\]{}"
bool Request::containsDelimiter(std::string& str) {
	"(),/:;<=>?@[\]{}"
	for (std::string::iterator it = str.begin(); it != str.end(); ++it) {
		if ()
	}
	"(),/:;<=>?@[\]{}"
}
/* parsing functions and setters */

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
	const char *bodyStart = headersEnd + 4; //moving pointer past CRLFCRLF; if now (start == msgEnd) -> no body
	headersStream << tmp;
	return bodyStart;
}

void	Request::parseMethod(std::stringstream& requestLine) {
	requestLine >> methodStr_;
	if (requestLine.fail() || methodStr_.empty())
		return setError(requestERROR, 400, "Bad request");
	const std::string methods[3] = {"GET", "POST", "DELETE"};
	for (int i = 0; i < 3; i++) {
		if (methods[i] == methodStr_) {
			method_ = static_cast<RequestMethod>(i);
			break ;
		}
	}
	rlstate_ = stateParseUri;
	return ;
}

void	Request::parseURI(std::stringstream& requestLine) {
	requestLine >> uri_;
	if (requestLine.fail() || uri_.empty())
		return setError(requestERROR, 400, "Bad request");
	rlstate_ = stateParseHTTPver;
	return ;
}

void	Request::parseHTTPver(std::stringstream& requestLine) {
	requestLine >> httpVer_;
	if (requestLine.fail() || httpVer_.empty() || !requestLine.eof() || httpVer_.compare(0, 5, "HTTP/") != 0)
		return setError(requestERROR, 400, "Bad request");
	std::stringstream verStr(httpVer_.substr(5));
	int	major, minor;
    char dot;
    verStr >> major >> dot >> minor;
    if (verStr.fail() || dot != '.' || verStr.peek() != EOF)
		return setError(requestERROR, 400, "Bad request version");
    if (major != 1 || minor != 1)
		return setError(requestERROR, 505, "HTTP version not supported");
	rlstate_ = requestLineOK;
	return ;
}

void	Request::parseRequestLine(std::stringstream& headersStream) {
	std::string rl;
	std::getline(headersStream, rl);
	if (!headersStream || rl.empty())
		return setError(requestParseFAIL, 400, "Bad request"); //???
	else if (rl.back() != '\r')
		return setError(requestERROR, 400, "Bad request");
	rl.pop_back();
	std::stringstream requestLineStream(rl);
	std::cout << "Request Line|" << rl << "|" << std::endl;
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
		return setError(requestERROR, 400, "Bad request");
	if (std::getline(headersStream, line)) {
		std::istringstream iss(line);
		if (!std::getline(iss, key, ':') || std::isspace(key.back()) || !std::getline(iss, value, '\r'))
			return setError(requestERROR, 400, "Bad request");
		trimString(key);
		trimString(value);
	}
	while ((headersStream && (headersStream.peek() == 32 || headersStream.peek() == 10))) {
		std::string moreValue;
		std::getline(headersStream, line);
		std::stringstream more(line);
		if ( !std::getline(more, moreValue, '\r'))
			return setError(requestERROR, 400, "Bad request");
		value.append(moreValue);
		trimString(value);
	}
	headers_.insert(std::make_pair(key, value));
	if (headersStream.eof())
		state_ = requestOK;
}

void	Request::processRequest(const char* requestBuf, int messageLen) { //what if len == 0?
	std::stringstream headersStream;
	const char *msgEnd = &requestBuf[messageLen - 1];
	const char *bodyStart = extractHeadersStream(headersStream, requestBuf, msgEnd);

	if (!bodyStart)
		return setError(requestERROR, 400, "Bad request"); 	//there is no CRLFCRLF -> bad request (syntax error)
	// if (bodyStart == msgEnd)
	// 	std::cout << "NOBODY" << std::endl;
	while (state_ != requestERROR && state_ != requestParseFAIL && state_ != requestOK) {
		switch (state_) {
		case stateParseRequestLine: parseRequestLine(headersStream);
			break;
		case stateParseHeaders: parseHeader(headersStream);
			break;
		case StateParseMessageBody:
			break;
		case requestParseFAIL:
			break;
		case requestERROR:
			break;
		case requestOK:
			break;
		}
	}
	std::cout << "MethodStr & enum|" << methodStr_ << "|" << method_ << "|\n";
	std::cout << "uri|" << uri_ << "|\n";
	std::cout << "http|" << httpVer_ << "|\n";
	std::cout << " ------HEADERS-------" << std::endl;
	for (std::map<std::string, std::string>::iterator it = headers_.begin(); it != headers_.end(); ++it) {
		std::cout << "|" << it->first << "|: |" << it->second << "|" << std::endl;
	}
	std::cout << " -------------" << std::endl;
	std::cout << "ERROR: " << errorCode_ << " : " << errorMsg_ << std::endl;

}

/* getters */
RequestMethod	Request::getMethod() const {
	return method_;
}
std::string		Request::getUri() const {
	return uri_;
}

std::string		Request::getHttpVer()const {
	return httpVer_;
}

// std::map<std::string, std::string>::const_iterator	Request::getHeadersBegin() const {
// 	return headers_.begin();
// }

// std::map<std::string, std::string>::const_iterator	Request::getHeadersEnd() const {
// 	return headers_.end();
// }

// std::string&	Request::getHeaderValueForKey(const std::string& key) const {
// 	return headers_[key];
// }
