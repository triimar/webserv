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
    // process
    void processRequest();
	void performGET();
    void performPOST();
    void performDELETE();
	// cgi
    bool isCGI();
    bool isValidCGI(std::string &path);
    static bool isSupportedCGI(const std::string &extension);
    std::string getCGIIndex();
	// autoindex
	std::string formatModificationTime(time_t modifTime);
	std::string formatSize(off_t size);
	void appendHtmlHead();
	void appendHtmlBodyStart();
	void appendHtmlRow(std::string& subPath, std::string& modTime, std::string& bytes);
	void appendHtmlEnd();
	void makeDirectoryListing();
    // construct
    void constructResponse();
    void makeErrorPage();
    void setHeaders();
    // send
    void send(int socket);
    // utils
    void fileToBody(std::string &path);
    std::string getIndex();

    const Server &_server;
    const Request &_request;
    const Location _location;
    std::map<std::string, std::string> _headers;
    std::vector<char> _body;
    std::vector<char> _response;
    std::string _path;
    bool _pathIsDir;
    int _status;
    std::vector<std::string> _redirectHistory;
    bool _isCGI;
    std::string _cgiPath;
    std::string _cgiExtension;
    std::string _cgiPathInfo;
    std::vector<std::string> _cgiArgv;
    static std::string _supportedCGI;
};
