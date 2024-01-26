#include "../../include/Response.hpp"

void Response::processRequest() {
    if (_request.getMethod() == OTHER) {
        _status = 501;
        return ;
    }
    findPath();
    if (_status != 0) {
        return ;
    }
    if ((_request.getMethod() & CGI_METHODS) && isValidCGI()) {
        _isCGI = true;
        executeCGI();
        return ;
    }
    switch (_request.getMethod()) {
    case GET:
        _status = performGET();
        return ;
    case POST:
        _status = performPOST();
        return ;
    case DELETE:
        _status = performDELETE();
    }
}

uint16_t Response::writeFile(std::string &path) {
    int fd = open(path.c_str(), O_RDONLY);
    if (fd == -1) {
        switch (errno) {
        case ENOENT: return (404);
        default: return (500);
        }
    }
    if (readToVector(fd, _body) == RETURN_FAILURE) {
        close(fd);
        return (500);
    }
    close(fd);
    return (200);
}

std::string Response::getAutoIndex() {

}

uint16_t Response::performGET() {
    if (S_ISDIR(_pathStat.st_mode == false)) {
        return (writeFile(_localPath));
    }
    if (_localPath.back() != '/') {
        return (301); // why???
    }
    if (_indexPaths.empty() == false) {
        return (writeFile(_indexPaths[0]));
    }
    std::string autoIndex = getAutoIndex();
    if (autoIndex.empty() == false) {
        return (writeFile(autoIndex));
    }
    return (403);
}

uint16_t Response::performPOST() {
    if (_request.getHeaderValueForKey("Content-Length").empty()) {
        return (411);
    }
    if (isSupportedMIMEType(_request.getHeaderValueForKey("XXXX")) == false) {
        return (415);
    }

    // create path with type

    // if ( /*location doesn't support upload*/ ) {
    //     return (403);
    // }

    // _headers["Location"] = path;
    return (201);
}

uint16_t Response::performDELETE() {
    if (S_ISDIR(_pathStat.st_mode == false)) {
        if (std::remove(_localPath.c_str()) == -1) {
            return (500);
        }
        return (204);
    }
    if (_localPath.back() != '/') {
        return (409); // why??
    }
    if (access(_indexPath.c_str(), W_OK) == -1) {
        return (403);
    }
    if (rmdir(_localPath.c_str()) == -1) {
        return (500);
    }
    return (204);
}
