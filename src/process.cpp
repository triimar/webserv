#include "../../include/Response.hpp"

uint16_t Response::processRequest() {
    if ((_status = checkRequest()) != 0) {
        return (_status);
    }
    if (isCGI()) {
        _isCGI = true;
        if (_request.getMethod() == DELETE) {
            return (501);
        }
        if (_server.isLocationMethodAllowed(_path, _request.getMethod())) {
            return (405);
        }
        executeCGI();
        return (_status);
    }
    switch (_request.getMethod()) {
    case GET:
        return (performGET());
    case POST:
        return (performPOST());
    case DELETE:
        return (performDELETE());
    }
}

uint16_t Response::checkRequest() {
    if (_request.getMethod() == OTHER) {
        return (501);
    } else if (_server.checkLocationMethod(_request.getPath(), _request.getMethod()) == false) {
        return (405);
    }
    // ???
    if (_server.hasLocationReturn(_request.getPath(), _status, _body) == true) {
        return (_status);
    }

    _path = _server.getLocationPath(_request.getPath());
    if (access(_path.c_str(), F_OK) == 0) {
        return (404);
    }
    if (stat(_path.c_str(), &_pathStat) != 0) {
        return (500);
    }

    return (0);
}

uint16_t Response::fileToBody(std::string &path) {
    int fd = open(path.c_str(), O_RDONLY);
    if (fd == -1) {
        if (errno == ENOENT) {
            return (404);
        }
        return (500);
    }
    if (readToVector(fd, _body) == RETURN_FAILURE) {
        close(fd);
        return (500);
    }
    close(fd);
    return (200);
}

uint16_t Response::performGET() {
    if (S_ISDIR(_pathStat.st_mode == false)) {
        return (fileToBody(_path));
    }
    if (_path.back() != '/') {
        return (301); // why???
    }
    std::string index = _server.getIndex(hasReadPermissions);
    if (index.empty() == false) {
        return (fileToBody(index));
    }
    // ???
    if (_server.getAutoIndexing() == true) {
        if (makeAutoIndex() == RETURN_SUCCESS) {
            return (200);
        }
        return (500);
    }
    return (403);
}

uint16_t Response::performPOST() {
    if (_request.getHeaderValueForKey("Content-Length").empty()) {
        return (411);
    }
    if (Response::isSupportedMIMEType(_request.getHeaderValueForKey("Content-Type")) == false) {
        return (415);
    }

    // what happends with directories

    // create path with type

    // if ( /*location doesn't support upload*/ ) {
    //     return (403);
    // }

    // _headers["Location"] = path;
    return (201);
}

uint16_t Response::performDELETE() {
    if (S_ISDIR(_pathStat.st_mode == false)) {
        if (std::remove(_path.c_str()) == -1) {
            return (500);
        }
        return (204);
    }
    if (_path.back() != '/') {
        return (409); // why??
    }
    if (access(_indexPath.c_str(), W_OK) == -1) {
        return (403);
    }
    if (rmdir(_path.c_str()) == -1) {
        return (500);
    }
    return (204);
}
