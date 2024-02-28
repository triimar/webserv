#include "../../include/utils.hpp"

void appendStringToVector(std::vector<char> &vector, std::string str) {
    if (str.empty() == false) {
        vector.insert(vector.end(), str.begin(), str.end());
    }
}

Return readToVector(int fd, std::vector<char> &vec) {
    char buf[BUFFER_SIZE];
    ssize_t readBytes;
    while ((readBytes = read(fd, buf, BUFFER_SIZE)) > 0) {
        vec.insert(vec.end(), buf, buf + readBytes);
    }
    return ((readBytes == -1) ? RETURN_FAILURE : RETURN_SUCCESS);
}

std::vector<char>::iterator findSubstring(std::vector<char>::iterator begin, std::vector<char>::iterator end, std::string s) {
    return (std::search(begin, end, s.begin(), s.end()));
}
