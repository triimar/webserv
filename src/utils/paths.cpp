#include "../../include/utils.hpp"

std::string combinePaths(std::string &lhs, std::string &rhs) {
    switch ((lhs.back() == '/') + (rhs.front() == '/')) {
    case 0:
        return (lhs + '/' + rhs);
    case 2:
        return (lhs + rhs.substr(1));
    default:
        return (lhs + rhs);
    }
}

bool hasReadPermissions(std::string &path) {
    return (access(path.c_str(), R_OK) == 0);
}
