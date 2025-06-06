/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 19:39:26 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/06 16:47:15 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "inc/webserv.hpp"

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

		void setPort(int port);
		void setHost(const std::string& host);
		void setName(const std::string& name);
		void setRoot(const std::string& root);
		void setIndex(const std::string& index);
		void setNotFound(const std::string& path);


	private:
		int _port;
		std::string _host;
		std::string _name;
		std::string _root;
		std::string _index;
		std::string _notFound;
};
