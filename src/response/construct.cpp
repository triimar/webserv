#include "../../include/webserv.hpp"

void Response::constructResponse() {
    if (_request.getMethod() == HEAD) {
        _body.clear();
    }
    _response.clear();
    if (_status >= 400) {
        makeErrorPage();
    }
    appendStringToVector(_response, HTTP_VERSION);
    appendStringToVector(_response, " ");
    appendStringToVector(_response, Server::getStatusMessage(_status));
    appendStringToVector(_response, CRLF);
    _headers["server"] = _server.getServerName();
    _headers["content-length"] = SSTR(_body.size());
    _headers["date"] = formatDate(time(0));
    for (std::map<std::string, std::string>::iterator it = _headers.begin();
        it != _headers.end(); ++it) {
        appendStringToVector(_response, it->first.c_str());
        appendStringToVector(_response, ": ");
        appendStringToVector(_response, it->second.c_str());
        appendStringToVector(_response, CRLF);
    }
    appendStringToVector(_response, CRLF);
    if (_body.empty() == false) {
        _response.insert(_response.end(), _body.begin(), _body.end());
        appendStringToVector(_response, CRLFCRLF);
    }
}

void Response::makeErrorPage() {
    std::string path = _server.getErrorPage(_status);
    if (path.empty() == false) {
        _body.clear();
        int fd = open(path.c_str(), O_RDONLY);
        if (readToVector(fd, _body) == RETURN_FAILURE) {
            appendStringToVector(_body, Server::getStatusMessage(_status));
        }
        close(fd);
    }
}
