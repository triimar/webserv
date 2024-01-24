#include "../../include/Response.hpp"

void Response::processRequest() {
    findPath();
    if ((_request.getMethod() & CGI_METHODS) && isValidCGI()) {
        executeCGI();
    } else {
        
        constructResponse();
    }
}

void Response::constructResponse() {
    appendStringToVector(_response, _request.getHttpVer().c_str());
    appendStringToVector(_response, " ");
    appendStringToVector(_response, _server.getStatusMessage(_status));
    appendStringToVector(_response, "\r\n");

    for (std::map<std::string, std::string>::iterator it = _headers.begin();
        it != _headers.end(); ++it) {
        appendStringToVector(_response, it->first.c_str());
        appendStringToVector(_response, ": ");
        appendStringToVector(_response, it->second.c_str());
        appendStringToVector(_response, "\r\n");
    }
    appendStringToVector(_response, "\r\n");

    _response.insert(_response.end(), _body.begin(), _body.end());
    // appendStringToVector(_response, "\r\n\r\n");
}
