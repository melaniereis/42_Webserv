/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/24 17:15:20 by meferraz          #+#    #+#             */
/*   Updated: 2025/07/12 10:45:24 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"
#include "../cgi/CgiManager.hpp"

CgiManager g_cgi_manager;  // Global instance

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
		const std::map<std::string, ListenConfig>& listens = serverConfigs[i].getListens();
			// Create a separate server instance for each listen directive
		for (std::map<std::string, ListenConfig>::const_iterator it = listens.begin();
			it != listens.end(); ++it)
		{
			Server* server = new Server(serverConfigs[i]);
			if (server->setupSocketForListen(it->second.getIp(), it->second.getPort()))
			{
				servers.push_back(server);
				Logger::info("Server setup for " + it->second.getIpPortJoin());
			}
			else
			{
				delete server;
			}
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

	time_t lastCleanup = time(NULL);
	while (true)
	{
		std::vector<struct pollfd> allFds = pollFds;

		// Add CGI pipes to poll set
		g_cgi_manager.fillPollfds(allFds);

		int pollResult = poll(&allFds[0], allFds.size(), -1);
		if (pollResult < 0) {
			if (errno == EINTR) continue;
			Logger::error(std::string("poll() failed: ") + strerror(errno));
			break;
		}

		// Handle server and client events
		std::vector<struct pollfd> newConnections;
		std::vector<size_t> toRemove;
		handlePollEvents(newConnections, toRemove, allFds);

		// Handle CGI events
		g_cgi_manager.handlePollEvents(allFds);

		// Clean up completed processes
		g_cgi_manager.checkProcesses();

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
		time_t now = time(NULL);
		if (now - lastCleanup > 600) {
			SessionManager::getInstance().cleanupSessions();
			lastCleanup = now;
		}
	}
}

void WebServer::handlePollEvents(std::vector<struct pollfd>& newConnections,
								std::vector<size_t>& toRemove, const std::vector<struct pollfd>& allFds)
{
	for (size_t i = 0; i < allFds.size(); ++i)
	{
		int fd = allFds[i].fd;
		short revents = allFds[i].revents;

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
			Client* client = servers[serverIndex]->getClientManager().getClient(fd);

			// Skip clients in CGI processing
			if (client && client->isCgiProcessing()) {
				continue;
			}

			if (!servers[serverIndex]->handleClientEvent(fd, revents))
			{
				servers[serverIndex]->removeClient(fd);
				fdToServerIndex.erase(fd);
				toRemove.push_back(i);
			}
		}
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
