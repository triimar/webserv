#pragma once

/* ************************************************************************** */
/*                                  INCLUDES                                  */
/* ************************************************************************** */

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <iterator>
#include <sstream>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <exception>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <unistd.h>

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

/* ************************************************************************** */
/*                                   ENUMS                                    */
/* ************************************************************************** */

enum WebservError {
	SUCCESS = 0,
	ERRNO,
	MEMFAIL,
    INVARGS
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
/*                                 FUNCTIONS                                  */
/* ************************************************************************** */

WebservError error(WebservError err, std::string &context);

std::vector<std::string> splitString(const std::string str, char delim);

std::string combinePaths(std::string &lhs, std::string &rhs);
