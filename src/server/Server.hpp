/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 19:30:04 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/03 19:59:24 by jmeirele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "ServerConfig.hpp"

class Server
{
	public:
		Server(const ServerConfig &config);
		void run();

	private:
		ServerConfig _config;
		int _serverFd;

		bool setupSocket();
		void acceptLoop();
		void handleClient(int client_fd);
};