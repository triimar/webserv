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
#define DEFAULT_CONFIG "conf_files/default.conf"

// Define macro for CGI extensions and interpreters
// define a pair separated by a '='
// define as many pairs as you want separated by '&'
// make sure the interpreter exists at the given path
#define SUPPORTED_CGI "sh=/bin/sh&py=/usr/bin/python3&perl=/usr/bin/perl"
typedef std::map<std::string, std::string> CGIList;

#define CGI_TIMEOUT_SEC 42

#define SSTR(x) static_cast<std::ostringstream &>(\
        (std::ostringstream() << std::dec << x)).str()

#define CRLF "\r\n"
#define CRLFCRLF "\r\n\r\n"

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

// string
void strToLower(std::string& str);
std::string& trimString(std::string& str);
bool containsControlChar(std::string& str);
