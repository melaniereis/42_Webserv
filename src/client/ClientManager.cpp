/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 20:55:24 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/11 22:42:22 by jmeirele         ###   ########.fr       */
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
		int err = errno;
		if (err != EAGAIN && err != EWOULDBLOCK) {
			Logger::error(std::string("accept() failed: ") + strerror(err));
		}
		return -1;
	}

	// Set clientFd non-blocking
	int flags = fcntl(clientFd, F_GETFL, 0);
	if (flags < 0) {
		int err = errno;
		Logger::error(std::string("fcntl(F_GETFL) failed: ") + strerror(err));
		close(clientFd);
		return -1;
	}

	if (fcntl(clientFd, F_SETFL, flags | O_NONBLOCK) < 0) {
		int err = errno;
		Logger::error(std::string("fcntl(F_SETFL) failed: ") + strerror(err));
		close(clientFd);
		return -1;
	}

	Client* client = new Client(clientFd, config);
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

	if (revents & POLLIN) {
		if (!client->handleClientRequest()) {
			return false;
		}
	}

	if (revents & (POLLOUT | POLLHUP)) {
		if (!client->handleClientResponse()) {
			return false;
		}
	}

	if (revents & POLLERR) {
		Logger::warn("Error event on client socket: " + intToString(fd));
		return false;
	}

	return true;
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
