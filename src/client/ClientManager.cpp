/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 20:55:24 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/06 16:31:53 by jmeirele         ###   ########.fr       */
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

	if (!client->handleClientRequest()){}
	if (!client->handleClientResponse()){}
}


void ClientManager::acceptNewClient(int serverFd)
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
	Client* client = new Client(clientFd);
	_clients[clientFd] = client;

	// Add pollfd
	struct pollfd pfd;
	pfd.fd = clientFd;
	pfd.events = POLLIN | POLLOUT;
	pfd.revents = 0;
	_pollFds.push_back(pfd);
	std::cout << "Client connected" << std::endl;
}
