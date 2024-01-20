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

void Response::setCGIEnvironment() {
    // pass header to script via env variables
}

void Response::bodyToFile(int fd) {
    std::vector<char> &reqBody = _request.getBody();
    write(fd, reqBody.data(), reqBody.size());
}

void Response::fileToBody(int fd) {
    char *buf[BUFFER_SIZE] = {NULL};
    ssize_t readBytes;
    while ((readBytes = read(fd, buf, BUFFER_SIZE)) > 0) {
        _body.insert(_body.end(), buf, buf + readBytes);
    }
    if (readBytes == -1) {
        // error reading file 
    }
}

void Response::executeCGI() {
    int cgiInput[2], cgiOutput[2];
    pipe(cgiInput);
    if (_request.getMethod() == POST) {
        bodyToFile(cgiInput[PIPEIN]);
    }
    close(cgiInput[PIPEIN]);
    pipe(cgiOutput);

    pid_t pid = fork();
    if (pid == 0) {
        dup2(cgiInput[PIPEOUT], STDIN_FILENO);
        dup2(cgiOutput[PIPEIN], STDOUT_FILENO);
        close(cgiOutput[PIPEOUT]);
        const char *argv[] = {
            const_cast<char *>(_cgiInterpreter.c_str()),
            const_cast<char *>(_cgiPath.c_str()),
            NULL};
        execve(argv[PIPEOUT], argv, _cgiEnv);
        // error if execve fails
    }

    close(cgiInput[PIPEOUT]);
    close(cgiOutput[PIPEIN]);
    if (_request.getMethod() == GET) {
        fileToBody(cgiOutput[PIPEOUT]);
    }
    close(cgiOutput[PIPEOUT]);

    // int waitStatus;
    // waitpid(pid, &waitStatus, WNOHANG | WUNTRACED ??);
    // WEXITSTATUS(waitStatus);
}

/**
 * @brief Tries to execute requested file if it is one of the following
 * supported cgi: .sh, .py, .perl
 * 
 * @retval true, if file was valid cgi
 * @retval false, else
 */
bool Response::tryCGI() {
    if (isValidCGI() == false) {
        return (false);
    }

    setCGIEnvironment();

    executeCGI();

    return (true);
}
