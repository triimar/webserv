#include "../../include/Response.hpp"

void Response::fileToBody(std::string &path) {
    int fd = open(path.c_str(), O_RDONLY);
    if (fd == -1) {
        if (errno == ENOENT) {
            throw 404;
        }
        throw 500;
    }
    if (readToVector(fd, _body) == RETURN_FAILURE) {
        close(fd);
        throw 500;
    }
    close(fd);
    _status = 200;
}

std::string Response::getIndex() {
    std::vector<std::string> indexes = _location.getIndex();
    for (std::vector<std::string>::iterator it = indexes.begin();
        it != indexes.end(); ++it) {
        std::string index = combinePaths(_path, *it);
        if (access(index.c_str(), R_OK) == 0) {
            return (index); // found valid index path
        }
    }
    return (""); // no index path found
}

// only inside cgi child process
char **vectorToArray(const std::vector<std::string> &vec) {
    if (vec.empty()) {
        return (NULL);
    }
    char **res = calloc(vec.size() + 1, sizeof(char *));
    if (res == NULL) {
        return (NULL);
    }
    for (size_t i = 0; i < vec.size(); ++i) {
        res[i] = calloc(vec[i].size() + 1, sizeof(char));
        if (res[i] == NULL) {
            free_2d_array(static_cast<void**>(res));
            return (NULL);
        }
        std::strcpy(res[i], vec[i].c_str());
    }
    return (res);
}

std::string getCurrentDate() {
    time_t now = time(0);
    char buf[DATE_FORMAT_LEN + 1];
    if (strftime(buf, DATE_FORMAT_LEN + 1, DATE_FORMAT, gmtime(&now)) == DATE_FORMAT_LEN) {
        return (buf);
    }
    return ("");
}
