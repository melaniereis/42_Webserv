/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 19:39:26 by jmeirele          #+#    #+#             */
/*   Updated: 2025/07/11 16:21:28 by meferraz         ###   ########.fr       */
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
		std::string getServerNotFound() const;
		size_t getClientMaxBodySize() const;
		const std::map<std::string, LocationConfig>& getLocations() const;

		// Setters with validation
		void addListen(const std::string& token);
		void addServerName(const std::string& name);
		void setRoot(const std::string& root);
		void setHost(const std::string& host);
		void setPort(unsigned int port);
		void setIndex(const std::vector<std::string>& index);
		void setNotFound(const std::string& path);
		void setClientMaxBodySize(size_t size);
		void addLocation(const LocationConfig& loc);
		void setErrorPage(int code, const std::string& path);
		std::string getErrorPage(int code) const;

	private:
		std::map<std::string, ListenConfig> _listens;
		std::vector<std::string> _serverNames;
		std::string _root;
		std::vector<std::string> _indexes;
		std::string _notFound;
		size_t _clientMaxBodySize;
		std::map<std::string, LocationConfig> _locations;

		std::string _intToString(int v) const;
		void _validatePort(unsigned int port) const;
		void _validatePath(const std::string& path) const;
		void _validateServerName(const std::string& name) const;
		std::map<int, std::string> _errorPages;
};
