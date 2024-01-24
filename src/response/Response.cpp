#include "../../include/Response.hpp"

Response::Response(const Server &serv, const Request &req) :
    _server(serv), _request(req), _cgiEnv(NULL), _status(0) {
    processRequest();
}

Response::~Response() {
    if (_cgiEnv != NULL) {
        delete[] _cgiEnv;
    }
}
