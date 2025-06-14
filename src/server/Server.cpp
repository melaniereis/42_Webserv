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

Server::Server(const ServerConfig &config) : _config(config) {}

Server::~Server()
{
	// Close all server sockets
	for (std::vector<int>::iterator it = _serverFds.begin();
		it != _serverFds.end(); ++it)
	{
		if (*it > 0) {
			close(*it);
		}
	}
}

void Server::runServer()
{
	if (!_setupSocket())
	{
		Logger::error("Failed to setup server sockets");
		return;
	}

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

	Logger::info("Server started, waiting for connections...");

	while (true)
	{
		int pollResult = poll(pollFds.data(), pollFds.size(), -1);

		if (pollResult < 0) {
			if (errno == EINTR) {
				continue; // Interrupted system call, try again
			}
			Logger::error("poll() failed: " + std::string(strerror(errno)));
			break;
		}

		if (pollResult == 0) {
			continue; // Timeout, no events
		}

		// Temporary vector to avoid modifying while iterating
		std::vector<struct pollfd> tempFds = pollFds;
		_handlePollEvents(tempFds);
	}
}

bool Server::_setupSocket()
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
		_serverSocketSet.insert(fd);
	}

	return true;
}

int Server::_createSocket() const
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
	{
		Logger::error("socket() failed: " + std::string(strerror(errno)));
	}
	return fd;
}

bool Server::_setSocketOptions(int fd) const
{
	int opt = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		Logger::error("setsockopt() failed: " + std::string(strerror(errno)));
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

	if (inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0)
	{
		Logger::error("inet_pton() failed for IP: " + ip);
		return false;
	}

	if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		std::ostringstream oss;
		oss << "bind() failed for " << ip << ":" << port
			<< " - " << strerror(errno);
		Logger::error(oss.str());
		return false;
	}
	return true;
}

bool Server::_makeSocketNonBlocking(int fd) const
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags < 0)
	{
		Logger::error("fcntl(F_GETFL) failed: " + std::string(strerror(errno)));
		return false;
	}
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0)
	{
		Logger::error("fcntl(F_SETFL) failed: " + std::string(strerror(errno)));
		return false;
	}
	return true;
}

bool Server::_startListening(int fd) const
{
	if (listen(fd, SOMAXCONN) < 0)
	{
		Logger::error("listen() failed: " + std::string(strerror(errno)));
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

void Server::_handlePollEvents(std::vector<struct pollfd>& tempFds)
{
	for (size_t i = 0; i < tempFds.size(); i++)
	{
		struct pollfd& pfd = tempFds[i];
		if (pfd.revents == 0) continue;

		if (_serverSocketSet.find(pfd.fd) != _serverSocketSet.end()) {
			_acceptNewConnection(pfd);
		} else {
			_handleClientIO(pfd);
		}

		if (pfd.revents & POLLHUP) {
			_cleanupDisconnectedClient(pfd);
		}
	}
}

void Server::_acceptNewConnection(struct pollfd& pfd)
{
	if (pfd.revents & POLLIN)
	{
		_clientManager.acceptNewClient(pfd.fd, _config);
	} else {
		Logger::warn("Unexpected poll event on server socket: " +
					_intToString(pfd.revents));
	}
}

void Server::_handleClientIO(struct pollfd& pfd)
{
	if (pfd.revents & (POLLIN | POLLOUT)) {
		_clientManager.handleClientIO(pfd.fd);
	} else if (pfd.revents & POLLERR) {
		Logger::warn("Error event on client socket: " + _intToString(pfd.fd));
		_clientManager.removeClient(pfd.fd);
	} else{
		Logger::warn("Unexpected poll event on client socket: " +
					_intToString(pfd.revents));
	}
}

void Server::_cleanupDisconnectedClient(struct pollfd& pfd)
{
	Logger::info("Client disconnected: " + _intToString(pfd.fd));
	_clientManager.removeClient(pfd.fd);
}

// Helper function to convert integers to strings
std::string Server::_intToString(int value) const
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}
