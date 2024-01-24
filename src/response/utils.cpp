#include "../include/Response.hpp"

uint16_t Response::isValidPath(std::string &path) {
    if (access(_indexPath.c_str(), F_OK) == -1) {
        return (404);
    }
    if (_request.getMethod() == GET && access(_indexPath.c_str(), R_OK) == -1) {
        return (403);
    }
    return (0);
}

void Response::findPath() {
    _localPath = combinePaths(_server.getRoot(), _request.getPath);
    if (stat(_localPath.c_str(), &_pathStat) != 0) {
        _status = 500;
        return ;
    }

    // assuming that if it is a directory it takes the first index that it finds as per config and is valid with the request

    // check auto index / indexing on?

    if (S_ISDIR(_pathStat.st_mode)) {
        for (std::vector<std::string>::iterator it = _server.getIndex.begin();
            it != _server.getIndex.end(); ++it) {
            _indexPath = combinePaths(_localPath, *it);
            _status = isValidPath(_indexPath);
            if (_status == 0) {
                // found valid index path
                return ;
            }
        }
    } else {
        _status = isValidPath(_localPath);
    }
    // didn't find a valid index path
    _indexPath = "";
}
