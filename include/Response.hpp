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
    // path
    void findPath();
    uint16_t pathExists(std::string &path);
    // cgi
    bool isValidCGI();
    void executeCGI();
    void cgiProcess(int cgiOutput[2]);
    void waitForCGI(pid_t cgi, int output);
    char **getCGIEnvironment();
    // process
    void processRequest();
    uint16_t performGET();
    uint16_t performPOST();
    uint16_t performDELETE();
    // send
    void constructResponse(std::vector &response);

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
