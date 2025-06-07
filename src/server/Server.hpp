/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 19:30:04 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/06 15:45:25 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "../config/ServerConfig.hpp"
#include "../client/Client.hpp"
#include "../client/ClientManager.hpp"


class Server
{
	public:
		Server(const ServerConfig &config);
		~Server();

		void runServer();

	private:
		int _serverFd;

		bool setupSocket();
		bool createSocket();
		bool setSocketOptions();
		bool bindSocket();
		bool makeSocketNonBlocking();
		bool startListening();
		void logListeningMessage();

		ServerConfig _config;
		ClientManager _clientManager;
};
