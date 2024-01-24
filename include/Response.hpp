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

/*

Just construct response with server and request & then call send()

        Response response(*this, request);
        response.send();

*/


class Response {

public:

    Response(const Server &serv, const Request &req);
    ~Response();

    // send
    void send();

private:
    // cgi
    bool isValidCGI();
    void executeCGI();
    bool setCGIEnvironment();
    // process
    void processRequest();
    // utils
    void findPath();

    const Server &_server;
    const Request &_request;
    std::string _cgiPath;
    std::string _cgiInterpreter;
    char **_cgiEnv;
    std::string _localPath;
    std::vector<std::string> _indexPaths;
    struct stat _pathStat;
    uint16_t _status;
    std::map<std::string, std::string> _headers;
    std::vector<char> _body;
    std::vector<char> _response;
};
