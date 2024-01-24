#include "../../include/utils.hpp"

void appendStringToVector(std::vector<char> &vector, const char *str) {
    vector.insert(vector.end(), str, str + strlen(str));
}