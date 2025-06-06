/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 19:39:26 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/05 17:14:14 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../../inc/webserv.hpp"

class ServerConfig
{
	public:
		ServerConfig();
		~ServerConfig();

		int getServerPort() const;
		std::string getServerHost() const;
		std::string getServerName() const;
		std::string getServerRoot() const;
		std::string getServerIndex() const;
		std::string getServerNotFound() const;

	private:
		int _port;
		std::string _host;
		std::string _name;
		std::string _root;
		std::string _index;
		std::string _notFound;
};
