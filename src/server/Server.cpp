/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 19:30:10 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/07 22:18:31 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "../client/ClientManager.hpp"
#include "../client/Client.hpp"
#include "../utils/Logger.hpp"

Server::Server(const ServerConfig &config) : _serverFd(-1), _config(config) {}

Server::~Server(){}

// ==========
// Server Run
// ==========

void Server::runServer()
{
	if (!setupSocket())
		return ;

	struct pollfd pfd;

	pfd.fd = _serverFd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	_clientManager.getPollFds().push_back(pfd);

	while(true)
	{
		std::vector<struct pollfd> &pollFds = _clientManager.getPollFds();

		int pollResult = poll(pollFds.data(), pollFds.size(), 10);
		if (pollResult < 0)
		{
			std::perror("poll");
			break;
		}
		for (size_t i = 0; i < pollFds.size(); i++)
		{
			int fd = pollFds[i].fd;
			if (fd == _serverFd && (pollFds[i].revents &POLLIN))
				_clientManager.acceptNewClient(_serverFd);
			else if (pollFds[i].revents & (POLLIN | POLLOUT))
				_clientManager.handleClientIO(fd);
			}
		// std::cout << "Num|ber of sockets" << _clientManager.getPollFds().size() << std::endl;
	}
}

bool Server::setupSocket()
{
	if (!createSocket()) return false;
	if (!setSocketOptions()) return false;
	if (!bindSocket()) return false;
	if (!makeSocketNonBlocking()) return false;
	if (!startListening()) return false;

	logListeningMessage();
	return true;
}

bool Server::createSocket()
{
	_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverFd < 0) {
		std::perror("socket");
		return false;
	}
	return true;
}

bool Server::setSocketOptions()
{
	int opt = 1;
	if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		std::perror("setsockopt");
		close(_serverFd);
		return false;
	}
	return true;
}

bool Server::bindSocket()
{
	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(_config.getServerPort());

	if (bind(_serverFd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		std::perror("bind");
		close(_serverFd);
		return false;
	}
	return true;
}

bool Server::makeSocketNonBlocking()
{
	if (fcntl(_serverFd, F_SETFL, O_NONBLOCK) < 0) {
		std::perror("fcntl");
		close(_serverFd);
		return false;
	}
	return true;
}

bool Server::startListening()
{
	if (listen(_serverFd, SOMAXCONN) < 0) {
		std::perror("listen");
		close(_serverFd);
		return false;
	}
	return true;
}

void Server::logListeningMessage()
{
	std::ostringstream oss;
	oss << "🟢 Server is listening on port " << _config.getServerPort();
	Logger::info(oss.str());
}
