#pragma once

/* ************************************************************************** */
/*                                  INCLUDE                                   */
/* ************************************************************************** */

#include "Server.hpp"
#include "Request.hpp"
#include "utils.hpp"

/* ************************************************************************** */
/*                                   CLASS                                    */
/* ************************************************************************** */

class Response {

public:

    Response(const Server &server, const Request &req);
    ~Response();

private:

    void handleUrl();
    bool tryCGI();
    bool isValidCGI();

    const Server _server;
    const Request _request;
    std::string _cgiPath;
    std::string _cgiInterpreter;
    char        **_cgiEnv;
    std::string _localPath;
    std::vector<std::string> _indexPaths;
    struct stat _pathStat;
    uint8_t _status;
    std::vector<char> _body;
    char *_buffer;
};
