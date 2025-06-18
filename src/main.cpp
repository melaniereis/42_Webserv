/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 13:21:37 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/13 21:57:22 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/webserv.hpp"
#include "./server/Server.hpp"
#include "./config/ServerConfig.hpp"
#include "./config/ConfigParser.hpp"
#include "./config/LocationConfig.hpp"
#include "./utils/Logger.hpp"

static void handleArguments(int argc, char** argv, std::string& configPath)
{
	if (argc == 1) {
		Logger::warn("No configuration file provided. Using default: default.conf");
		configPath = "./config/valid/default.conf";
	}
	else if (argc > 2) {
		throw std::runtime_error(std::string("Too many arguments. Usage: ") +
								argv[0] + " <config_file>");
	}
	else {
		configPath = argv[1];
	}
}

// Helper function to convert int to string
static std::string intToString(int value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

int main(int argc, char** argv)
{
	std::string configPath;
	handleArguments(argc, argv, configPath);

	try {
		// Parse configuration
		Logger::info("Parsing configuration file: " + configPath);
		ConfigParser parser(configPath);
		std::vector<ServerConfig> serverConfigs = parser.parse();

		if (serverConfigs.empty()) {
			throw std::runtime_error("No valid server configurations found");
		}

		// Create servers
		std::vector<Server> servers;
		for (size_t i = 0; i < serverConfigs.size(); ++i) {
			Server server(serverConfigs[i]);
			if (server.setup()) {
				servers.push_back(server);
				Logger::info("Server " + intToString(i + 1) + " setup on " +
							serverConfigs[i].getServerHost() + ":" +
							intToString(serverConfigs[i].getServerPort()));
			}
		}

		if (servers.empty()) {
			throw std::runtime_error("No servers were successfully setup");
		}

		// Create global poll structure
		std::vector<struct pollfd> pollFds;
		std::map<int, int> fdToServerIndex;  // Maps fd to server index

		// Add server sockets to poll
		for (size_t i = 0; i < servers.size(); ++i) {
			const std::vector<int>& serverFds = servers[i].getServerFds();
			for (size_t j = 0; j < serverFds.size(); ++j) {
				struct pollfd pfd;
				pfd.fd = serverFds[j];
				pfd.events = POLLIN;
				pfd.revents = 0;
				pollFds.push_back(pfd);
				fdToServerIndex[serverFds[j]] = i;
			}
		}

		Logger::info("All servers started, waiting for connections...");

		while (true) {
			int pollResult = poll(&pollFds[0], pollFds.size(), -1);

			if (pollResult < 0) {
				if (errno == EINTR) continue;
				Logger::error(std::string("poll() failed: ") + strerror(errno));
				break;
			}

			if (pollResult == 0) continue;  // Timeout

			// Create temporary list for new connections
			std::vector<struct pollfd> newConnections;
			std::vector<int> toRemove;

			// Process events
			for (size_t i = 0; i < pollFds.size(); ++i) {
				if (pollFds[i].revents == 0) continue;

				int fd = pollFds[i].fd;
				short revents = pollFds[i].revents;

				// Check if it's a server socket
				if (fdToServerIndex.find(fd) != fdToServerIndex.end()) {
					int serverIdx = fdToServerIndex[fd];

					if (revents & POLLIN) {
						int clientFd = servers[serverIdx].acceptNewConnection(fd);
						if (clientFd >= 0) {
							struct pollfd newPfd;
							newPfd.fd = clientFd;
							newPfd.events = POLLIN | POLLOUT;
							newPfd.revents = 0;
							newConnections.push_back(newPfd);
							fdToServerIndex[clientFd] = serverIdx;
						}
					}
				}
				// Client socket
				else {
					int serverIdx = fdToServerIndex[fd];
					if (!servers[serverIdx].handleClientEvent(fd, revents)) {
						servers[serverIdx].removeClient(fd);
						fdToServerIndex.erase(fd);
						toRemove.push_back(i);
					}
				}

				// Reset revents for next poll
				pollFds[i].revents = 0;
			}

			// Add new connections
			for (size_t i = 0; i < newConnections.size(); ++i) {
				pollFds.push_back(newConnections[i]);
			}

			// Clean up closed connections (from back to front)
			for (int i = toRemove.size() - 1; i >= 0; --i) {
				int idx = toRemove[i];
				if (idx < static_cast<int>(pollFds.size()) - 1) {
					pollFds[idx] = pollFds.back();
				}
				pollFds.pop_back();
			}
		}
	}
	catch (const std::exception& e) {
		Logger::error(e.what());
		return 1;
	}
	return 0;
}
