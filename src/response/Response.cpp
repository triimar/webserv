#include "../../include/webserv.hpp"

std::string Response::_supportedCGI;

Response::Response(const Server &serv, Request &req) :
    _server(serv), _request(req), _isCGI(false), _status(0) {

    _supportedCGI = "," + std::string(SUPPORTED_CGI) + ",";
    
    try {
        processRequest();
    } catch (int &e) {
        _status = e;
    }

    constructResponse();
}

Response::~Response() {}

std::vector<char> &Response::getResponse() {
    return (_response);
}
