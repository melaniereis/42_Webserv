/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 20:55:24 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/10 21:41:44 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientManager.hpp"

ClientManager::ClientManager(){}

ClientManager::~ClientManager(){}

std::vector<struct pollfd> &ClientManager::getPollFds() { return _pollFds ;}

void ClientManager::handleClientIO(int fd)
{
	Client* client = _clients[fd];
	if (!client) return;

	if (!client->handleClientRequest()) {
		removeClient(fd);
		return;
	}

	if (!client->handleClientResponse()) {
		removeClient(fd);
	}
}


void ClientManager::acceptNewClient(int serverFd, const ServerConfig &config)
{
	int clientFd = accept(serverFd, NULL, NULL);
	if (clientFd < 0)
	{
		perror("accept failed");
		return;
	}

	// Set clientFd non-blocking
	fcntl(clientFd, F_SETFL, O_NONBLOCK);

	// Create new Client object
	Client* client = new Client(clientFd, config);
	_clients[clientFd] = client;

	// Add pollfd
	struct pollfd pfd;
	pfd.fd = clientFd;
	pfd.events = POLLIN | POLLOUT;
	pfd.revents = 0;
	_pollFds.push_back(pfd);
	std::cout << "Client connected" << std::endl;
}

void ClientManager::removeClient(int fd)
{
	if (_clients.find(fd) != _clients.end())
	{
		delete _clients[fd];
		_clients.erase(fd);
		// Remove from pollfds
		for (size_t i = 0; i < _pollFds.size(); ++i)
		{
			if (_pollFds[i].fd == fd)
			{
				_pollFds.erase(_pollFds.begin() + i);
				break;
			}
		}
	}
}
