#include "../../include/utils.hpp"

std::vector<std::string> splitString(const std::string& input, const std::string delim) {
    std::vector<std::string> result;
    size_t pos = input.find_first_not_of(delim, 0);
    while (pos != std::string::npos) {
        size_t next = input.find_first_of(delim, pos);
        if (next != std::string::npos) {
            result.push_back(input.substr(pos, next - pos));
            pos = input.find_first_not_of(delim, next);
        } else {
            result.push_back(input.substr(pos));
            break ;
        }
    }
    return (result);
}
