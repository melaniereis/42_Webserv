/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 19:30:04 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/04 20:08:51 by jmeirele         ###   ########.fr       */
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

		void runServer();

	private:
		ServerConfig _config;
		int _serverFd;
		
		bool setupSocket();
		bool createSocket();
		bool setSocketOptions();
		bool bindSocket();
		bool makeSocketNonBlocking();
		bool startListening();
		void logListeningMessage();


		std::map<int, Client *> _clients;
		std::vector<struct pollfd> _pollFds;
		
		void acceptNewClient();
		void handleClientIO(int fd);
		void removeClient(int fd);
		void updatePollFds();
};

// struct pollfd is the core structure used by the poll() system call, which allows you to monitor multiple file descriptors (like sockets) to see if they're ready for reading, writing, or if an error occurred.
// Poll works for both linux and macOs

/* struct pollfd {
	int fd;
	short events;
	short revents;
}; */
