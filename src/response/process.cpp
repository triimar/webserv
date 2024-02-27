#include "../../include/Response.hpp"

void Response::processRequest() {
    if (_status = (_request.getErrorCode()) != 0) {
        return ;
    }

    _location = _server.getLocation(_request.getPath());

    std::vector<RequestMethod> allowedMethods = _location.getAllowedMethods();
    if (std::find(allowedMethods.begin(), allowedMethods.end(), _request.getMethod()) == allowedMethods.end()) {
       throw 405; // request method is not allowed
    }

    _path = _location.getRoot() + _request.getPath();
    if (_request.getMethod() != POST) {
        if (access(_path.c_str(), F_OK) != 0) {
            throw 404;
        }
        struct stat pathStat;
        if (stat(_path.c_str(), &pathStat) != 0) {
            throw 500;
        }
        _pathIsDir = S_ISDIR(pathStat.st_mode);
    }

    if ((_isCGI = isCGI()) == true) {
        executeCGI();
        return ;
    }
   
    switch (_request.getMethod()) {
    case GET: 
        performGET();
        return ;
    case POST: 
        performPOST();
        return ;
    case DELETE: 
        performDELETE();
        return ;
    default:
        _status = 501;
    }
}

void Response::performGET() {
    if (access(_path.c_str(), R_OK) != 0) {
        throw 403;
    }
    
    if (_pathIsDir == false) {
        fileToBody(_path);
        return ;
    }
    if (_path.back() != '/') {
        throw 301; // why???
    }
    std::string index = getIndex();
    if (index.empty() == false) {
        fileToBody(index);
        return ;
    }

    if (_server.getAutoIndexing() == true) {
        makeDirectoryListing();
    }

    throw 403;
}

void Response::performPOST() {
    if (_request.getHeaderValueForKey("content-length").empty()) {
        throw 411;
    }
    if (Response::isSupportedMIMEType(_request.getHeaderValueForKey("content-type")) == false) {
        return (415);
    }

    // what happends with directories

    // create path with type

    // if ( /*location doesn't support upload*/ ) {
    //     return (403);
    // }

    _status = 201;
}

void Response::performDELETE() {
    if (_pathIsDir == false) {
        if (std::remove(_path.c_str()) == -1) {
            throw 500;
        }
        _status = 204;
        return ;
    }
    if (_path.back() != '/') {
        throw 409; // why??
    }
    if (access(_path.c_str(), W_OK) == -1) {
        throw 403;
    }
    if (rmdir(_path.c_str()) == -1) {
        throw 500;
    }
    _status = 204;
}
