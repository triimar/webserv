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
#include <netinet/in.h>
#include <arpa/inet.h>
#include <exception>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <unistd.h>
#include <fstream>
#include <dirent.h>
#include <ctime>

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
    PIPEIN,
    PIPEOUT
};

/* ************************************************************************** */
/*                                  DEFINES                                   */
/* ************************************************************************** */

#define BUFFER_SIZE 30720
#define DEFAULT_CONFIG "webserv.conf"
#define SUPPORTED_CGI "sh,py,perl"

#define CGI_TIMEOUT_SEC 42
#define SHEBANG "#!"

#define SSTR(x) static_cast<std::ostringstream &>(\
        (std::ostringstream() << std::dec << x)).str()

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
void appendStringToVector(std::vector<char> &vector, const char *str);
Return readToVector(int fd, std::vector<char> &vec);

// free
void free_2d_array(void **array);
