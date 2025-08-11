/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 17:01:17 by jmeirele          #+#    #+#             */
/*   Updated: 2025/08/11 12:46:08 by jmeirele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../../inc/webserv.hpp"
#include "../config/ServerConfig.hpp"
#include "../http/Request.hpp"
#include "../http/Response.hpp"
#include "../http/RequestHandler.hpp"
#include "../utils/Logger.hpp"

class Client
{
	public:
		Client(int fd, const ServerConfig &config);
		~Client();


		int getClientFd() const;
		bool isClientClosed() const;

		std::string getClientRead();
		std::string getClientWrite();

		bool handleClientRequest();
		bool handleClientResponse();
		void closeClient();

	private:
		int _fd;
		bool _closed;
		std::string _readBuffer;
		std::string _writeBuffer;

		std::string _index;

		Request *_request;
		Response _response;

		const ServerConfig &_config;
};
