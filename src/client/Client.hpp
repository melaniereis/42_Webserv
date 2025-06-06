/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 17:01:17 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/06 16:33:54 by jmeirele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "inc/webserv.hpp"
#include "../server/ServerConfig.hpp"
#include "../http/Request.hpp"
#include "../http/Response.hpp"

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

		Request *_request;
		Response _response;

		ServerConfig _config;
		
		void closeClient();
};