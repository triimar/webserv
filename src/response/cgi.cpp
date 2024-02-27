#include "../../include/Response.hpp"

/* ************************************************************************** */
/*                                   CHECK                                    */
/* ************************************************************************** */

bool Response::isCGI() {
    if (_request.getMethod() == DELETE) {
        return (false); //405
    }

    if (isValidCGI(_path)) {
        return (true);
    }

    if (_pathIsDir == false) {
        return (false);
    }

    return (hasCGIIndex());
}

bool Response::isValidCGI(std::string &path) {
    for (size_t ext = path.find('.'); ext != std::string::npos; ext = path.find('.', ext + 1)) {
        size_t end = path.find('/', ext);
        if (end == std::string::npos) {
            end = path.size();
        }
        std::string testPath = path.substr(0, end);
        struct stat dirStat;
        if (stat(testPath.c_str(), &dirStat) != 0) {
            return (false);
        }
        if (S_ISDIR(dirStat.st_mode) == false) {
            std::string extension = path.substr(ext + 1, end - ext - 1);
            if (isSupportedCGI(extension)) {
                return (false);
            }
            if (access(testPath.c_str(), R_OK) != 0) {
                return (false);
            }
            _cgiPath = testPath;
            _cgiExtension = extension;
            _cgiPathInfo = path.substr(end, path.size() - end);
            _pathIsDir = false;
            return (true);
        }
    }
    return (false);
}

bool Response::isSupportedCGI(const std::string &extension) {
    return (supportedCGI.find("," + extension + ",") != std::string::npos);
}

bool Response::hasCGIIndex() {
    std::vector<std::string> indexes = _location.getIndex();
    for (std::vector<std::string>::iterator it = indexes.begin();
        it != indexes.end(); ++it) {
        std::string index = combinePaths(_path, *it);
        if (isValidCGI(index) == true) {
            return (true);
        }
    }
    return (false);
}

/* ************************************************************************** */
/*                                  EXECUTE                                   */
/* ************************************************************************** */


void Response::executeCGI() {
    checkCGI();
    int cgiPipe[2];
    if (pipe(cgiPipe) == -1) {
        throw 500;
    }
    pid_t cgi = fork();
    if (cgi == -1) {
        close(cgiPipe[PIPE_READ]);
        close(cgiPipe[PIPE_WRITE]);
        throw 500;
    } else if (cgi == 0) {
        cgiProcess(cgiPipe);
    }
    close(cgiPipe[PIPE_WRITE]);
    if ((_status = waitForCGI(cgi)) == 0) {
        if (readToVector(cgiPipe[PIPE_READ], _response) == RETURN_FAILURE) {
            _status = 500;
        } else {
            _status = 200;
        }
    }
    close(cgiPipe[PIPE_READ]);
    parseCGIOutput();
}

void Response::checkCGI() {
    if (std::find(_location.getCgiInfo().begin(), _location.getCgiInfo().end(), _cgiExtension) == _location.getCgiInfo().end()) {
        throw 403; // file extension not allowed
    }
    std::ifstream file(_cgiPath);
    if (file.is_open() == false) {
        throw 500; // failed opening file
    }
    std::string line;
    std::getline(file, line);
    file.close();
    if (line.compare(0, std::strlen(SHEBANG), SHEBANG) == std::string::npos) {
        throw 500; // no shebang
    }
    line.erase(0, std::strlen(SHEBANG));
    _cgiArgv = splitString(line, " \t");
    if (access(_cgiArgv.front().c_str(), X_OK) != 0) {
        throw 500; // interpreter invalid or not installed
    }
    _cgiArgv.push_back(_cgiPath);
}

void Response::cgiProcess(int cgiPipe[2]) {
    dup2(cgiPipe[PIPE_WRITE], STDOUT_FILENO);
    close(cgiPipe[PIPE_READ]);
    close(cgiPipe[PIPE_WRITE]);
    if (chdir(_server.getRoot().c_str()) == -1) {
        std::exit(EXIT_FAILURE);
    }
    if (_request.getMethod() == POST) {
        // write request body to cgi stdin
        if (write(STDIN_FILENO, _request.getBody().data(), _request.getBody().size()) == -1) {
            std::exit(EXIT_FAILURE);
        }
    }
    char **argv = vectorToArray(_cgiArgv);
    if (argv == NULL) {
        std::exit(EXIT_FAILURE);
    }
    char **env = getCGIEnvironment();
    if (env != NULL) {
        execve(argv[0], argv, env);
        free_2d_array(static_cast<void**>(env));
    }
    free_2d_array(static_cast<void**>(argv));
    std::exit(EXIT_FAILURE);
}

char **Response::getCGIEnvironment() {
    std::vector<std::string> env;

    env.push_back("GATEWAY_INTERFACE=" + std::string(CGI_VERSION));
    env.push_back("SERVER_SOFTWARE=" + std::string(SERVER_VERSION));
    env.push_back("SERVER_NAME=" + _server.getServerName());
    env.push_back("SERVER_PROTOCOL=" + _request.getHttpVer());
    env.push_back("SERVER_PORT=" + SSTR(_server.getPort()));
    env.push_back("REQUEST_METHOD=" + _request.getMethodStr());
    env.push_back("REQUEST_URI=" + _request.getUri());
    env.push_back("SCRIPT_NAME=" + _cgiPath);
    env.push_back("PATH_INFO=" + _cgiPathInfo); // or _cgiPath ?? -> then _cgiPathInfo not needed at all
    env.push_back("PATH_TRANSLATED=" + _cgiPath);
    env.push_back("DOCUMENT_ROOT=" + _location.getRoot());
    env.push_back("QUERY_STRING=" + _request.getQuery());
    env.push_back("CONTENT_TYPE=" + _request.getHeaderValueForKey("content-type"));
    env.push_back("CONTENT_LENGTH=" + SSTR(_request.getBody().size()));
    env.push_back("REMOTE_ADDR=" + _server.getIpAddr());
    env.push_back("HTTP_FROM=" + _request.getHeaderValueForKey("from"));
    env.push_back("HTTP_ACCEPT" + _request.getHeaderValueForKey("accept"));
    env.push_back("HTTP_USER_AGENT" + _request.getHeaderValueForKey("user-agent"));
    env.push_back("HTTP_REFERER" + _request.getHeaderValueForKey("referer"));

    return (vectorToArray(env));
}

int Response::waitForCGI(pid_t cgi) {
    time_t start = std::time(NULL);
    int waitStatus;

    while (waitpid(cgi, &waitStatus, WNOHANG) != 1) {
        if (difftime(std::time(NULL), start) >= CGI_TIMEOUT) {
            kill(cgi, SIGTERM);
            return (503)
        }
    }
    if (WIFEXITED(waitStatus)) {
        switch (WEXITSTATUS(waitStatus)) {
        case 0: return (0);
        case 2: return (400);
        case 127: return (404);
        case 128: return (400);
        }
    }
    return (500);
}

/* ************************************************************************** */
/*                                   PARSE                                    */
/* ************************************************************************** */

void Response::parseCGIOutput() {
    if (_response.empty()) {
        throw 204;
    }
    std::vector<char>::iterator headerEnd = findSubstring(_response.begin(), _response.end(), "\n\n");
    if (headerEnd == _response.end()) {
        headerEnd = findSubstring(_response.begin(), _response.end(), CRLFCRLF);
        if (headerEnd == _response.end()) {
            throw 500;
        }
    }
    _body.insert(_body.begin(), headerEnd + 4, _response.end());
    _response.erase(headerEnd + 4);
    
    parseCGIHeaders();
    interpretCGIHeaders();

    // erase all but the supported headers (delete _headers.erase() from Status and Transfer-Encoding)
}

void Response::parseCGIHeaders() {
    // parse headers into map -> syntax error 500

}

void Response::interpretCGIHeaders() {
    std::map<std::string, std::string>::iterator searchIt;

    // Status header 
    searchIt = _headers.find("status");
    if (searchIt != _headers.end()) {
        std::istringstream(searchIt->second) >> _status;
        if (_server.getStatusMessage(_status) == NULL) {
            throw 500;
        }
        _headers.erase(it); // remove
    }

    // Location header 
    searchIt = _headers.find("location");
    if (searchIt != _headers.end()) {
        if (searchIt->second.find("://") != std::string::npos) {
            // absolute URI 
            if (_headers.size() == 1) {
                throw 302; // -> client redirect
            } else if (_body.empty() && _status != 302) {
                throw 500; // -> client redirect with body didn't set Status
            }
        } else if (_headers.size() == 1) {
            _redirectHistory.push_back(_request.getUri());
            if (_redirectHistory.size() < REDIRECTION_LIMIT && 
                std::find(_redirectHistory.begin(), uriHistory.end(), searchIt->second) != _redirectHistory.end()) {
                // relative URI -> local redirect
                std::stringstream ss(searchIt->second);
                _request.parseURI(ss);
                _headers.clear();
                _body.clear();
                _response.clear();
                processRequest();
            } else {
                throw 500;
            }
        }
    }

    // Content-Type header 
    if (_body.empty() == false && _headers.find("content-type") == _headers.end()) {
        throw 500;
    }

    // Transfer-Encoding: chunked -> unchunk
    searchIt = map.find("transfer-encoding");
    if (searchIt != map.end() && searchIt->second == "chunked"
        && unchunkCGIBody() == RETURN_FAILURE) {
        throw 500;
    }

    // Content-Length header 
    if (_body.empty() == false) {
        searchIt = _headers.find("content-length");
        if (searchIt == _headers.end()) {
            _headers["content-length"] = SSTR(_body.size());
        } else {
            size_t len;
            std::istringstream(searchIt->second) >> len;
            if (len <= _body.size()) {
                _body.erase(_body.begin(), _body.begin() + len);
            }
        }
    }
}

Return Response::unchunkCGIBody() {
    std::vector<char> temp(_body);
    _body.clear();
    while (true) {
        std::vector<char>::iterator nl = findSubstring(temp.begin(), temp.end(), CRLF);
        if (nl == temp.end()) {
            return (RETURN_FAILURE);
        }
        std::string length(temp.begin(), nl);
        size_t len;
        std::istringstream iss(length);
        iss >> std::hex >> len;
        if (iss.eof() == false || iss.fail() == true) {
            return (RETURN_FAILURE);
        }
        temp.erase(temp.begin(), nl + 2);
        if (temp.empty()) {
            return (RETURN_FAILURE);
        }
        nl = findSubstring(temp.begin(), temp.end(), CRLF);
        if (nl == temp.end()) {
            return (RETURN_FAILURE);
        } else if (len == 0 && nl == temp.begin()) {
            return (RETURN_SUCCESS); // found 0 chunk
        }
        std::string chunk(temp.begin(), nl);
        if (chunk.size() + 2 != len) {
            return (RETURN_FAILURE);
        }
        appendStringToVector(_body, chunk);
        temp.erase(temp.begin(), nl + 2);
        if (temp.empty()) {
            return (RETURN_FAILURE);
        }
    }
}
