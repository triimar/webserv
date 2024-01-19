#include "../../include/Server.hpp"

/**
 * @brief Takes request and executes the file if it is one of the following
 * supported cgi: .sh, .py, .perl
 * 
 * @param req Request to be handled
 * @retval true, if file was valid cgi
 * @retval false, else
 */
bool Server::tryCGI(Request &req) {
    std::string cgi;

    if (S_ISDIR(req.pathStat_.st_mode) == false) {
        cgi = req.localPath_;
    } else if (req.indexPath_.empty() == false) {
        cgi = req.indexPath_;
    } else {
        return (false);
    }

    std::string cgiExt = cgi.substr(cgi.find_last_of('.'));
    CGIList::iterator it = supportedCGI.find(cgiExt);
    if (it == supportedCGI.end()) {
        return (false); // requested file is not a supported cgi
    }

    // fork, redirect output, pass header to script via env variables? prepare cgi request

    
    pid_t pid = fork();
    if (pid == 0) {
        const char *argv[] = {
            const_cast<char *>(it->second.c_str()),
            const_cast<char *>(cgi.c_str()),
            NULL};
        execve(argv[0], argv, env);
        // error if execve fails
    }
    // int waitStatus;
    // waitpid(pid, &waitStatus, WNOHANG | WUNTRACED ??);
    // WEXITSTATUS(waitStatus);

// output of script is cgi response -> convert to client response body


    return (true);
}
