#pragma once

#include "Location.hpp"
#include "utils.hpp"

class Server;
class Request;

class Response {

public:

    Response(const Server &serv, Request &req);
    ~Response();

    std::vector<char> &getResponse();

private:
    // process
    void processRequest();
    std::string getIndex();
    std::string cleanPath(const std::string &path);
	void performGET();
    void fileToBody(std::string &path);
    void performPOST();
    void performDELETE();
	// cgi
    bool isCGI();
    bool isValidCGI(std::string &path);
    static bool isSupportedCGI(const std::string &extension);
    bool hasCGIIndex();
	void executeCGI();
	void checkCGI();
    void setupPipes(int cgiInput[2], int cgiOutput[2]);
    void cgiProcess(int cgiInputREAD, int cgiOutputWRITE);
	char **getCGIEnvironment();
	int waitForCGI(pid_t cgi);
	void parseCGIOutput();
	void parseCGIHeaders();
	void interpretCGIHeaders();
	Return unchunkCGIBody();
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
    Request &_request;
    Location _location;
    std::map<std::string, std::string> _headers;
    std::vector<char> _body;
    std::vector<char> _response;
    std::string _path;
    struct stat _pathStat;
    bool _isCGI;
    int _status;
    std::vector<std::string> _redirectHistory;
    std::string _cgiPath;
    std::string _cgiExtension;
    std::string _cgiPathInfo;
    std::vector<std::string> _cgiArgv;
    static std::string _supportedCGI;

    struct mimeEntry {
        const char *extension;
        const char *mimeType;
    };
    static Response::mimeEntry _mimeTypes[348];
};

// utils
char **vectorToArray(const std::vector<std::string> &vec);
std::string formatDate(time_t pit);
std::string capitalizeHeader(std::string name);
Return deleteDirectory(const std::string &path);
