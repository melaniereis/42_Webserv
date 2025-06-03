/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 19:39:26 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/03 19:58:23 by jmeirele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "inc/webserv.hpp"

class ServerConfig
{
	public:
		ServerConfig();
		~ServerConfig();

		int getServerPort();
		std::string getServerHost();
		std::string getServerName();
		std::string getServerRoot();
		std::string getServerIndex();
		std::string getServerNotFound();

	protected:
		int _port;
		std::string _host;
		std::string _name;
		std::string _root;
		std::string _index;
		std::string _notFound;
};