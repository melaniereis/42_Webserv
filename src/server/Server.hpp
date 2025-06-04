/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 19:30:04 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/04 18:41:58 by jmeirele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "ServerConfig.hpp"
#include "../client/Client.hpp"

class Server
{
	public:
		Server(const ServerConfig &config);
		~Server();

		void run();

	private:
		ServerConfig _config;
		int _serverFd;

		bool setupSocket();

		std::map<int, Client *> _clients;
		std::vector<struct pollfd> _pollFds;
		
		void acceptNewClient();
		void handleClientIO(int fd);
		void removeClient(int fd);
		void updatePollFds();
};


/* struct pollfd {
	int fd;
	short events;
	short revents;
}; */
