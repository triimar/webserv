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
    bool isCGI();
    bool isValidCGI(std::string &path);
    void executeCGI();
    void cgiProcess(int cgiOutput[2]);
    uint16_t waitForCGI(pid_t cgi);
    char **getCGIEnvironment();
    // process
    uint16_t processRequest();
    uint16_t checkRequest();
    uint16_t fileToBody(std::string &path);

	std::string formatModificationTime(time_t modifTime);
	std::string formatSize(off_t size);
	void appendHtmlHead();
	void appendHtmlRow(std::string& subPath, std::string& modTime, std::string& bytes);
	void makeDirectoryListing(std::string& path);
    
	uint16_t performGET();
    uint16_t performPOST();
    uint16_t performDELETE();
    // index
    std::string getIndex();
    std::string getCGIIndex();
    // send
    void constructResponse();
    void makeErrorPage();
    void setHeaders();

    const Server &_server;
    const Request &_request;
    const Location _location;
    std::string _cgiPath;
    std::string _cgiInterpreter;
    bool _isCGI;
    char **_cgiEnv;
    std::string _path;
    struct stat _pathStat;
    int _status;
    std::map<std::string, std::string> _headers;
    std::vector<char> _body;
    std::vector<char> _response;
};
