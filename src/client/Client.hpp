/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 17:01:17 by jmeirele          #+#    #+#             */
/*   Updated: 2025/08/11 17:02:48 by jmeirele         ###   ########.fr       */
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
		Client(int fd, const struct sockaddr_in& addr, const ServerConfig &config);
		~Client();

		bool handleClientRequest();
		bool handleClientResponse();
		void closeClient();

		int getFd() const;
		const std::string& getClientAddress() const;
		bool isClientClosed() const;

	private:
		int _fd;
		bool _closed;
		std::string _readBuffer;
		std::string _writeBuffer;
		std::string _clientAddress;

		Request *_request;
		Response _response;
		const ServerConfig &_config;

		bool _hasCompleteRequest() const;
};
