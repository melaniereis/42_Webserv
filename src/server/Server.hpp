/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 19:30:04 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/16 14:30:13 by meferraz         ###   ########.fr       */
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

	void runServer();
	std::string _intToString(int v) const;

private:
	std::vector<int> _serverFds;
	std::set<int> _serverSocketSet;
	ServerConfig _config;
	ClientManager _clientManager;

	bool _setupSocket();
	int _createSocket() const;
	bool _setSocketOptions(int fd) const;
	bool _bindSocket(int fd, const std::string &ip, int port) const;
	bool _makeSocketNonBlocking(int fd) const;
	bool _startListening(int fd) const;
	void _logListeningMessage(const std::string &ip, int port) const;
	void _handlePollEvents(std::vector<struct pollfd> &pollFds);
	void _acceptNewConnection(struct pollfd &pfd);
	void _handleClientIO(struct pollfd &pfd);
	void _cleanupDisconnectedClient(struct pollfd &pfd);
};
