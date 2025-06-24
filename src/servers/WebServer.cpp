/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/24 17:15:20 by meferraz          #+#    #+#             */
/*   Updated: 2025/06/24 17:15:29 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"

WebServer::WebServer(const std::string& configPath) :
	configPath(configPath) {}

WebServer::~WebServer()
{
	cleanup();
}

void WebServer::run()
{
	parseConfig();
	setupServers();
	initPollStructures();
	runEventLoop();
}

void WebServer::parseConfig()
{
	Logger::info("Parsing configuration file: " + configPath);
	ConfigParser parser(configPath);
	serverConfigs = parser.parse();

	if (serverConfigs.empty())
		throw std::runtime_error("No valid server configurations found");
}

void WebServer::setupServers()
{
	for (size_t i = 0; i < serverConfigs.size(); ++i)
	{
		Server* server = new Server(serverConfigs[i]);
		if (server->setup())
		{
			servers.push_back(server);
			Logger::info("Server " + intToString(i + 1) + " setup");
		}
		else
		{
			delete server;
		}
	}

	if (servers.empty())
		throw std::runtime_error("No servers were successfully setup");
}

void WebServer::initPollStructures()
{
	for (size_t i = 0; i < servers.size(); ++i)
	{
		const std::vector<int>& serverFds = servers[i]->getServerFds();
		for (size_t j = 0; j < serverFds.size(); ++j)
		{
			struct pollfd pfd;
			pfd.fd = serverFds[j];
			pfd.events = POLLIN;
			pfd.revents = 0;
			pollFds.push_back(pfd);
			serverFdsSet.insert(serverFds[j]);
			fdToServerIndex[serverFds[j]] = i;
		}
	}
}

void WebServer::runEventLoop()
{
	Logger::info("All servers started, waiting for connections...");

	while (true)
	{
		int pollResult = poll(&pollFds[0], pollFds.size(), -1);
		if (pollResult < 0)
		{
			if (errno == EINTR) continue;
			Logger::error(std::string("poll() failed: ") + strerror(errno));
			break;
		}

		std::vector<struct pollfd> newConnections;
		std::vector<size_t> toRemove;
		handlePollEvents(newConnections, toRemove);

		// Add new connections
		for (size_t i = 0; i < newConnections.size(); ++i)
			pollFds.push_back(newConnections[i]);

		// Remove closed clients
		for (std::vector<size_t>::reverse_iterator it = toRemove.rbegin();
			it != toRemove.rend(); ++it)
		{
			size_t idx = *it;
			if (idx < pollFds.size() - 1)
				pollFds[idx] = pollFds.back();
			pollFds.pop_back();
		}
	}
}

void WebServer::handlePollEvents(std::vector<struct pollfd>& newConnections,
								std::vector<size_t>& toRemove)
{
	for (size_t i = 0; i < pollFds.size(); ++i)
	{
		if (pollFds[i].revents == 0) continue;

		int fd = pollFds[i].fd;
		short revents = pollFds[i].revents;

		if (serverFdsSet.find(fd) != serverFdsSet.end())
		{
			int serverIndex = fdToServerIndex[fd];
			if (revents & POLLIN)
			{
				int clientFd = servers[serverIndex]->acceptNewConnection(fd);
				if (clientFd >= 0)
				{
					struct pollfd newPfd;
					newPfd.fd = clientFd;
					newPfd.events = POLLIN | POLLOUT;
					newPfd.revents = 0;
					newConnections.push_back(newPfd);
					fdToServerIndex[clientFd] = serverIndex;
				}
			}
		}
		else
		{
			int serverIndex = fdToServerIndex[fd];
			if (!servers[serverIndex]->handleClientEvent(fd, revents))
			{
				servers[serverIndex]->removeClient(fd);
				fdToServerIndex.erase(fd);
				toRemove.push_back(i);
			}
		}
		pollFds[i].revents = 0;
	}
}

void WebServer::cleanup()
{
	for (size_t i = 0; i < pollFds.size(); ++i)
	{
		if (pollFds[i].fd >= 0)
			close(pollFds[i].fd);
	}

	for (size_t i = 0; i < servers.size(); ++i)
		delete servers[i];

	pollFds.clear();
	servers.clear();
	serverFdsSet.clear();
	fdToServerIndex.clear();
}

std::string WebServer::intToString(int value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}
