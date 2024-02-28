#include "../../include/Response.hpp"

// only inside cgi child process
char **vectorToArray(const std::vector<std::string> &vec) {
    if (vec.empty()) {
        return (NULL);
    }
    char **res = (char **)calloc(vec.size() + 1, sizeof(char *));
    if (res == NULL) {
        return (NULL);
    }
    for (size_t i = 0; i < vec.size(); ++i) {
        res[i] = (char *)calloc(vec[i].size() + 1, sizeof(char));
        if (res[i] == NULL) {
            free_2d_array((void **)res);
            return (NULL);
        }
        std::strcpy(res[i], vec[i].c_str());
    }
    return (res);
}

std::string formatDate(time_t pit) {
    char buf[DATE_FORMAT_LEN + 1];
    if (strftime(buf, DATE_FORMAT_LEN + 1, DATE_FORMAT, gmtime(&pit)) == DATE_FORMAT_LEN) {
        return (buf);
    }
    return ("");
}

std::string capitalizeHeader(std::string name) {
    if (name == "www-authenticate") {
        name = "WWW-Authenticate";
        return name;
    }
    for (size_t i = 0; i != std::string::npos; i = name.find('-', i + 1)) {
        name[i + !!i] = std::toupper(name[i + !!i]);
    }
	return name;
}
