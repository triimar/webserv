#include "Request.hpp"

/* constructors & destructor */
Request::Request(): socketFd_(-1), state_(requestERROR) {

}

Request::Request(const int	socketFd): socketFd_(socketFd), state_(requestOK) {

}

Request::~Request() {

}

Request::Request(const Request& rhs): socketFd_(rhs.socketFd_), state_(rhs.state_), \
									method_(rhs.method_), url_(rhs.url_), httpVer_(rhs.httpVer_), \
									headers_(rhs.headers_), body_(rhs.body_) {

}

Request& Request::operator=(const Request& ) {
	return *this;
}

/* member functions that set member variables*/



/* getters */
RequestMethod	Request::getMethod() const {
	return method_;
}
std::string		Request::getUrl() const {
	return url_;
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