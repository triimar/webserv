#include "../../include/webserv.hpp"

Client::Client() : _clientfd(0) , _request(Request(DEFAULT_CLIENT_BODY_SIZE)){}

Client::Client(Server *server) : _server(server) , _request(server->getClientBodySize()){
    if (!_server->acceptConnection()) {
		throw std::runtime_error("Client starting error: client number exceeded in server.");
	}

	struct sockaddr_in clientAddress;
	socklen_t socketLen = sizeof(clientAddress);

	_clientfd = accept(_server->getSocketFd(), (struct sockaddr *)&clientAddress, &socketLen);

	if (_clientfd == -1)
		throw std::runtime_error("Client starting error: failed to connect client.");

	time(&_connectionStart);
	_finishedChunked = true;
	_keepAlive = true;

	return;
}

 Client::Client(const Client &client) : _clientfd(client._clientfd), _server(client._server),
 _request(client._request), _connectionStart(client._connectionStart), _keepAlive(client._keepAlive),
 _finishedChunked(client._finishedChunked){
 	return;
 }

Client::~Client() {}

Client &Client::operator=(Client &client) {
 	if (client._clientfd != _clientfd)
 	{
 		_clientfd = client._clientfd;
 		_server = client._server;
 		_request = client._request;
 		_connectionStart = client._connectionStart;
 		_keepAlive = client._keepAlive;
 		_finishedChunked = client._finishedChunked;
 	}
 	return *this;
}


// Timeout Expiry:
// If there is no activity (no requests from the client) for the duration of the keep-alive timeout, the _server may close the connection to free up resources.
// It's important to note that the keep-alive timeout is often a _server-side configuration, and the _server determines how long it keeps a connection open based on its settings.


//for client after request kas been parsed
void Client::confirmKeepAlive() {
	_keepAlive = _request.isKeepAlive(); //Triin changed the isConnectionClose to iskeepAlive
}
//timeout count needs to be set after connection is established and updated after each response has been sent
//and socket is ready to read.
void Client::updateTime() {
	_connectionStart = time(NULL);
}

bool Client::isTimeout() {
    return (std::difftime(time(NULL), _connectionStart) >= KEEPALIVE_TIMEOUT);
}

void Client::setChunkedUnfinished() {
	_finishedChunked = false;
}

void Client::setChunkedFinished() {
	_finishedChunked = true;
}

void Client::setResponse(std::vector<char> &response) {
	_responseMsg = response;
}

void Client::setActivity(std::string activity) {
    _activity = activity;
}

int &Client::getClientFd() {
	return _clientfd;
}

Server *Client::getServer() {
	return _server;
}

Request &Client::getRequest() {
	return _request;
}

std::vector<char> &Client::getResponse() {
	return _responseMsg;
}

time_t &Client::getConnectionStart() {
	return _connectionStart;
}

bool &Client::getKeepAlive() {
	return _keepAlive;
}

bool &Client::getFinishedChunked() {
	return _finishedChunked;
}

std::string &Client::getActivity() {
    return _activity;
}