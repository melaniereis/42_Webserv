/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/24 17:14:02 by meferraz          #+#    #+#             */
/*   Updated: 2025/07/06 02:45:53 by jmeirele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../../inc/webserv.hpp"

#include "Server.hpp"
#include "../config/ConfigParser.hpp"
#include "../utils/Logger.hpp"
#include "../client/ClientManager.hpp"

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

		static void handleSigInt(int signum);

		static std::string intToString(int value);

		std::string configPath;
		std::vector<ServerConfig> serverConfigs;
		std::vector<Server*> servers;
		std::vector<struct pollfd> pollFds;
		std::map<int, int> fdToServerIndex;
		std::set<int> serverFdsSet;

		static bool _stopFlag;
};
