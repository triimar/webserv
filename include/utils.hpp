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
#include <unistd.h>

/* ************************************************************************** */
/*                                   ENUMS                                    */
/* ************************************************************************** */

enum Return
{
	RETURN_SUCCESS,
	RETURN_FAILURE
};

enum WebservError
{
	SUCCESS = 0,
	ERRNO,
	MEMFAIL,
    INVARGS
};

/* ************************************************************************** */
/*                                 FUNCTIONS                                  */
/* ************************************************************************** */

WebservError error(WebservError err, std::string &context);

std::vector<std::string> splitString(const std::string str, char delim);
