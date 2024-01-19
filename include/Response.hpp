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

    void Response::handleUrl();

    const Server _server;
    const Request _request;
    std::string _localPath;
    std::string _indexPath;
    struct stat _pathStat;

};
