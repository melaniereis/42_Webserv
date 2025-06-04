/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 19:30:10 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/04 20:11:39 by jmeirele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Server.hpp"
#include "../utils/Logger.hpp"

// ==========
// Constructor & Destructor
// ==========

Server::Server(const ServerConfig& config) : _config(config), _serverFd(-1) {}

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
	_pollFds.push_back(pfd);

	while(true)
	{
		if (poll(&_pollFds[0], _pollFds.size(), -1) < 0)
		{
			std::perror("poll");
			break;
		}
	}
}

// ==========
// Private member functions
// ==========

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
