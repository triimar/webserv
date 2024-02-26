#define KEEPALIVE_TIMEOUT 60
#include "../../include/Client.hpp"

Client::Client() : clientfd(0){
	return;
}

Client::Client(Server *server) {
	struct sockaddr_in clientAddress;
	socklen_t socketLen = sizeof(clientAddress);

	clientfd = accept(server->getSocketFd(), (struct sockaddr *)&clientAddress, &socketLen);

	if (clientfd == -1)
		throw std::runtime_error("Client starting error: failed to connect client.\n");

	this->server = server;
	time(&connectionStart);
	this->isActive = true;
	this->keepAlive = true;


	std::cout << "Client Connected!\n";
	return;
}

Client::Client(const Client &client) : clientfd(client.clientfd), server(client.server),
request(client.request), connectionStart(client.connectionStart), keepAlive(client.keepAlive),
isActive(client.isActive){
	return;
}

Client::~Client() {
//	close(this->clientfd);
	return;
}

Client &Client::operator=(Client &client) {
	if (client.clientfd != this->clientfd)
	{
		this->clientfd = client.clientfd;
		this->server = client.server;
		this->request = client.request;
		this->connectionStart = client.connectionStart;
		this->keepAlive = client.keepAlive;
		this->isActive = client.isActive;
	}
	return *this;
}


// Timeout Expiry:
// If there is no activity (no requests from the client) for the duration of the keep-alive timeout, the server may close the connection to free up resources.
// It's important to note that the keep-alive timeout is often a server-side configuration, and the server determines how long it keeps a connection open based on its settings.


//for client after request kas been parsed
void Client::confirmKeepAlive() {
	keepAlive = request.isKeepAlive(); //Triin changed the isConnectionClose to iskeepAlive
}
//timeout count needs to be set after connection is established and updated after each response has been sent
//and socket is ready to read.
void Client::updateTime() {
	time(&connectionStart);
}

bool Client::isTimeout() {
	time_t current;

	time(&current);
	if (current - this->connectionStart >= KEEPALIVE_TIMEOUT)
		return true;
	return false;
}

int &Client::getClientFd() {
	return this->clientfd;
}

Server *Client::getServer() {
	return this->server;
}

Request &Client::getRequest() {
	return this->request;
}

time_t Client::getConnectionStart() {
	return this->connectionStart;
}

bool Client::getKeepAlive() {
	return this->keepAlive;
}

bool Client::getIsActive() {
	return this->isActive;
}