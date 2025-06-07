/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 19:39:26 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/07 18:23:27 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../../inc/webserv.hpp"
#include "LocationConfig.hpp"

class LocationConfig;

class ServerConfig
{
	public:
		ServerConfig();
		~ServerConfig();

		int getServerPort() const;
		std::string getServerHost() const;
		std::string getServerName() const;
		const std::string& getServerRoot() const;
		const std::vector<std::string>& getServerIndexes() const;
		std::string getServerNotFound() const;

		void setPort(int port);
		void setHost(const std::string& host);
		void setName(const std::string& name);
		void setRoot(const std::string& root);
		void setIndex(const std::vector<std::string>& index);
		void setNotFound(const std::string& path);
		// void setMaxClients(size_t maxClients);
		// void setClientTimeout(size_t timeout);
		void addServerName(const std::string& name) { _name = name; }
		void addErrorPage(int code, const std::string& path) { (void)code; _notFound = path; }
		void setMaxBodySize(size_t size) { (void)size; /* implement */ }
		void addLocation(const LocationConfig& loc);
		const std::vector<LocationConfig>& getLocations() const;

	private:
		int _port;
		std::string _host;
		std::string _name;
		std::string _root;
		std::vector<std::string> _indexes;
		std::string _notFound;
		// size_t _maxClients;
		// size_t _clientTimeout;

		std::vector<LocationConfig> _locations;
};
