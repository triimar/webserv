#include "../../include/Response.hpp"

void Response::send() {
    makeErrorPage();
    if (_isCGI == false || _response.empty()) {
        setHeaders();
        constructResponse();
    }
    // send(_socket, _response.data(), _response.size(), 0);
}

void Response::makeErrorPage() {
    std::string path = _server.getErrorPage(_path, _status);
    if (path.empty()) {
        return ;
    }
    int fd = open(path.c_str(), O_RDONLY);
    if (fd == -1 || readToVector(fd, _body) == RETURN_FAILURE) {
        _body.clear();
        appendStringToVector(_body, Server::getStatusMessage(_status));
    }
    if (fd != -1) {
        close(fd);
    }
}

// std::string Response::getContentType

void Response::setHeaders() {
    _headers["Server"] = _server.getServerName();
    _headers["Content-Length"] = SSTR(_body.size());
    // if (_reqest.getMethod() == GET) {
    //    _headers["Content-Type"] = getContentType();
    // }
    
}

void Response::constructResponse() {
    appendStringToVector(_response, _request.getHttpVer().c_str());
    appendStringToVector(_response, " ");
    appendStringToVector(_response, Server::getStatusMessage(_status));
    appendStringToVector(_response, "\r\n");

    if (_headers.empty() == false) {
        for (std::map<std::string, std::string>::iterator it = _headers.begin();
            it != _headers.end(); ++it) {
            appendStringToVector(_response, it->first.c_str());
            appendStringToVector(_response, ": ");
            appendStringToVector(_response, it->second.c_str());
            appendStringToVector(_response, "\r\n");
        }
        appendStringToVector(_response, "\r\n");
    }

    _response.insert(_response.end(), _body.begin(), _body.end());
    appendStringToVector(_response, "\r\n\r\n");
}