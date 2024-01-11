#include "Request.hpp"

#define CRLF "\r\n" //end-of-line marker for all protocol elements except the entity-body

/* constructors & destructor */
Request::Request(): requestStream_(), state_(requestERROR) {

}

Request::Request(const char *buffer): requestStream_(buffer), state_(requestOK), versionMinor_(1), \
									versionMajor_(1) {
	std::cout << requestStream_.str();
}

Request::~Request() {

}

Request::Request(const Request& rhs): requestStream_(rhs.requestStream_.str()), state_(rhs.state_), \
									method_(rhs.method_), uri_(rhs.uri_), versionMinor_(rhs.versionMinor_), \
									versionMajor_(rhs.versionMajor_), headers_(rhs.headers_), body_(rhs.body_) {
}

Request& Request::operator=(const Request& ) {
	return *this;
}

/* member functions that set member variables*/

void Request::parseRequestLine() {
	std::string requestLine;
    std::getline(requestStream_, requestLine, '\r');
    requestStream_.ignore();

    std::stringstream rls(requestLine);
	std::string http;
	rls >> methodStr_ >> uri_ >> http;
	std::cout << "|"<< methodStr_ << "|" << uri_ << "|" << http << "|" << std::endl;
	std::cout << requestStream_.str();

}

/* helpers (maybe should be in a separate namespace that the class uses?)*/

std::string& Request::trimWhitespaces(std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start != std::string::npos) {
        str.erase(0, start);
    } else {
        // if the string is all whitespaces, clear it(just in case)
        str.clear();
        return str;
    }
    size_t end = str.find_last_not_of(" \t\r\n");
    if (end != std::string::npos) {
        str.erase(end + 1);
    }
    return str;
}



/* getters */
RequestMethod	Request::getMethod() const {
	return method_;
}
std::string		Request::getUri() const {
	return uri_;
}
unsigned int	Request::getHttpVerMajor()const {
	return versionMajor_;
}

unsigned int	Request::getHttpVerMinor()const {
	return versionMinor_;
}
// std::string		Request::getHeaderKey(int index) const {
// 	return headers_[index].first;
// }
// std::string		Request::getHeaderValue(int index) const {
// 	return headers_[index].second;
// }

std::string		Request::getHeaderKey(int index) const {
	return headers_[index].first;
}
std::string		Request::getHeaderValue(int index) const {
	return headers_[index].second;
}
