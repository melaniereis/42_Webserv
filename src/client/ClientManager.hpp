/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 20:39:08 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/09 21:49:14 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Client.hpp"

class ClientManager
{
	public:
		ClientManager();
		~ClientManager();

		void acceptNewClient(int serverFd, const ServerConfig &config);
		void handleClientIO(int fd);
		void removeClient(int fd);
		std::vector<struct pollfd> &getPollFds();

	private:
		std::map<int, Client *> _clients;
		std::vector<struct pollfd> _pollFds;

		void updatePollFds();
};

// struct pollfd is the core structure used by the poll() system call, which allows you to monitor multiple file descriptors (like sockets) to see if they're ready for reading, writing, or if an error occurred.
// Poll works for both linux and macOs

/* struct pollfd {
	int fd;
	short events;
	short revents;
}; */
