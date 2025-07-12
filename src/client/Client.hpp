/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 17:01:17 by jmeirele          #+#    #+#             */
/*   Updated: 2025/07/12 10:47:24 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../../inc/webserv.hpp"
#include "../config/ServerConfig.hpp"
#include "../http/Request.hpp"
#include "../http/Response.hpp"
#include "../http/RequestHandler.hpp"
#include "../utils/Logger.hpp"

class Client;

class Client
{
	public:
		Client(int fd, const struct sockaddr_in& addr, const ServerConfig &config);
		~Client();


		int getClientFd() const;
		bool isClientClosed() const;

		std::string getClientRead();
		std::string getClientWrite();

		bool handleClientRequest();
		bool handleClientResponse();
		void closeClient();

		void setCgiOutput(const std::string& output);
		bool isCgiProcessing() const;
		void setCgiProcessing(bool state);
		int getFd() const;
		const std::string& getClientAddress() const;

	private:
		int _fd;
		bool _closed;
		std::string _readBuffer;
		std::string _writeBuffer;
		bool _cgi_processing;
		std::string _cgi_output;
		std::string _clientAddress;

		Request *_request;
		Response _response;

		const ServerConfig &_config;
};
