/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 19:30:04 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/24 16:39:06 by meferraz         ###   ########.fr       */
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

	bool setup(); // Setup server sockets
	int acceptNewConnection(int serverFd); // Accept new client
	bool handleClientEvent(int clientFd, short revents); // Handle client I/O
	const std::vector<int>& getServerFds() const; // Get server sockets
	void removeClient(int fd); // Remove client

private:
	std::vector<int> _serverFds;
	ServerConfig _config;
	ClientManager _clientManager;

	int _createSocket() const;
	bool _setSocketOptions(int fd) const;
	bool _bindSocket(int fd, const std::string& ip, int port) const;
	bool _makeSocketNonBlocking(int fd) const;
	bool _startListening(int fd) const;
	void _logListeningMessage(const std::string& ip, int port) const;
	std::string _intToString(int value) const;

	Server(const Server&);
	Server& operator=(const Server&);
};
