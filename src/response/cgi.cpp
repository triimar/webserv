#include "../../include/Response.hpp"

static bool isSupportedCGI(const std::string &extension) {
    std::string supported = SUPPORTED_CGI;
    supported.insert(supported.begin(), ',');
    supported.insert(supported.end(), ',');
    return (supported.find("," + extension + ",") != std::string::npos);
}

bool Response::isValidCGI(std::string &path) {
    if (_request.getMethod() == DELETE) {
        return (false); //405
    }
    size_t ext = path.rfind('.');
    if (ext == std::string::npos) {
        return (false); // file without extension
    }
    _cgiExtension = path.substr(ext + 1);
    if (isSupportedCGI(_cgiExtension)) {
        return (false); // file extension not supported
    }
    return (access(path.c_str(), R_OK) == 0);
}

bool Response::isCGI() {
    _cgiPath = _path;
    if (S_ISDIR(_pathStat.st_mode) == false) {
        return (isValidCGI(_path));
    }
    _cgiPath = getCGIIndex();
    return (_cgiPath.empty() == false);
}

void Response::checkCGI() {
    if (std::find(_location.getCgiInfo().begin(), _location.getCgiInfo().end(), _cgiExtension) == _location.getCgiInfo().end()) {
        throw 403; // file extension not allowed
    }
    std::string shebang;
    std::ifstream file(_cgiPath);
    if (file.is_open() == false) {
        throw 500; // failed opening file
    }
    std::getline(file, shebang);
    file.close();
    if (shebang.compare(0, 2, SHEBANG) == std::string::npos) {
        throw 500; // no shebang
    }
    shebang.erase(0, 2);
    _cgiArgv = splitString(shebang, " \t");
    if (_cgiArgv.front().front() != '/' || _cgiArgv.front().size() < 2
        || access(_cgiArgv.front().c_str(), X_OK) != 0) {
        throw 500; // invalid interpreter
    }
    _cgiArgv.push_back(_cgiPath);
}

void Response::executeCGI() {
    _isCGI = true;
    checkCGI();
    int cgiOutput[2];
    if (pipe(cgiOutput) == -1) {
        throw 500;
    }
    pid_t cgi = fork();
    if (cgi == -1) {
        throw 500;
    } else if (cgi == 0) {
        cgiProcess(cgiOutput);
    }
    close(cgiOutput[PIPEIN]);
    if ((_status = waitForCGI(cgi)) == 0) {
        if (readToVector(cgiOutput[PIPEOUT], _response) == RETURN_FAILURE) {
            _status = 500;
        } else if (request.getMethod() == POST) {
            _status = 201;
        } else {
            _status = 200;
        }
    }
    close(cgiOutput[PIPEOUT]);
}

void Response::cgiProcess(int cgiOutput[2]) {
    if (chdir(_server.getRoot().c_str()) == -1) {
        exit(EXIT_FAILURE);
    }
    dup2(cgiOutput[PIPEIN], STDOUT_FILENO);
    close(cgiOutput[PIPEOUT]);
    if (_request.getMethod() == POST) {
        // write request body to cgi stdin
        if (write(STDIN_FILENO, _request.getBody().data(), _request.getBody().size()) == -1) {
            exit(EXIT_FAILURE);
        }
    }
    char **argv = vectorToArray(_cgiArgv);
    if (argv == NULL) {
        exit(EXIT_FAILURE);
    }
    char **env = getCGIEnvironment();
    if (env == NULL) {
        free_2d_array(static_cast<void**>(argv));
        exit(EXIT_FAILURE);
    }
    execve(argv[0], argv, env);
    free_2d_array(static_cast<void**>(argv));
    free_2d_array(static_cast<void**>(env));
    exit(EXIT_FAILURE);
}

int Response::waitForCGI(pid_t cgi) {
    size_t time = CGI_TIMEOUT_SEC * 1000;
    int waitStatus;

    while (time > 0) {
        if (waitpid(cgi, &waitStatus, WNOHANG) != 0) {
            break ;
        }
        usleep(100);
        time -= 100;
    }
    if (time == 0 && kill(cgi, SIGTERM) == 0
        && waitpid(cgi, &waitStatus, 0) != -1) {
        return (503); // timeout and no fail on the waits
    } else if (WIFEXITED(waitStatus)) {
        switch (WEXITSTATUS(waitStatus)) {
        case 0: return (0);
        case 2: return (400);
        case 127: return (404);
        case 128: return (400);
        default: return (500);
        }
    }
}

char **vectorToArray(const std::vector<std::string> &vec) {
    if (vec.empty()) {
        return (NULL);
    }
    char **res = new char*[vec.size() + 1];
    size_t i = 0;
    for (; i < vec.size(); ++i) {
        res[i] = new char[vec[i].size() + 1];
        std::strcpy(res[i], vec[i].c_str());
    }
    res[i] = NULL;
    return (res);
}

char **Response::getCGIEnvironment() {
    std::vector<std::string> temp;

    temp.push_back("GATEWAY_INTERFACE=webserv_cgi");
    temp.push_back("SERVER_NAME=" + _server.getServerName());
    temp.push_back("SERVER_SOFTWARE=webserv_1.0");
    temp.push_back("SERVER_PROTOCOL=" + _request.getHttpVer());
    temp.push_back("SERVER_PORT=" + SSTR(_server.getPort()));
    temp.push_back("REQUEST_METHOD=" + getMethodStr());
    temp.push_back("PATH_INFO");
    temp.push_back("PATH_TRANSLATED");
    temp.push_back("SCRIPT_NAME=" + _request.getPath());
    temp.push_back("DOCUMENT_ROOT=" + _location.getRoot());
    temp.push_back("QUERY_STRING=" + _request.getQuery());
    temp.push_back("CONTENT_TYPE=" + _request.getHeaderValueForKey("Content-Type"));
    temp.push_back("CONTENT_LENGTH=" + SSTR(_request.getBody().size()));
    temp.push_back("REMOTE_HOST");
    temp.push_back("REMOTE_ADDR");
    temp.push_back("AUTH_TYPE");
    temp.push_back("REMOTE_USER=" + _request.getHeaderValueForKey("Proxy-Authenticate"));
    temp.push_back("REMOTE_IDENT");
    temp.push_back("HTTP_FROM=" + _request.getHeaderValueForKey("From"));
    temp.push_back("HTTP_ACCEPT" + _request.getHeaderValueForKey("Accept"));
    temp.push_back("HTTP_USER_AGENT" + _request.getHeaderValueForKey("User-Agent"));
    temp.push_back("HTTP_REFERER" + _request.getHeaderValueForKey("Referer"));

    return (vectorToArray(temp));
}
