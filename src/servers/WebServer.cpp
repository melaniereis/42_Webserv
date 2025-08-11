/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/24 17:14:02 by meferraz          #+#    #+#             */
/*   Updated: 2025/08/11 15:08:28 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServer.hpp"

WebServer::WebServer(const std::string& configPath) : configPath(configPath)
{
	parseConfig();
	setupServers();
	initPollStructures();
}

WebServer::~WebServer()
{
	cleanup();
}

void WebServer::parseConfig()
{
	ConfigParser parser(configPath);
	serverConfigs = parser.parse();

	if (serverConfigs.empty()) {
		throw std::runtime_error("No server configurations found");
	}

	Logger::info("Parsed " + intToString(serverConfigs.size()) + " server configurations");
}

void WebServer::setupServers()
{
	for (size_t i = 0; i < serverConfigs.size(); ++i) {
		Server* server = new Server(serverConfigs[i]);
		if (!server->setup()) {
			delete server;
			throw std::runtime_error("Failed to setup server");
		}
		servers.push_back(server);
	}
}

void WebServer::initPollStructures()
{
	for (size_t i = 0; i < servers.size(); ++i) {
		const std::vector<int>& serverFds = servers[i]->getServerFds();
		for (size_t j = 0; j < serverFds.size(); ++j) {
			struct pollfd pfd;
			pfd.fd = serverFds[j];
			pfd.events = POLLIN;
			pfd.revents = 0;
			pollFds.push_back(pfd);

			fdToServerIndex[serverFds[j]] = i;
			serverFdsSet.insert(serverFds[j]);
		}
	}
}

void WebServer::run()
{
	Logger::info("WebServer starting...");
	runEventLoop();
}

void WebServer::runEventLoop()
{
	while (true) {
		// SINGLE POLL CALL - evaluation requirement
		int ready = poll(pollFds.data(), pollFds.size(), -1);

		if (ready < 0) {
			// NO ERRNO CHECKING - evaluation requirement
			continue;
		}

		std::vector<struct pollfd> newConnections;
		std::vector<size_t> toRemove;

		handlePollEvents(newConnections, toRemove, pollFds);

		// Add new connections
		for (size_t i = 0; i < newConnections.size(); ++i) {
			pollFds.push_back(newConnections[i]);
		}

		// Remove closed connections (in reverse order)
		for (int i = toRemove.size() - 1; i >= 0; --i) {
			size_t index = toRemove[i];
			int fd = pollFds[index].fd;

			// Remove from server if it's a client
			if (serverFdsSet.find(fd) == serverFdsSet.end()) {
				// Find which server owns this client
				for (size_t j = 0; j < servers.size(); ++j) {
					servers[j]->removeClient(fd);
				}
			}

			pollFds.erase(pollFds.begin() + index);
		}
	}
}

void WebServer::handlePollEvents(std::vector<struct pollfd>& newConnections,
								std::vector<size_t>& toRemove,
								const std::vector<struct pollfd>& allFds)
{
	for (size_t i = 0; i < allFds.size(); ++i) {
		if (allFds[i].revents == 0) continue;

		int fd = allFds[i].fd;

		if (serverFdsSet.find(fd) != serverFdsSet.end()) {
			// Server socket - accept new connection
			int serverIndex = fdToServerIndex[fd];
			int clientFd = servers[serverIndex]->acceptNewConnection(fd);

			if (clientFd > 0) {
				struct pollfd clientPfd;
				clientPfd.fd = clientFd;
				clientPfd.events = POLLIN | POLLOUT;
				clientPfd.revents = 0;
				newConnections.push_back(clientPfd);
			}
		} else {
			// Client socket - handle I/O
			bool keepConnection = false;
			for (size_t j = 0; j < servers.size(); ++j) {
				if (servers[j]->handleClientEvent(fd, allFds[i].revents)) {
					keepConnection = true;
					break;
				}
			}

			if (!keepConnection) {
				toRemove.push_back(i);
			}
		}
	}
}

void WebServer::cleanup()
{
	for (size_t i = 0; i < servers.size(); ++i) {
		delete servers[i];
	}
	servers.clear();
}

std::string WebServer::intToString(int value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}
