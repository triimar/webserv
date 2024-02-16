#include "../../include/Response.hpp"

Response::Response(const Server &serv, const Request &req) :
    _server(serv), _request(req), _isCGI(false), _cgiEnv(NULL), _status(0) {
    if (_status = (_request.getErrorCode()) == 0) {
        try {
            processRequest();
        } catch (int &e) {
            _status = e;
        }    
    }
}

Response::~Response() {
    if (_cgiEnv != NULL) {
        delete[] _cgiEnv;
    }
}
