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

// Define macro for CGI extensions and interpreters
// define a pair separated by a '='
// define as many pairs as you want separated by '&'
// make sure the interpreter exists at the given path
#define SUPPORTED_CGI "sh=/bin/sh&py=/usr/bin/python3&perl=/usr/bin/perl"
typedef std::map<std::string, std::string> CGIList;

#define CGI_TIMEOUT_SEC 42

#define SSTR(x) static_cast<std::ostringstream &>(\
        (std::ostringstream() << std::dec << x)).str()

/* ************************************************************************** */
/*                                 FUNCTIONS                                  */
/* ************************************************************************** */

// error
WebservError ft_perror(WebservError err, const char *context);

// split
std::vector<std::string> splitString(const std::string str, char delim);

// paths
std::string combinePaths(std::string &lhs, std::string &rhs);

// vector
void appendStringToVector(std::vector<char> &vector, const char *str);
Return readToVector(int fd, std::vector<char> &vec);
