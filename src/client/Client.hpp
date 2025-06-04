/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 17:01:17 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/04 18:32:55 by jmeirele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "inc/webserv.hpp"

class Client
{
	public:
		Client(int fd);
		~Client();

		int getClientFd() const;
		bool isClientClosed() const;

		std::string getClientRead();
		std::string getClientWrite();

		bool handleClientRead();
		bool handleClientWrite();
		
	private:
		int _fd;
		bool _closed;
		std::string _readBuffer;
		std::string _writeBuffer;
		
		void closeClient();
};