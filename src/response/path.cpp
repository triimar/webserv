#include "../include/Response.hpp"

uint16_t Response::pathExists(std::string &path) {
    if (_request.getMethod() == POST) {
        if (access(path.c_str(), F_OK) == 0) { //already exists
            return (409);
        }
    } else if (access(path.c_str(), F_OK) == -1) {
        return (404);
    } else if (_request.getMethod() == GET && access(path.c_str(), R_OK) == -1) {
        return (403);
    }
    return (0);
}

void Response::findPath() {
    _localPath = combinePaths(_server.getRoot(), _request.getPath());

    _status = pathExists(_localPath);
    if (_status != 0 || _request.getMethod() == POST) {
        return ;
    }
    if (stat(_localPath.c_str(), &_pathStat) != 0) {
        _status = 500;
        return ;
    }
    
    // assuming that if it is a directory it takes the first index that it finds as per config and is valid with the request

    if (S_ISDIR(_pathStat.st_mode)) {
        for (std::vector<std::string>::iterator it = _server.getIndex.begin();
            it != _server.getIndex.end(); ++it) {
            _indexPath = combinePaths(_localPath, *it);
            _status = pathExists(_indexPath);
            if (_status == 0) {
                return ; // found valid index path
            }
        }
    }
    // didn't find a valid index path
    _indexPath = "";
}
