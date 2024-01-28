#include "../../include/Response.hpp"

    // if (_request.getMethod() == POST) {
    //     if (access(path.c_str(), F_OK) == 0) { //already exists
    //         return (409);
    //     }
    // } else if (access(path.c_str(), F_OK) == -1) {
    //     return (404);
    // } else if (_request.getMethod() == GET && access(path.c_str(), R_OK) == -1) {
    //     return (403);
    // }
    // return (0);

std::string Response::getIndex(bool (*condition)(std::string &path)) {
    std::string index;
    std::vector<std::string> &location = _server.getLocationIndexes(_path);
    for (std::vector<std::string>::iterator it = location.begin();
        it != location.end(); ++it) {
        index = combinePaths(_path, *it);
        if (condition(index)) {
            return (index); // found valid index path
        }
    }
    return (""); // no index path found
}

Return Response::tryAutoIndex() {

}
