#include "../include/Response.hpp"

Response::Response(const Server &server, const Request &req) :
    _server(server), _request(req) {
    handleUrl();
}

Response::~Response() {}

void Response::handleUrl() {
    _localPath = combinePaths(_server.getRoot(), _request.getUrl);
    if (stat(_localPath.c_str(), &_pathStat) != 0) {
        // throw error for response
    }

    // assuming that if it is a directory it takes the first index that it finds as per config and is valid with the request

    // check auto index / indexing on?

    if (S_ISDIR(_pathStat.st_mode)) {
        for (std::vector<std::string>::iterator it = _server.getIndex.begin();
            it != _server.getIndex.end(); ++it) {
            _indexPath = combinePaths(_localPath, *it);
            // check read access if GET request
            if (access(_indexPath.c_str(), F_OK) == 0) {
                return ;
            }
        }
    }
    // didn't find an existing index
    _indexPath = "";
}
