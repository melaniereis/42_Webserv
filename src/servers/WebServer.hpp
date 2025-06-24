/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/24 17:14:02 by meferraz          #+#    #+#             */
/*   Updated: 2025/06/24 17:15:00 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../../inc/webserv.hpp"

#include "Server.hpp"
#include "../config/ConfigParser.hpp"
#include "../utils/Logger.hpp"

class WebServer
{
	public:
		WebServer(const std::string& configPath);
		~WebServer();

		void run();

	private:
		WebServer(const WebServer&);
		WebServer& operator=(const WebServer&);

		void handleArguments(int argc, char** argv);
		void parseConfig();
		void setupServers();
		void initPollStructures();
		void runEventLoop();
		void handlePollEvents(std::vector<struct pollfd>& newConnections,
							std::vector<size_t>& toRemove);
		void acceptNewConnections();
		void cleanup();

		static std::string intToString(int value);

		std::string configPath;
		std::vector<ServerConfig> serverConfigs;
		std::vector<Server*> servers;
		std::vector<struct pollfd> pollFds;
		std::map<int, int> fdToServerIndex;
		std::set<int> serverFdsSet;
};
