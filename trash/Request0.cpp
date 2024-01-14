#include "Request0.hpp"

#define CRLFCRLF "\r\n\r\n" 
// \r\n is end-of-line marker  for all protocol elements except the entity-body

/* constructors & destructor */


Request::Request(): state_(requestOK), method_(OTHER) {
}

Request::~Request() {

}

Request::Request(const Request& rhs): state_(rhs.state_), \
									method_(rhs.method_), uri_(rhs.uri_), headers_(rhs.headers_), body_(rhs.body_) {
}

Request& Request::operator=(const Request& ) {
	return *this;
}

/* member functions that set member variables*/

void	Request::setRequestMethod() {
	const std::string methods[3] = {"GET", "POST", "DELETE"};
	for (int i = 0; i < 3; i++) {
		if (methods[i] == methodStr_) {
			method_ = static_cast<RequestMethod>(i);
			break ;
		}
	}
}

int Request::processRequestLine(char *bufRequestLine) {
    std::stringstream rls(bufRequestLine);
	rls >> methodStr_;
	if (!rls) {
		return false; // why failed, in which way - can fail only if there is nothing in the string?
	}
	setRequestMethod();
	
	rls >> uri_;
	if (!rls.bad())
		return false; // why failed, in which 
	else if (rls.fail() || rls.eof() || uri_.length() == 0)
		return false; //set error code
	//validate uri_
	
	rls >> httpVer_;
	if (!rls.bad())
		return false; // why failed, in which 
	else if (rls.fail() || rls.eof() || httpVer_.length() == 0)
		return false; //set error code
	if (httpVer_.compare(0, httpVer_.length(), "HTTP/1.1") != 0) { //not a valid idea, because there is bad request version 400 & version not supported 505
        return false;
    }

	std::cout << "|"<< methodStr_ << "|" << uri_ << "|" << httpVer_ << "|" << errorCode_ << std::endl;
	return true;
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

std::string		Request::getHttpVer()const {
	return httpVer_;
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
