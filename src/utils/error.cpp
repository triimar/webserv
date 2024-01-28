#include "../../include/utils.hpp"

using std::cerr;
using std::endl;

// errors only if establishing connection fails - afterwards always response

static void printError(WebservError err, const char *context) {
    switch (err) {
        case MEMFAIL:
            cerr << "Memory allocation failed while " << context;
        case INVARGS:
            cerr << "Too many arguments. Expected 0-1";
        default:;
    }
}

WebservError ft_perror(WebservError err, const char *context) {
    if (err == SUCCESS) {
        return (err);
    }
    cerr << "Error: ";
    if (err == ERRNO) {
        cerr << strerror(errno) << endl;
        return (static_cast<WebservError>(errno));
    }
    printError(err, context);
    cerr << endl;
    return (err);
}
