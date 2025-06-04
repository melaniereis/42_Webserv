/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 19:30:04 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/04 22:10:28 by jmeirele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "ServerConfig.hpp"
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
