/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 20:39:08 by jmeirele          #+#    #+#             */
/*   Updated: 2025/07/06 02:45:42 by jmeirele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Client.hpp"

class ClientManager
{
	public:
		ClientManager();
		~ClientManager();

		int acceptNewClient(int serverFd, const ServerConfig &config);
		bool handleClientIO(int fd, short revents);
		void removeClient(int fd);

		void cleanup();

	private:
		std::map<int, Client *> _clients;

};

// struct pollfd is the core structure used by the poll() system call, which allows you to monitor multiple file descriptors (like sockets) to see if they're ready for reading, writing, or if an error occurred.
// Poll works for both linux and macOs

/* struct pollfd {
	int fd;
	short events;
	short revents;
}; */
