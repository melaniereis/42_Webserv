/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 17:01:17 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/10 16:25:31 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../../inc/webserv.hpp"
#include "../config/ServerConfig.hpp"
#include "../http/Request.hpp"
#include "../http/Response.hpp"
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
