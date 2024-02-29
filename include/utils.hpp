#pragma once

/* ************************************************************************** */
/*                                  INCLUDES                                  */
/* ************************************************************************** */

#include <string>
#include <vector>
#include <map>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <exception>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <ctime>
// #include <cstdint>
#include <cmath> //
#include <iomanip> //
#include <sys/wait.h> //
#include <cctype>
#include <csignal>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <dirent.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* ************************************************************************** */
/*                                  DEFINES                                   */
/* ************************************************************************** */

#define BUFFER_SIZE 30720
#define MAX_HEADER_SIZE 8175 //8kb
#define KEEPALIVE_TIMEOUT 60
#define DEFAULT_CONFIG "./conf_files/default.conf"
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

#if defined(__APPLE__)
    #define MTIME st_mtimespec.tv_sec
#else
	#define MTIME st_mtime
#endif

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
std::vector<std::string> splitString(const std::string& input, const std::string delim);

// free
void free_2d_array(void **array);
