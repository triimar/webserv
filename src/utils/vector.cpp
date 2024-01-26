#include "../../include/utils.hpp"

void appendStringToVector(std::vector<char> &vector, const char *str) {
    if (str) {
        std::string temp(str);
        vector.insert(vector.end(), temp.begin(), temp.end());
    }
}

ssize_t readToVector(int fd, std::vector<char> &vec) {
    char buf[BUFFER_SIZE + 1] = {'\0'};
    ssize_t fileSize = 0;
    ssize_t readBytes;
    while ((readBytes = read(fd, buf, BUFFER_SIZE)) > 0) {
        buf[readBytes] = '\0';
        appendStringToVector(vec, buf);
        fileSize += readBytes;
    }
    if (readBytes == -1) {
        return (-1);
    }
    return (fileSize);
}
