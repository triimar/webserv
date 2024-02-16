#include "../../include/Response.hpp"

void Response::processRequest() {
    checkLocation();

    if (isCGI()) {
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

void Response::checkLocation() {
    _location = getLocation(_request.getPath());

    std::vector<RequestMethod> allowedMethods = _location.getAllowedMethods();
    if (std::find(allowedMethods.begin(), allowedMethods.end(), _request.getMethod()) == allowedMethods.end()) {
       throw 405; // request method is not allowed
    }

    _path = _location.getRoot() + _request.getPath();
    if (access(_path.c_str(), F_OK) != 0) {
        throw 404;
    }
    if (stat(_path.c_str(), &_pathStat) != 0) {
        throw 500;
    }

    // int returnStatus = _location.getReturn();
    // if (returnStatus != 0) {
    //     throw returnStatus;
    // }
}

void Response::fileToBody(std::string &path) {
    int fd = open(path.c_str(), O_RDONLY);
    if (fd == -1) {
        if (errno == ENOENT) {
            throw 404;
        }
        throw 500;
    }
    if (readToVector(fd, _body) == RETURN_FAILURE) {
        close(fd);
        throw 500;
    }
    close(fd);
    _status = 200;
}

void Response::performGET() {
    if (S_ISDIR(_pathStat.st_mode) == false) {
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
    if (_request.getHeaderValueForKey("Content-Length").empty()) {
        throw 411;
    }
    // if (Response::isSupportedMIMEType(_request.getHeaderValueForKey("Content-Type")) == false) {
    //     return (415);
    // }

    // what happends with directories

    // create path with type

    // if ( /*location doesn't support upload*/ ) {
    //     return (403);
    // }

    _status = 201;
}

void Response::performDELETE() {
    if (S_ISDIR(_pathStat.st_mode) == false) {
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
