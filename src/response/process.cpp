#include "../../include/webserv.hpp"

void Response::processRequest() {

    _location = _server.getLocation(_request.getPath());

    std::vector<RequestMethod> allowedMethods = _location.getAllowedMethods();
    if (std::find(allowedMethods.begin(), allowedMethods.end(), _request.getMethod()) == allowedMethods.end()) {
        throw 405; // request method is not allowed
    }

    _path = _location.getRoot() + cleanPath(_request.getPath());
    if (_request.getMethod() != POST) {
        if (access(_path.c_str(), F_OK) != 0) {
            throw 404;
        }
        if (stat(_path.c_str(), &_pathStat) != 0) {
            throw 500;
        }
    }

    if ((_status = _request.getErrorCode()) != 0) {
        return ;
    }
    if ((_isCGI = isCGI()) == true) {
        executeCGI();
        return ;
    }

    switch (_request.getMethod()) {
    case GET: 
        performGET();
        return ;
    case POST: 
        performPOST();
        return ;
    case DELETE: 
        performDELETE();
        return ;
    default:
        _status = 501;
    }
}

std::string Response::cleanPath(const std::string &path) {
    std::vector<std::string> components;
    std::istringstream stream(path);
    std::string component;

    while (std::getline(stream, component, '/')) {
        if (component == "..") {
            if (components.empty()) {
                throw 403; // directory traversal attempt
            }
            components.pop_back();
        } else if (component != "." && !component.empty()) {
            components.push_back(component);
        }
    }
    std::ostringstream cleanedPath;
    for (size_t i = 0; i < components.size(); ++i) {
        cleanedPath << '/' << components[i];
    }
    std::string result = cleanedPath.str();
    if (path.at(path.size()- 1) == '/') {
        result.push_back('/');
    }
    return (result.empty() ? "/" : result);
}

void Response::performGET() {
    if (access(_path.c_str(), R_OK) != 0) {
        throw 403;
    }
    std::string pathExtension = _path;
    if (S_ISDIR(_pathStat.st_mode) == false) {
        fileToBody(_path);
    } else {
        if (_path.at(_path.size()- 1) != '/') {
            _headers["location"] = _request.getUri() + "/";
            throw 301;
        }
        pathExtension = getIndex();
        if (pathExtension.empty() == false) { // has index file
            fileToBody(pathExtension);
        } else if (_location.isAutoIndex() == true) { // auto-indexing on
            makeDirectoryListing();
            pathExtension = "html";
        } else { // directory without response
            throw 403;
        }
    }
    _status = 200;
    _headers["content-length"] = SSTR(_body.size());
    const char *type = Response::getMimeType(pathExtension.c_str());
    if (type == NULL) {
        throw 415;
    }
    _headers["content-type"] = type;
    _headers["last-modified"] = formatDate(_pathStat.MTIME);
}

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

void Response::performPOST() {
    if (access(_path.c_str(), F_OK) == 0) {
        throw 403;
    }
    const char *extension = Response::getMimeExtenstion(_request.getHeaderValueForKey("content-type").c_str());
    if (extension == NULL) {
        throw 415;
    }
    if (_path.rfind('.') == std::string::npos) {
        _path = _path + "." + extension;
    }
    std::ofstream file(_path.c_str(), std::ios::binary);
    if (file.is_open()) {
        throw 500;
    }
    file.write(_body.data(), _body.size());
    if (file.fail()) {
        file.close();
        throw 500;
    }
    file.close();
    _status = 201;
    _headers["content-length"] = "0";
    _headers["location"] = _path.substr(_location.getRoot().size());
}

void Response::performDELETE() {
    if (S_ISDIR(_pathStat.st_mode) == false) {
        if (std::remove(_path.c_str()) == -1) {
            throw 500;
        }
    } else {
        if (_path.at(_path.size()- 1) != '/') {
            _headers["location"] = _request.getUri() + "/";
            throw 301;
        }
        if (access(_path.c_str(), W_OK) == -1) {
            throw 403;
        }
        if (rmdir(_path.c_str()) == -1) {
            throw 500;
        }
    }
    _status = 204;
}
