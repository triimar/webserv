#include "../../include/Response.hpp"

std::string Response::getIndex() {
    std::string index;
    std::vector<std::string> indexes = _location.getIndex();
    for (std::vector<std::string>::iterator it = indexes.begin();
        it != indexes.end(); ++it) {
        index = combinePaths(_path, *it);
        if (access(index.c_str(), R_OK) == 0) {
            return (index); // found valid index path
        }
    }
    return (""); // no index path found
}

std::string Response::getCGIIndex() {
    std::string index;
    std::vector<std::string> indexes = _location.getIndex();
    for (std::vector<std::string>::iterator it = indexes.begin();
        it != indexes.end(); ++it) {
        index = combinePaths(_path, *it);
        if (isValidCGI(index) == true) {
            return (index); // found valid index path
        }
    }
    return (""); // no index path found
}
