#include "../../include/Response.hpp"

bool Response::isValidCGI(std::string &path) {
    size_t ext = path.rfind('.');
    if (ext == std::string::npos) {
        return (false); // file without extension
    }
    std::string extension = path.substr(ext + 1);
    _cgiInterpreter = _server.getCGIInterpreter(extension);
    if (_cgiInterpreter.empty()) {
        return (false); // file extension isn't supported
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
    _status = waitForCGI(cgi);
    readToVector(cgiOutput[PIPEOUT], _response);
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
    char **cgiEnv = getCGIEnvironment();
    if (cgiEnv == NULL) {
        exit(EXIT_FAILURE);
    }
    char *argv[3] = {
        const_cast<char *>(_cgiInterpreter.c_str()),
        const_cast<char *>(_cgiPath.c_str()),
        NULL
    };
    execve(argv[0], argv, cgiEnv);
    exit(EXIT_FAILURE);
}

uint16_t Response::waitForCGI(pid_t cgi) {
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
        case 0: return (200);
        case 2: return (400);
        case 127: return (404);
        case 128: return (400);
        }
    }
    return (500);
}

char **Response::getCGIEnvironment() {
    // pass header to script via env variables

    // Everything after the ? in the URL appears in the QUERY_STRING environment variable
    return (NULL);
}
