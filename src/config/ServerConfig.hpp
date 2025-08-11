/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 19:39:26 by jmeirele          #+#    #+#             */
/*   Updated: 2025/08/11 14:46:56 by jmeirele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "LocationConfig.hpp"
#include "ListenConfig.hpp"
#include "../../inc/webserv.hpp"

class LocationConfig; // Forward declaration to avoid circular dependency

class ServerConfig
{
	public:
		ServerConfig();
		~ServerConfig();

		// Getters
		const std::map<std::string, ListenConfig>& getListens() const;
		const std::vector<std::string>& getServerNames() const;
		const std::string& getServerRoot() const;
		const std::vector<std::string>& getServerIndexes() const;
		std::string getServerHost() const;
		unsigned int getServerPort() const;
		size_t getClientMaxBodySize() const;
		const std::map<std::string, LocationConfig>& getLocations() const;
		const std::map<int, std::string> &getErrorPage() const;

		// Setters with validation
		void addListen(const std::string& token);
		void addServerName(const std::string& name);
		void setRoot(const std::string& root);
		void setHost(const std::string& host);
		void setPort(unsigned int port);
		void setIndex(const std::vector<std::string>& index);
		void setErrorPage(int code, const std::string& path);
		void setClientMaxBodySize(size_t size);
		void addLocation(const LocationConfig& loc);

	private:
		std::map<std::string, ListenConfig> _listens;
		std::vector<std::string> _serverNames;
		std::string _root;
		std::vector<std::string> _indexes;
		size_t _clientMaxBodySize;
		std::map<std::string, LocationConfig> _locations;
		std::map<int, std::string> _errorPage;

		std::string _intToString(int v) const;
		void _validatePort(unsigned int port) const;
		void _validatePath(const std::string& path) const;
		void _validateServerName(const std::string& name) const;
};
