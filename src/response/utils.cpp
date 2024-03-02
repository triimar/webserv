#include "../../include/webserv.hpp"

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

Return deleteDirectory(const std::string &path) {
    DIR *dir = opendir(path.c_str());
    struct dirent *nextFile;
    while ((nextFile = readdir(dir)) != NULL) {
        std::string fileName = nextFile->d_name;
        if (fileName == "." || fileName == "..") {
            continue ;
        }
        std::string file = combinePaths(path, fileName);
        std::cout << file << std::endl;
        struct stat fileStat;
        if (stat(file.c_str(), &fileStat) == -1) {
            closedir(dir);
            return (RETURN_FAILURE);
        }
        if (S_ISDIR(fileStat.st_mode)) {
            if (deleteDirectory(file) == RETURN_FAILURE) {
                closedir(dir);
                return (RETURN_FAILURE);
            }
        } else if (std::remove(file.c_str()) == -1) {
            closedir(dir);
            return (RETURN_FAILURE);
        }
    }
    closedir(dir);
    return ((remove(path.c_str()) == -1) ? RETURN_FAILURE : RETURN_SUCCESS);
}
