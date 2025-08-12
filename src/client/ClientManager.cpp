/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 20:55:24 by jmeirele          #+#    #+#             */
/*   Updated: 2025/08/12 13:22:57 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientManager.hpp"

static std::string intToString(int value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

ClientManager::ClientManager(){}

ClientManager::~ClientManager(){}

int ClientManager::acceptNewClient(int serverFd, const ServerConfig &config)
{
	struct sockaddr_in clientAddr;
	socklen_t clientAddrSize = sizeof(clientAddr);
	int clientFd = accept(serverFd, (struct sockaddr*)&clientAddr, &clientAddrSize);

	if (clientFd < 0) {
		Logger::error("accept() failed.");
		return -1;
	}

	// Set clientFd non-blocking
	int flags = fcntl(clientFd, F_GETFL, 0);
	if (flags < 0) {
		Logger::error("fcntl(F_GETFL) failed.");
		close(clientFd);
		return -1;
	}

	if (fcntl(clientFd, F_SETFL, flags | O_NONBLOCK) < 0) {
		Logger::error("fcntl(F_SETFL) failed.");
		close(clientFd);
		return -1;
	}

	Client* client = new Client(clientFd, clientAddr, config);
	_clients[clientFd] = client;

	Logger::info("Client connected: " + intToString(clientFd));
	return clientFd;
}

bool ClientManager::handleClientIO(int fd, short revents)
{
	if (_clients.find(fd) == _clients.end()) {
		return false;
	}

	Client* client = _clients[fd];

	bool keepConnection = true;

	// Handle POLLIN (data available to read)
	if (revents & POLLIN) {
		if (!client->handleClientRequest()) {
			keepConnection = false; // Mark for closure only if explicitly failed
		}
	}

	// Handle POLLOUT (ready to write)
	if (revents & POLLOUT) {
		if (!client->handleClientResponse()) {
			keepConnection = false; // Mark for closure only if explicitly failed
		}
	}

	// Handle POLLHUP (connection hung up by peer)
	if (revents & POLLHUP) {
		keepConnection = false; // Peer closed connection, mark for closure
	}

	// Handle POLLERR (error on socket)
	if (revents & POLLERR) {
		Logger::warn("Error event on client socket: " + intToString(fd));
		keepConnection = false; // Error condition, mark for closure
	}

	return keepConnection;
}

void ClientManager::removeClient(int fd)
{
	if (_clients.find(fd) != _clients.end()) {
		delete _clients[fd];
		_clients.erase(fd);
		close(fd);
		Logger::info("Client disconnected: " + intToString(fd));
	}
}

Client *ClientManager::getClient(int fd) const
{
	std::map<int, Client *>::const_iterator it = _clients.find(fd);
	if (it != _clients.end()) {
		return it->second;
	}
	return NULL;
}

void ClientManager::cleanup()
{
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		delete it->second;
		close(it->first);
	}
	_clients.clear();
	Logger::info("All clients cleaned up.");
}
