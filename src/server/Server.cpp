/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 19:30:10 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/04 15:24:02 by jmeirele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(const ServerConfig& config) : _config(config), _serverFd(-1) {}

bool Server::setupSocket() {
	_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverFd == -1) {
		std::perror("socket failed");
		return false;
	}

	int opt = 1;
	if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		std::perror("setsockopt failed");
		close(_serverFd);
		return false;
	}

	struct sockaddr_in address;
	std::memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(_config.getServerPort());

	if (bind(_serverFd, (struct sockaddr*)&address, sizeof(address)) < 0) {
		std::perror("bind failed");
		close(_serverFd);
		return false;
	}

	if (listen(_serverFd, 5) < 0) {
		std::perror("listen failed");
		close(_serverFd);
		return false;
	}

	std::cout << "🟢 Server listening on port " << _config.getServerPort() << std::endl;
	return true;
}

void Server::run() {
	if (!setupSocket())
		return;

	acceptLoop();
	close(_serverFd);
}

void Server::acceptLoop() {
	while (true) {
		struct sockaddr_in client_addr;
		socklen_t client_len = sizeof(client_addr);
		int client_fd = accept(_serverFd, (struct sockaddr*)&client_addr, &client_len);
		if (client_fd < 0) {
			std::perror("accept failed");
			continue;
		}

		handleClient(client_fd);
		close(client_fd);
	}
}

void Server::handleClient(int client_fd) {
	char buffer[4096];
	std::memset(buffer, 0, sizeof(buffer));
	read(client_fd, buffer, sizeof(buffer));

	std::cout << "📥 Request received:\n" << buffer << std::endl;

	std::string response =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/plain\r\n"
		"Content-Length: 13\r\n"
		"\r\n"
		"Hello, World!";

	send(client_fd, response.c_str(), response.length(), 0);
}