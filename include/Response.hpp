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

    std::vector<char> &getResponse();

private:
    // process
    void processRequest();
    std::string getIndex();
    void cleanPath();
	void performGET();
    void fileToBody(std::string &path);
    void performPOST();
    void performDELETE();
	// cgi
    bool isCGI();
    bool isValidCGI(std::string &path);
    static bool isSupportedCGI(const std::string &extension);
    bool hasCGIIndex();
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
    // mime.cpp
    static const char *getMimeType(const char *extension);
    static const char *getMimeExtenstion(const char *type);
    static int mimeStrcmpi(const char *s1, const char *s2);

    const Server &_server;
    const Request &_request;
    const Location _location;
    std::map<std::string, std::string> _headers;
    std::vector<char> _body;
    std::vector<char> _response;
    std::string _path;
    struct stat _pathStat;
    int _status;
    std::vector<std::string> _redirectHistory;
    bool _isCGI;
    std::string _cgiPath;
    std::string _cgiExtension;
    std::string _cgiPathInfo;
    std::vector<std::string> _cgiArgv;
    static std::string _supportedCGI;

    struct mimeEntry {
        const char *extension;
        const char *mimeType;
    };
    static Response::mimeEntry _mimeTypes[347];
};

// utils
char **vectorToArray(const std::vector<std::string> &vec);
std::string formatDate(time_t pit);
void capitalizeHeader(std::string &name);
