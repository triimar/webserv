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

/* parsing functions and setters */

bool peekCRLF(std::stringstream& ss) {
    // Skip whitespace characters
    while (std::isspace(ss.peek())) {
        ss.get();
    }
    // Check if the next characters are "\r\n"
    return (ss.peek() == '\r' && ss.get() && ss.peek() == '\n');
}

// Extracts from buffer request-line + headers  as a stringstream.
// Returns a pointer to the start of the body or NULL in case empty line is not found.
const char *Request::extractHeadersStream(std::stringstream& headersStream, const char *requestBuf, const char *msgEnd) {

	while (requestBuf != msgEnd && std::isspace(*requestBuf))
		requestBuf++;
	if (requestBuf == msgEnd) {
		std::cout << "Heyaaa";
		return NULL;
	}
	const char* headersEnd = std::strstr(requestBuf, "\r\n\r\n"); // can we guarantee that it's \0 terminated?
	if (!headersEnd) 
		return NULL;
	std::size_t len = static_cast<std::size_t>(headersEnd - requestBuf);
	char tmp[len + 1];
    std::memcpy(tmp, requestBuf, len);
	tmp[len] = '\0';
	const char *bodyStart = headersEnd + 4; //moving pointer past CRLFCRLF; if (start == msgEnd) -> no body
	headersStream << tmp;
	return bodyStart;
}

void	Request::parseMethod(std::stringstream& requestLine) {
	requestLine >> methodStr_;
	if (requestLine.fail() || methodStr_.empty()) {
		state_ = requestERROR;
		errorCode_ = 400;
		return ; 
	}
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
	if (requestLine.fail() || uri_.empty()) {
		state_ = requestERROR;
		errorCode_ = 400;
		return ; 
	}
	rlstate_ = stateParseHTTPver;
	return ;
}

void	Request::parseHTTPver(std::stringstream& requestLine) {
	requestLine >> httpVer_;
	if (requestLine.fail() || httpVer_.empty() || !requestLine.eof() || httpVer_.compare(0, 5, "HTTP/") != 0) {
		state_ = requestERROR;
		errorCode_ = 400;
		return ; 
	}
	std::stringstream verStr(httpVer_.substr(5));
	int	major, minor;
    char dot;
    verStr >> major >> dot >> minor;
    if (verStr.fail() || dot != '.' || verStr.peek() != EOF) {
		state_ = requestERROR;
		errorCode_ = 400;
		return ; 
    }
    if (major != 1 || minor != 1) {
		state_ = requestERROR;
		errorCode_ = 505;
		return ; 
    }
	rlstate_ = requestLineOK;
	return ;
}

void	Request::parseRequestLine(std::stringstream& headersStream) {
	std::string rl;
	std::getline(headersStream, rl);
	if (!headersStream || rl.empty()) {
		state_ = requestParseFAIL;
		return ; 
		}
	else if (rl.back() != '\r') {
		state_ = requestERROR;
		return ; 
		}
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

void Request::parseHeaders(std::stringstream& headersStream) {
			std::string headerLine;
			std::getline(headersStream, headerLine);
			std::cout << "FIRST HEADER LINE: |" << headerLine << "|" << std::endl;
			state_ = requestOK;
}

void	Request::processRequest(const char* requestBuf, int messageLen) { //what if len == 0?
	const char *msgEnd = &requestBuf[messageLen - 1];
	std::stringstream headersStream;
	const char *bodyStart = extractHeadersStream(headersStream, requestBuf, msgEnd);
	if (!bodyStart) {
		state_ = requestERROR;
		errorCode_ = 400;	
		return ; 	//there is no CRLFCRLF bad request (syntax error)
	}
	if (bodyStart == msgEnd)
		std::cout << "NOBODY" << std::endl;
	std::string requestLine;
	while (state_ != requestERROR && state_ != requestParseFAIL && state_ != requestOK) {
		switch (state_) {
		case stateParseRequestLine: parseRequestLine(headersStream);
			break;
		case stateParseHeaders: parseHeaders(headersStream);
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
	std::cout << "ERROR CODE: " << errorCode_ << std::endl;
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

std::string		Request::getHeaderKey(int index) const {
	return headers_[index].first;
}
std::string		Request::getHeaderValue(int index) const {
	return headers_[index].second;
}
