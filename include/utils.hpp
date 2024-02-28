#pragma once

/* ************************************************************************** */
/*                                  INCLUDES                                  */
/* ************************************************************************** */

#include <iterator>
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <map>
#include <cctype>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <exception>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <unistd.h>
#include <fstream>
#include <poll.h>
#include <csignal>
//#include "Server.hpp"
#include <dirent.h>
#include <ctime>

/* ************************************************************************** */
/*                                  DEFINES                                   */
/* ************************************************************************** */

#define BUFFER_SIZE 30720
#define DEFAULT_CONFIG "webserv.conf"
#define SERVER_VERSION "webserv/1.0"
#define CGI_VERSION "CGI/1.1"
#define SUPPORTED_CGI "sh,py,perl"
#define CGI_TIMEOUT 3.0
#define SHEBANG "#!"
#define DATE_FORMAT "%a, %d %b %Y %T GMT"
#define DATE_FORMAT_LEN 29
#define REDIRECTION_LIMIT 5

#define CRLF "\r\n"
#define CRLFCRLF "\r\n\r\n"

#define SSTR(x) static_cast<std::ostringstream &>(\
        (std::ostringstream() << std::dec << x)).str()

/* ************************************************************************** */
/*                                   ENUMS                                    */
/* ************************************************************************** */

enum WebservError {
	SUCCESS = 0,
	ERRNO,
	MEMFAIL,
    INVARGS
};

enum RequestMethod {
	GET,
	POST,
	DELETE,
    OTHER
};

enum Return {
	RETURN_SUCCESS,
	RETURN_FAILURE
};

enum Pipe {
    PIPE_READ,
    PIPE_WRITE
};

/* ************************************************************************** */
/*                                 FUNCTIONS                                  */
/* ************************************************************************** */

// error
WebservError ft_perror(WebservError err, const char *context);

// split
std::vector<std::string> splitString(const std::string& input, const std::string delim);

// paths
std::string combinePaths(std::string &lhs, std::string &rhs);

// vector
void appendStringToVector(std::vector<char> &vector, std::string str);
Return readToVector(int fd, std::vector<char> &vec);

// string
void strToLower(std::string& str);
std::string& trimString(std::string& str);
bool containsControlChar(std::string& str);
std::vector<char>::iterator findSubstring(std::vector<char>::iterator begin, std::vector<char>::iterator end, std::string s);

// free
void free_2d_array(void **array);
