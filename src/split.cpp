#include "../../include/utils.hpp"

std::vector<std::string> splitString(const std::string str, char delim) {
    std::vector<std::string> tokens;
    std::istringstream tokenStream(str);
    std::string token;
    while (std::getline(tokenStream, token, delim)) {
        tokens.push_back(token);
    }
    return tokens;
}
