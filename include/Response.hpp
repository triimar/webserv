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
    void checkCGI();
    void cgiProcess(int cgiOutput[2]);
    int waitForCGI(pid_t cgi);
    char **getCGIEnvironment();
    // process
    void processRequest();
    void checkLocation();
    void fileToBody(std::string &path);

	void performGET();
    void performPOST();
    void performDELETE();
	
	// autoindex
	std::string formatModificationTime(time_t modifTime);
	std::string formatSize(off_t size);
	void appendHtmlHead();
	void appendHtmlBodyStart();
	void appendHtmlRow(std::string& subPath, std::string& modTime, std::string& bytes);
	void appendHtmlEnd();
	void makeDirectoryListing();
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
    std::string _cgiExtension;
    std::vector<std::string> _cgiArgv;
    bool _isCGI;
    std::string _path;
    struct stat _pathStat;
    int _status;
    std::map<std::string, std::string> _headers;
    std::vector<char> _body;
    std::vector<char> _response;
};
