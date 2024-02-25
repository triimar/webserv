#include "../../include/Client.hpp"

Client::Client() : clientfd(0){
	return;
}

Client::Client(Server *server) {
	struct sockaddr clientAddress;
	socklen_t socketLen = sizeof(clientAddress);

	clientfd = accept(server->getSocketFd(), &clientAddress, &socketLen);

	if (clientfd <= 0)
		throw std::runtime_error("Client starting error: failed to connect client.\n");

	this->server = server;
	time(&connectionStart);
	this->isActive = true;
	this->keepAlive = true;
	return;
}

Client::Client(const Client &client) : clientfd(client.clientfd), server(client.server),
request(client.request), connectionStart(client.connectionStart), keepAlive(client.keepAlive),
isActive(client.isActive){
	return;
}

Client::~Client() {
	close(this->clientfd);
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