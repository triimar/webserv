#include "../../include/utils.hpp"

std::string combinePaths(const std::string &lhs, const std::string &rhs) {
    switch ((lhs.at(lhs.size()- 1) == '/') + (rhs.at(0) == '/')) {
    case 0:
        return (lhs + '/' + rhs);
    case 2:
        return (lhs + rhs.substr(1));
    default:
        return (lhs + rhs);
    }
}
