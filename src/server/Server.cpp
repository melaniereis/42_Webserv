/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 19:30:10 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/13 22:16:33 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
/* ... existing code ... */
Server::Server(const ServerConfig &config) : _config(config) {}

Server::~Server()
{
	for (std::vector<int>::iterator it = _serverFds.begin();
		it != _serverFds.end(); ++it)
	{
		if (*it > 0) {
			close(*it);
		}
	}
}

bool Server::setup()
{
	const std::map<std::string, ListenConfig>& listens = _config.getListens();

	if (listens.empty()) {
		Logger::error("No listen directives in server configuration");
		return false;
	}

	for (std::map<std::string, ListenConfig>::const_iterator it = listens.begin();
		it != listens.end(); ++it)
	{
		const std::string& ip = it->second.getIp();
		int port = it->second.getPort();

		int fd = _createSocket();
		if (fd < 0) {
			return false;
		}

		if (!_setSocketOptions(fd) ||
			!_bindSocket(fd, ip, port) ||
			!_makeSocketNonBlocking(fd) ||
			!_startListening(fd))
		{
			close(fd);
			return false;
		}

		_logListeningMessage(ip, port);
		_serverFds.push_back(fd);
	}

	return true;
}

int Server::acceptNewConnection(int serverFd)
{
	return _clientManager.acceptNewClient(serverFd, _config);
}

bool Server::handleClientEvent(int clientFd, short revents)
{
	return _clientManager.handleClientIO(clientFd, revents);
}

const std::vector<int>& Server::getServerFds() const
{
	return _serverFds;
}

void Server::removeClient(int fd)
{
	_clientManager.removeClient(fd);
}

int Server::_createSocket() const
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		int err = errno;
		Logger::error(std::string("socket() failed: ") + strerror(err));
	}
	return fd;
}

bool Server::_setSocketOptions(int fd) const
{
	int opt = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		int err = errno;
		Logger::error(std::string("setsockopt() failed: ") + strerror(err));
		return false;
	}
	return true;
}

bool Server::_bindSocket(int fd, const std::string& ip, int port) const
{
	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	if (inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0) {
		Logger::error("Invalid IP address: " + ip);
		return false;
	}

	if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		int err = errno;
		std::ostringstream oss;
		oss << "bind() failed for " << ip << ":" << port
			<< " - " << strerror(err);
		Logger::error(oss.str());
		return false;
	}
	return true;
}

bool Server::_makeSocketNonBlocking(int fd) const
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags < 0) {
		int err = errno;
		Logger::error(std::string("fcntl(F_GETFL) failed: ") + strerror(err));
		return false;
	}
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
		int err = errno;
		Logger::error(std::string("fcntl(F_SETFL) failed: ") + strerror(err));
		return false;
	}
	return true;
}

bool Server::_startListening(int fd) const
{
	if (listen(fd, SOMAXCONN) < 0) {
		int err = errno;
		Logger::error(std::string("listen() failed: ") + strerror(err));
		return false;
	}
	return true;
}

void Server::_logListeningMessage(const std::string& ip, int port) const
{
	std::ostringstream oss;
	oss << "Listening on " << ip << ":" << port;
	Logger::info(oss.str());
}

std::string Server::_intToString(int value) const
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}
