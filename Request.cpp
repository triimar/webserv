#include "Request.hpp"

//Orthodox canonical form
Request::Request(): state_(parseMethod), method_(OTHER), errorCode_(0) {
}

Request::~Request() {

}

Request::Request(const Request& rhs): state_(rhs.state_), methodStr_(rhs.methodStr_), method_(rhs.method_), \
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

void	Request::setRequestMethod() {
	
	const std::string methods[3] = {"GET", "POST", "DELETE"};
	for (int i = 0; i < 3; i++) {
		if (methods[i] == methodStr_) {
			method_ = static_cast<RequestMethod>(i);
			break ;
		}
	}
}
void	Request::processRequest(const char* requestBuf, int messageLen) {
	const char *start = requestBuf;
	const char *msgEnd = &requestBuf[messageLen - 1];

	const char* headersEnd = std::strstr(start, "\r\n\r\n");
	if (!headersEnd) {
		state_ = requestERROR;
		errorCode_ = 400;		//bad request (syntax error)
		return;
	}
	std::size_t len = static_cast<std::size_t>(headersEnd - start);
	char tmp[len + 1];
    std::memcpy(tmp, start, len);
	tmp[len] = '\0';
	start = headersEnd + 4; //moving pointer past CRLFCRLF; if (start == msgEnd) -> no body
	std::istringstream headersStream(tmp);
		// 	switch (state_) {
	// 	case parseMethod:setRequestMethod(start)
	// 		break;
	// 	case parseURI:
	// 		break;
	// 	case parseHTTPver:
	// 		break;
	// 	case parseHeaders:
	// 		break;
}

// void	Request::processRequest(const char* requestBuf, int messageLen) {
// 	const char *start = requestBuf;
// 	const char *msgEnd = &requestBuf[messageLen - 1];

// 	const char* headersEnd = std::strstr(start, "\r\n\r\n");
// 	if (!headersEnd) {
// 		state_ = requestERROR;
// 		errorCode_ = 400;		//bad request (syntax error)
// 		return;
// 	}
// 	std::size_t len = static_cast<std::size_t>(headersEnd - start);
// 	char tmp[len + 1];
//     std::memcpy(tmp, start, len);
// 	tmp[len] = '\0';
// 	start = tmp + 4; //moving pointer past CRLFCRLF
// 	std::istringstream headersStream(tmp);
// 	std::cout << "|" << headersStream.str() << "|" << start << "|" << std::endl;

// 	// while (start != msgEnd || state_ != reqestParseFAIL || state_ != requestERROR)
// 	// {
		
// 	// 	switch (state_) {
// 	// 	case parseMethod:setRequestMethod(start)
// 	// 		break;
// 	// 	case parseURI:
// 	// 		break;
// 	// 	case parseHTTPver:
// 	// 		break;
// 	// 	case parseHeaders:
// 	// 		break;
// 	// 	case parseMessageBody:
// 	// 	}
// 	// }
// 	// if (start == msgEnd || !parseRequestLine(start, msgEnd))
// 	// 	return ;
// 	// if (!parseHeaders(start, msgEnd));
// 	// 	return ;
// }

//getters
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
