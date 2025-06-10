/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 19:30:04 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/09 22:13:13 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "../config/ServerConfig.hpp"
#include "../client/Client.hpp"
#include "../client/ClientManager.hpp"
#include "../utils/Logger.hpp"


class Server
{
	public:
		Server(const ServerConfig &config);
		~Server();

		void runServer();

	private:
		std::vector<int> _serverFds;
		std::set<int> _serverSocketSet;

		bool setupSocket();
		int createSocket(const std::string& ip, int port);
		bool setSocketOptions(int fd);
		bool bindSocket(int fd, const std::string& ip, int port);
		bool makeSocketNonBlocking(int fd);
		bool startListening(int fd);
		void logListeningMessage(const std::string& ip, int port);

		ServerConfig _config;
		ClientManager _clientManager;
};
