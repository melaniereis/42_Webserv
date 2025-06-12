/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 19:42:37 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/12 22:14:25 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"

ServerConfig::~ServerConfig(){}

ServerConfig::ServerConfig()
{
	_root = "./pages";
	_indexes.push_back("./pages/index.html");
	_notFound = "./pages/404.html";
	_clientMaxBodySize = 1048576; // 1 MB
	// _maxClients = 100; // Default value, can be set later
	// _clientTimeout = 60; // Default value, can be set later
}

const std::string& ServerConfig::getServerRoot() const { return _root; }
const std::vector<std::string>& ServerConfig::getServerIndexes() const { return _indexes; }
std::string ServerConfig::getServerNotFound() const { return _notFound; }
std::string ServerConfig::getServerHost() const
{
	std::string ip = _listens.empty() ? "0.0.0.0" : _listens.begin()->second.getIp();
	return ip;
}
unsigned int ServerConfig::getServerPort() const
{
	return _listens.empty() ? 8080 : _listens.begin()->second.getPort();
}

const std::vector<std::string>& ServerConfig::getServerNames() const
{
	return _serverNames;
}
void ServerConfig::addServerName(const std::string& name)
{
	if (std::find(_serverNames.begin(), _serverNames.end(), name) == _serverNames.end())
		_serverNames.push_back(name);
}

void ServerConfig::setRoot(const std::string& root) { _root = root; }
void ServerConfig::setIndex(const std::vector<std::string>& index) { _indexes = index; }
void ServerConfig::setNotFound(const std::string& path) { _notFound = path; }
// void ServerConfig::setMaxClients(size_t maxClients) { _maxClients = maxClients; }
// void ServerConfig::setClientTimeout(size_t timeout) { _clientTimeout = timeout; }
void ServerConfig::setHost(const std::string& host)
{
	if (!_listens.empty())
	{
		ListenConfig& listen = _listens.begin()->second;
		listen = ListenConfig(host + ":" + intToString(listen.getPort()));
	}
}

void ServerConfig::setPort(unsigned int port)
{
	if (port < 1 || port > 65535)
		throw std::runtime_error("Invalid port number");
	if (!_listens.empty())
	{
		ListenConfig& listen = _listens.begin()->second;
		listen = ListenConfig(listen.getIp() + ":" + intToString(listen.getPort()));
	}
}


void ServerConfig::addLocation(const LocationConfig& loc)
{
	std::string path = loc.getPath();
	if (_locations.find(path) != _locations.end()) {
		throw std::runtime_error("Duplicate location path: " + path);
	}
	_locations[path] = loc;
}

void ServerConfig::addListen(const std::string& token)
{
	ListenConfig listen(token);
	_listens[listen.getIpPortJoin()] = listen;
}

void ServerConfig::setClientMaxBodySize(size_t size)
{
	_clientMaxBodySize = size;
}

size_t ServerConfig::getClientMaxBodySize() const
{
	return _clientMaxBodySize;
}

const std::map<std::string, ListenConfig>& ServerConfig::getListens() const
{
	return _listens;
}

const std::map<std::string, LocationConfig> &ServerConfig::getLocations() const
{
	return _locations;
}

std::string ServerConfig::intToString(int v)
{
	std::ostringstream oss;
	oss << v;
	return oss.str();
}

const LocationConfig* ServerConfig::findLocation(const std::string& requestPath) const
{
	if (requestPath.empty()) {
		std::map<std::string, LocationConfig>::const_iterator root_it = _locations.find("/");
		return (root_it != _locations.end()) ? &root_it->second : NULL;
	}

	std::string path = requestPath;

	// Normalize path - ensure it starts with '/'
	if (path[0] != '/') path = "/" + path;

	// Try exact match first
	std::map<std::string, LocationConfig>::const_iterator it = _locations.find(path);
	if (it != _locations.end()) {
		return &it->second;
	}

	// Try with trailing slash
	if (path[path.size()-1] != '/') {
		std::string dirPath = path + "/";
		it = _locations.find(dirPath);
		if (it != _locations.end()) {
			return &it->second;
		}
	}

	// Find the best matching prefix
	std::string bestMatch = "";
	const LocationConfig* bestLocation = NULL;

	for (it = _locations.begin(); it != _locations.end(); ++it) {
		const std::string& locPath = it->first;

		// Check if this location is a prefix of the request path
		if (path.compare(0, locPath.length(), locPath) == 0) {
			// Use the longest matching prefix
			if (locPath.length() > bestMatch.length()) {
				bestMatch = locPath;
				bestLocation = &it->second;
			}
		}
	}

	if (bestLocation) {
		return bestLocation;
	}

	// Final fallback to root location
	it = _locations.find("/");
	return (it != _locations.end()) ? &it->second : NULL;
}
