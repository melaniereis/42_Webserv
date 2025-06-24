/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 19:30:04 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/24 17:00:06 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../config/ServerConfig.hpp"
#include "../client/ClientManager.hpp"
#include "../utils/Logger.hpp"

class Server
{
	public:
		Server(const ServerConfig &config);
		~Server();

		bool setup();
		int acceptNewConnection(int serverFd);
		bool handleClientEvent(int clientFd, short revents);
		const std::vector<int>& getServerFds() const;
		void removeClient(int fd);

	private:
		Server(const Server&);
		Server& operator=(const Server&);

		int createSocket() const;
		bool setSocketOptions(int fd) const;
		bool bindSocket(int fd, const std::string& ip, int port) const;
		bool makeSocketNonBlocking(int fd) const;
		bool startListening(int fd) const;
		void logListeningMessage(const std::string& ip, int port) const;
		std::string intToString(int value) const;
		bool setupSocketForListen(const std::string& ip, int port);
		bool verifySocket(int fd) const;
		void logSocketInfo(int fd) const;

		std::vector<int> serverFds;
		ServerConfig config;
		ClientManager clientManager;
};
