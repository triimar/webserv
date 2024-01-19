#include "../include/Response.hpp"

Response::Response(const Server &server, const Request &req) :
    _server(server), _request(req) {
    handleUrl();
}

Response::~Response() {}

void Response::handleUrl() {

    // first translate url to local path, something like:
    // localPath_ = config.mountDir_ + url_; // slash in between?

    // save path stats in member variable
    if (stat(path.c_str(), &pathStat_) != 0) {
        // error ???
    }
    if (S_ISDIR(pathStat_.st_mode)) {
        // indexPath_ = localPath_ + config.index // slash in between? test.d for example is also directory!!
    }

}