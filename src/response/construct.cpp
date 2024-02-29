#include "../../include/webserv.hpp"

void Response::constructResponse() {
    _response.clear();
    if (_status >= 300) {
        _body.clear();
        if (_status >= 400) {
            _headers.clear();
            makeErrorPage();
        }
        _headers["content-length"] = SSTR(_body.size());
    }
    appendStringToVector(_response, _request.getHttpVer().c_str());
    appendStringToVector(_response, " ");
    appendStringToVector(_response, Server::getStatusMessage(_status));
    appendStringToVector(_response, CRLF);
    _headers["server"] = _server.getServerName();
    _headers["date"] = formatDate(time(0));
    for (std::map<std::string, std::string>::iterator it = _headers.begin();
        it != _headers.end(); ++it) {
//        capitalizeHeader(it->first);
        appendStringToVector(_response, it->first.c_str());
        appendStringToVector(_response, ": ");
        appendStringToVector(_response, it->second.c_str());
        appendStringToVector(_response, CRLF);
    }
    appendStringToVector(_response, CRLF);
    _response.insert(_response.end(), _body.begin(), _body.end());
    appendStringToVector(_response, CRLFCRLF);
}

void Response::makeErrorPage() {
    std::string path = _server.getErrorPage(_status);
    int fd = -1;
    if (path.empty() == false) {
        fd = open(path.c_str(), O_RDONLY);
    }
    if (fd == -1 || readToVector(fd, _body) == RETURN_FAILURE) {
        appendStringToVector(_body, Server::getStatusMessage(_status));
    }
    if (fd != -1) {
        close(fd);
    }
}
