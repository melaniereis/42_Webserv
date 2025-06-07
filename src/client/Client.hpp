/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 17:01:17 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/07 18:03:08 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../../inc/webserv.hpp"
#include "../config/ServerConfig.hpp"

class Client
{
	public:
		Client(int fd);
		~Client();

		int getClientFd() const;
		bool isClientClosed() const;

		std::string getClientRead();
		std::string getClientWrite();

		bool handleClientRequest();
		bool handleClientResponse();

	private:
		int _fd;
		bool _closed;
		std::string _readBuffer;
		std::string _writeBuffer;

		ServerConfig _config;

		void closeClient();
};
