/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 19:30:10 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/24 16:59:34 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(const ServerConfig &config) : config(config) {}

Server::~Server()
{
	for (std::vector<int>::iterator it = serverFds.begin();
		it != serverFds.end(); ++it)
	{
		if (*it > 0) close(*it);
	}
}

bool Server::setup()
{
	const std::map<std::string, ListenConfig>& listens = config.getListens();

	if (listens.empty())
	{
		Logger::error("No listen directives in server configuration");
		return false;
	}

	for (std::map<std::string, ListenConfig>::const_iterator it = listens.begin();
		it != listens.end(); ++it)
	{
		const std::string& ip = it->second.getIp();
		int port = it->second.getPort();

		if (!setupSocketForListen(ip, port))
			return false;
	}
	return true;
}

bool Server::setupSocketForListen(const std::string& ip, int port)
{
	int fd = createSocket();
	if (fd < 0) return false;

	if (!setSocketOptions(fd) ||
		!bindSocket(fd, ip, port) ||
		!makeSocketNonBlocking(fd) ||
		!startListening(fd))
	{
		close(fd);
		return false;
	}

	logListeningMessage(ip, port);
	logSocketInfo(fd);
	serverFds.push_back(fd);
	return true;
}

int Server::acceptNewConnection(int serverFd)
{
	return clientManager.acceptNewClient(serverFd, config);
}

bool Server::handleClientEvent(int clientFd, short revents)
{
	return clientManager.handleClientIO(clientFd, revents);
}

const std::vector<int>& Server::getServerFds() const
{
	return serverFds;
}

void Server::removeClient(int fd)
{
	clientManager.removeClient(fd);
}

int Server::createSocket() const
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
		Logger::error("socket() failed: " + std::string(strerror(errno)));
	else
		Logger::info("Created socket FD: " + intToString(fd));
	return fd;
}

bool Server::setSocketOptions(int fd) const
{
	int opt = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		Logger::error("setsockopt() failed: " + std::string(strerror(errno)));
		return false;
	}
	return true;
}

bool Server::bindSocket(int fd, const std::string& ip, int port) const
{
	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	if (ip == "0.0.0.0")
		addr.sin_addr.s_addr = INADDR_ANY;
	else if (inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0)
	{
		Logger::error("Invalid IP address: " + ip);
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

bool Server::makeSocketNonBlocking(int fd) const
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

bool Server::startListening(int fd) const
{
	if (listen(fd, SOMAXCONN) < 0)
	{
		Logger::error("listen() failed: " + std::string(strerror(errno)));
		return false;
	}
	Logger::info("Listening on FD: " + intToString(fd));
	return true;
}

void Server::logListeningMessage(const std::string& ip, int port) const
{
	std::ostringstream oss;
	oss << "Configured to listen on " << ip << ":" << port;
	Logger::info(oss.str());
}

void Server::logSocketInfo(int fd) const
{
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);
	if (getsockname(fd, (struct sockaddr*)&addr, &len) < 0)
	{
		Logger::error("getsockname failed for FD: " + intToString(fd));
		return;
	}

	char ip_str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &addr.sin_addr, ip_str, INET_ADDRSTRLEN);
	std::ostringstream oss;
	oss << "Socket FD " << fd << " listening on "
		<< ip_str << ":" << ntohs(addr.sin_port);
	Logger::info(oss.str());
}

std::string Server::intToString(int value) const
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}
