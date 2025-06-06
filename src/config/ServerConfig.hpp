/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 19:39:26 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/06 15:58:09 by meferraz         ###   ########.fr       */
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
		const std::string getServerHost() const;
		const std::string getServerName() const;
		const std::string getServerRoot() const;
		const std::string getServerIndex() const;
		const std::string getServerNotFound() const;

		void setPort(int port);
		void setHost(const std::string& host);
		void setName(const std::string& name);
		void setRoot(const std::string& root);
		void setIndex(const std::string& index);
		void setNotFound(const std::string& path);


	private:
		int _port;
		const std::string _host;
		const std::string _name;
		const std::string _root;
		const std::string _index;
		const std::string _notFound;
};
