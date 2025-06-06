/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 17:01:17 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/06 15:27:07 by jmeirele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "inc/webserv.hpp"
#include "../server/ServerConfig.hpp"

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
	
		std::string _index;
	
		ServerConfig _config;
		
		void closeClient();
};