#include "../../include/Server.hpp"

// checks location config for `loc`, if there is one and has allow_methods checks if `method` is allowed - otherwise tries (global) server config for allow_methods
bool Server::checkLocationMethod(const std::string &path, RequestMethod method) const {
    (void)path;
    (void)method;
    return (true);
}

// takes request path looks in location config, if there is root combine the paths and return - otherwise use (global) server config for root
std::string Server::getLocationPath(const std::string &path) const {
    // combinePaths(correct_root, request);
    (void)path;
    return ("placeholder");
}

// checks if location allows the request method
bool Server::isLocationMethodAllowed(const std::string &path, RequestMethod method) const {
    (void)path;
    (void)method;
    return (true);
}

// returns path to error page of that status if it was defined in location
std::string Server::getLocationErrorPage(const std::string &path, uint16_t status) const {
    (void)path;
    (void)status;
    return ("placeholder");
}

std::vector<std::string> Server::getLocationIndexes(const std::string &path) const {
    (void)path;
    std::vector<std::string> placeholder;
    return (placeholder);
}

std::string Server::getErrorPage(const std::string &path, uint16_t status) const {
    (void)path;
    (void)status;
    return ("placeholder");
}