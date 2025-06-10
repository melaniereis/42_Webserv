/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 19:30:10 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/10 21:53:15 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "../client/ClientManager.hpp"
#include "../client/Client.hpp"
#include "../utils/Logger.hpp"

Server::Server(const ServerConfig &config) : _config(config) {}

Server::~Server(){}

void Server::runServer()
{
	//Create all server sockets
	if (!setupSocket())
		return;

	std::vector<struct pollfd> &pollFds = _clientManager.getPollFds();

	// Add server sockets to poll
	for (size_t i = 0; i < _serverFds.size(); ++i)
	{
		struct pollfd pfd;
		pfd.fd = _serverFds[i];
		pfd.events = POLLIN;
		pfd.revents = 0;
		pollFds.push_back(pfd);
		_serverSocketSet.insert(_serverFds[i]);
	}

	while(true)
	{
		int pollResult = poll(pollFds.data(), pollFds.size(), -1);
		if (pollResult < 0)
		{
			std::perror("poll");
			break;
		}

		// Temporary vector to avoid modifying while iterating
		std::vector<struct pollfd> tempFds = pollFds;

		for (size_t i = 0; i < tempFds.size(); i++)
		{
			struct pollfd &pfd = tempFds[i];
			if (pfd.revents == 0) continue;

			if (_serverSocketSet.find(pfd.fd) != _serverSocketSet.end())
			{
				if (pfd.revents & POLLIN)
					_clientManager.acceptNewClient(pfd.fd, _config);
			}
			else if (pfd.revents & (POLLIN | POLLOUT))
			{
				_clientManager.handleClientIO(pfd.fd);
			}

			if (pfd.revents & POLLHUP)
			{
				_clientManager.removeClient(pfd.fd);
			}
		}
	}
}

bool Server::setupSocket()
{
	const std::map<std::string, ListenConfig>& listens = _config.getListens();
	for (std::map<std::string, ListenConfig>::const_iterator listen = listens.begin(); listen != listens.end(); ++listen)
	{
		const std::string& ip = listen->second.getIp();
		int port = listen->second.getPort();

		int fd = createSocket(ip, port);
		if (fd < 0)
			return false;

		if (!setSocketOptions(fd) || !bindSocket(fd, ip, port) ||
			!makeSocketNonBlocking(fd) || !startListening(fd))
		{
			close(fd);
			return false;
		}

		logListeningMessage(ip, port);
		_serverFds.push_back(fd);
		_serverSocketSet.insert(fd);
	}

	return true;
}

int Server::createSocket(const std::string& ip, int port)
{
	(void)ip; (void)port;
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
	{
		std::perror("socket");
		return -1;
	}
	return fd;
}

bool Server::setSocketOptions(int fd)
{
	int opt = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		std::perror("setsockopt");
		close(fd);
		return false;
	}
	return true;
}

bool Server::bindSocket(int fd, const std::string& ip, int port)
{
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);

	if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		std::perror("bind");
		close(fd);
		return false;
	}
	return true;
}

bool Server::makeSocketNonBlocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags < 0) {
		std::perror("fcntl");
		close(fd);
		return false;
	}
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
		std::perror("fcntl");
		close(fd);
		return false;
	}
	return true;
}

bool Server::startListening(int fd)
{
	if (listen(fd, SOMAXCONN) < 0) {
		std::perror("listen");
		close(fd);
		return false;
	}
	return true;
}
void Server::logListeningMessage(const std::string& ip, int port)
{
	std::ostringstream oss;
	oss << "Server listening on " << ip << ":" << port;
	Logger::info(oss.str());
}
