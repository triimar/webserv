#include "Request.hpp"


//Orthodox canonical form
Request::Request(): state_(stateParseRequestLine), rlstate_(stateParseMethod), method_(OTHER), errorCode_(0) {
}


Request::~Request() {

}

Request::Request(const Request& rhs): state_(rhs.state_), rlstate_(rhs.rlstate_), methodStr_(rhs.methodStr_), method_(rhs.method_), \
								uri_(rhs.uri_), path_(rhs.path_), httpVer_(rhs.httpVer_), \
								headers_(rhs.headers_), errorCode_(rhs.errorCode_), errorMsg_(rhs.errorMsg_), body_(rhs.body_) {
}

Request& Request::operator=(const Request& ) {
	return *this;
}

void Request::setError(ParseState type, int errorCode, const char *message) {
	state_ = type;
	errorCode_ = errorCode;
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
	const char *bodyStart = headersEnd + 4; //moving pointer past CRLFCRLF; if now (start == msgEnd) -> no body
	headersStream << tmp;
	return bodyStart;
}

// void Request::createHeadersStream(std::stringstream& headersStream, const char *requestBuf, const char *msgEnd) {

// 	std::size_t len = static_cast<std::size_t>(msgEnd - 4 - requestBuf); //excluding CRLFCRLF from end
// 	char tmp[len + 1];
//     std::memcpy(tmp, requestBuf, len);
// 	tmp[len] = '\0';
// 	headersStream << tmp;
// }

void	Request::parseMethod(std::stringstream& requestLine) {
	requestLine >> methodStr_;
	if (requestLine.fail() || methodStr_.empty())
		return setError(requestERROR, 400, "Bad Request");
	const std::string methods[3] = {"GET", "POST", "DELETE"};
	for (int i = 0; i < 3; i++) {
		if (methods[i] == methodStr_) {
			method_ = static_cast<RequestMethod>(i);
			break ;
		}
	}
	if (method_ == OTHER)
		return setError(requestERROR, 501, "Not Implemented");
	rlstate_ = stateParseUri;
	return ;
}

void	Request::parseURI(std::stringstream& requestLine) {
	std::string tmp;
	requestLine >> tmp;
	if (requestLine.fail() || tmp.empty() || containsControlChar(tmp))
		return setError(requestERROR, 400, "Bad Request");
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
			return setError(requestERROR, 400, "Bad Request");
		componentStart = tmp.find('/', 8);
		if (componentStart != std::string::npos)
			tmp = tmp.substr(componentStart);
	}
	if (tmp.at(0) == '/')
		path_ = tmp;
	else
		return setError(requestERROR, 400, "Bad Request");
	rlstate_ = stateParseHTTPver;
	return ;
}

void	Request::parseHTTPver(std::stringstream& requestLine) {
	requestLine >> httpVer_;
	if (requestLine.fail() || httpVer_.empty() || !requestLine.eof() || httpVer_.compare(0, 5, "HTTP/") != 0)
		return setError(requestERROR, 400, "Bad Request");
	std::stringstream verStr(httpVer_.substr(5));
	int	major, minor;
	char dot;
	verStr >> major >> dot >> minor;
	if (verStr.fail() || dot != '.' || verStr.peek() != EOF)
		return setError(requestERROR, 400, "Bad Request Version");
	if (major != 1 || minor != 1)
		return setError(requestERROR, 505, "HTTP Version Not Supported");
	rlstate_ = requestLineOK;
	return ;
}

void	Request::parseRequestLine(std::stringstream& headersStream) {
	std::string rl;
	std::getline(headersStream, rl);
	if (!headersStream || rl.empty())
		return setError(requestParseFAIL, 400, "Bad Request"); //???
	else if (rl.back() != '\r')
		return setError(requestERROR, 400, "Bad Request");
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
		return setError(requestERROR, 400, "Bad Request");
	if (std::getline(headersStream, line)) {
		std::istringstream iss(line);
		if (!std::getline(iss, key, ':') || std::isspace(key.back()) || !std::getline(iss, value, '\r'))
			return setError(requestERROR, 400, "Bad Request");
		trimString(key);
		trimString(value);
	}
	while ((headersStream && (headersStream.peek() == 32 || headersStream.peek() == 10))) {
		std::string moreValue;
		std::getline(headersStream, line);
		std::stringstream more(line);
		if ( !std::getline(more, moreValue, '\r'))
			return setError(requestERROR, 400, "Bad Request");
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
			return setError(requestERROR, 400, "Bad Request: Request method contains a message body"); //maybe not an error?
	}
	else if (isTransferEncodingChunked())
		state_ = stateParseChunkedBody;
	else
		state_ = stateParseMessageBody;
}

void	Request::storeBody(const char *bodyStart, const char *msgEnd) {

	std::string bodyLenStr = getHeaderValueForKey("Content-Length");
	if (bodyLenStr.empty())
		return setError(requestERROR, 411, "Length Required: Missing Content-Length header in POST request");
	char *endptr;
	long contentLen = strtol(bodyLenStr.c_str(), &endptr, 10);
	if (contentLen != msgEnd - bodyStart - 3) { //-2 when I assume it is not 0 terminated
		std::cout << bodyLenStr << "   " << contentLen << " **** " << (msgEnd - bodyStart - 3) << std::endl;
		return setError(requestERROR, 400, "Bad Request: Mismatch between actual and declared Content-Length");
	}
	while (bodyStart != msgEnd && contentLen > 0) {
		body_.push_back(*bodyStart);
		bodyStart++;
		contentLen--;
	}
	state_ = requestOK;
}

void 	Request::decodeChunked(const char *bodyStart, const char *msgEnd) {

	// void 	Request::decodeChunked(const char *chunk, int len)
	char *endptr;
	const char *chunk = bodyStart;
	unsigned long chunkLen;
	// while (chunkLen != 0 && chunk != msgEnd) {
	// 	if (*endptr != ';' && *endptr != '\r')
	// 		return setError(requestERROR, 400, "Bad Request");
	// 	chunk = std::strstr(chunk, "\r\n");
	// 	if (!chunk)
	// 		return setError(requestERROR, 400, "Bad Request");
	// 	chunk += 2;
	// 	while (chunk != msgEnd && chunkLen > 0) {
	// 		body_.push_back(*chunk);
	// 		chunk++;
	// 		chunkLen--;
	// 	}
	// }
	while (chunk != msgEnd) {
		chunkLen = strtoul(chunk, &endptr, 16);
		std::cout << "LEN " << chunkLen << std::endl;
		if (*endptr != ';' && *endptr != '\r')
			return setError(requestERROR, 400, "Bad Request");
		else if (chunkLen == 0)
			break;
		chunk = std::strstr(chunk, "\r\n");
		if (!chunk)
			return setError(requestERROR, 400, "Bad Request");
		chunk += 2;
		std::cout << chunk << std::endl;
		while (chunk != msgEnd && chunkLen > 0) {
			body_.push_back(*chunk);
			chunk++;
			chunkLen--;
		}
	}
	if (chunkLen == 0) {
		state_ = requestOK;
		return ;
	}
	else
		return setError(requestERROR, 400, "Bad Request");



	// const char* chunkEnd = chunk + len;
	// unsigned long chunkLen = strtoul(chunk, &endptr, 16);
	// if (*endptr != ';' && *endptr != '\r')
	// 	return setError(requestERROR, 400, "Bad Request");
	// if (chunkLen == 0) {
	// 	state_ = requestOK;
	// 	return;
	// }
	// chunk = std::strstr(chunk, "\r\n");
	// if (!chunk)
	// 	return setError(requestERROR, 400, "Bad Request");
	// chunk += 2;
	// while (chunk != chunkEnd && chunkLen > 0) {
	// 	body_.push_back(*chunk);
	// 	chunk++;
	// }
	// if (chunk == chunkEnd || *chunk != '\r')
	// 	return setError(requestERROR, 400, "Bad Request");
}

// void	Request::processHeaders(const char* requestBuf, int messageLen) { //what if len == 0?
// 	const char *msgEnd = &requestBuf[messageLen - 1];

// 	if (std::strncmp(msgEnd - 4, CRLFCRLF,  4) != 0)
// 		return setError(requestERROR, 400, "Bad Request: Syntax error"); 	//there is no CRLFCRLF -> bad Request (syntax error)
// 	if (requestBuf == msgEnd)
// 		return setError(requestERROR, 400, "Bad Request: Empty request");
// 	std::stringstream headersStream;
// 	createHeadersStream(headersStream, requestBuf, msgEnd);
// 	while (state_ != requestERROR && state_ != requestParseFAIL && state_ != requestOK && state_ != stateExpectingBody) {
// 		switch (state_) {
// 		case stateParseRequestLine: parseRequestLine(headersStream);
// 			break;
// 		case stateParseHeaders: parseHeader(headersStream);
// 			break;
// 		case stateCheckBody: checkForBody();
// 		case stateExpectingBody:
// 		case requestParseFAIL:
// 		case requestERROR:
// 		case requestOK:
// 			break;
// 		}
// 	}
// }

// void	Request::processHeaders(const char* requestBuf, int messageLen) { //what if len == 0?
// 	const char *msgEnd = &requestBuf[messageLen - 1];
// 	if (std::strncmp(msgEnd - 4, CRLFCRLF,  4) != 0)
// 		return setError(requestERROR, 400, "Bad Request: Syntax error"); 	//there is no CRLFCRLF -> bad Request (syntax error)
// 	while (requestBuf != msgEnd && std::isspace(*requestBuf))
// 		requestBuf++;
// 	if (requestBuf == msgEnd)
// 		return setError(requestERROR, 400, "Bad Request: Empty request");
// 	std::stringstream headersStream;
// 	createHeadersStream(headersStream, requestBuf, msgEnd);
// 	while (state_ != requestERROR && state_ != requestParseFAIL && state_ != requestOK) {
// 		switch (state_) {
// 		case stateParseRequestLine: parseRequestLine(headersStream);
// 			break;
// 		case stateParseHeaders: parseHeader(headersStream);
// 			break;
// 		case StateParseMessageBody:
// 			break;
// 		case requestParseFAIL:
// 			break;
// 		case requestERROR:
// 			break;
// 		case requestOK:
// 			break;
// 		}
// 	}
// }
void	Request::processRequest(const char* requestBuf, int messageLen) { //what if len == 0?
	std::stringstream headersStream;
	const char *msgEnd = &requestBuf[messageLen - 1];
	const char *bodyStart = extractHeadersStream(headersStream, requestBuf, msgEnd);
	if (!bodyStart)
		return setError(requestERROR, 400, "Bad Request"); 	//there is no CRLFCRLF -> bad Request (syntax error)
	// if (bodyStart == msgEnd)
	// 	std::cout << "NOBODY" << std::endl;
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
			case stateParseChunkedBody: decodeChunked(bodyStart, msgEnd);
			case requestParseFAIL:
				break;
			case requestERROR:
				break;
			case requestOK:
				break;
		}
	}
}

/* getters */
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
	return errorCode_;
}

const std::string&		Request::getErrorMsg() const {
	return errorMsg_;
}

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

const std::vector<char> Request::getBody() const {
    return body_;
}