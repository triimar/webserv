#include "../../include/utils.hpp"

void appendStringToVector(std::vector<char> &vector, const char *str) {
    if (str) {
        std::string temp(str);
        vector.insert(vector.end(), temp.begin(), temp.end());
    }
}

Return readToVector(int fd, std::vector<char> &vec) {
    char buf[BUFFER_SIZE + 1] = {'\0'};
    ssize_t readBytes;
    while ((readBytes = read(fd, buf, BUFFER_SIZE)) > 0) {
        for (ssize_t i = 0; i < readBytes; ++i) {
            vec.push_back(buf[i]);
        }
    }
    if (readBytes == -1) {
        return (RETURN_FAILURE);
    }
    return (RETURN_SUCCESS);
}
