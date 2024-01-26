#include "../../include/Response.hpp"

bool Response::isValidCGI() {
    if (S_ISDIR(_pathStat.st_mode) == false) {
        _cgiPath = _localPath;
    } else if (_indexPaths.empty() == false) {
        _cgiPath = _indexPaths.front();
    } else {
        return (false); // not a file or directory with index
    }
    size_t ext = _cgiPath.rfind('.');
    if (ext == std::string::npos) {
        return (false); // file without extension
    }
    _cgiInterpreter = _server.getCGIInterpreter(_cgiPath.substr(ext + 1));
    if (_cgiInterpreter.empty()) {
        return (false); // file extension isn't supported
    }
    return (true);
}

void Response::executeCGI() {
    _status = 500; // default fail, gets set back to 0 on success
    int cgiOutput[2];
    if (pipe(cgiOutput) == -1) {
        return ;
    }
    pid_t cgi = fork();
    if (cgi == -1) {
        return ;
    } else if (cgi == 0) {
        cgiProcess(cgiOutput);
    }
    close(cgiOutput[PIPEIN]);
    waitForCGI(cgi, cgiOutput[PIPEOUT]);
    close(cgiOutput[PIPEOUT]);
}

void Response::cgiProcess(int cgiOutput[2]) {
    dup2(cgiOutput[PIPEIN], STDOUT_FILENO);
    close(cgiOutput[PIPEOUT]);
    if (_request.getMethod() == POST) {
        // write request body to cgi stdin
        std::vector<char> &reqBody = _request.getBody();
        if (write(STDIN_FILENO, reqBody.data(), reqBody.size()) == -1) {
            exit(EXIT_FAILURE);
        }
    }
    char **cgiEnv = getCGIEnvironment();
    if (cgiEnv == NULL) {
        exit(EXIT_FAILURE);
    }
    const char *argv[] = {
        const_cast<char *>(_cgiInterpreter.c_str()),
        const_cast<char *>(_cgiPath.c_str()),
        NULL};
    execve(argv[PIPEOUT], argv, cgiEnv);
    exit(EXIT_FAILURE);
}

void Response::waitForCGI(pid_t cgi, int output) {
    size_t time = CGI_TIMEOUT_SEC * 1000;
    int waitStatus;

    while (time > 0) {
        if (waitpid(cgi, &waitStatus, WNOHANG) != 0) {
            break ;
        }
        usleep(100);
        time -= 100;
    }
    if (time == 0
        && kill(cgi, SIGTERM) == 0 && waitpid(cgi, &waitStatus, 0) != -1) {
        _status = 503; // timeout and no fail on the waits
    } else if (WIFEXITED(waitStatus) && WEXITSTATUS(waitStatus) == EXIT_SUCCESS
        && (_request.getMethod() == POST
        || readToVector(output, _response) == RETURN_SUCCESS)) {
        switch (_request.getMethod()) {
        case GET:
            _status = 200;
        case POST:
            _status = 201;
        }
    } // else is fail so status is left unchanged 500
}

char **Response::getCGIEnvironment() {
    // pass header to script via env variables

    // Everything after the ? in the URL appears in the QUERY_STRING environment variable

}
