/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 19:42:37 by jmeirele          #+#    #+#             */
/*   Updated: 2025/07/11 16:23:24 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"

ServerConfig::ServerConfig() : _root("./pages"),
							   _notFound("./pages/404.html"),
							   _clientMaxBodySize(1048576) // 1 MB
{
	_indexes.push_back("./pages/index.html");
}

ServerConfig::~ServerConfig() {}

const std::string &ServerConfig::getServerRoot() const { return _root; }
const std::vector<std::string> &ServerConfig::getServerIndexes() const { return _indexes; }
std::string ServerConfig::getServerNotFound() const { return _notFound; }
std::string ServerConfig::getServerHost() const
{
	return _listens.empty() ? "0.0.0.0" : _listens.begin()->second.getIp();
}

unsigned int ServerConfig::getServerPort() const
{
	return _listens.empty() ? 8080 : _listens.begin()->second.getPort();
}

const std::vector<std::string> &ServerConfig::getServerNames() const
{
	return _serverNames;
}

std::string ServerConfig::_intToString(int v) const
{
	std::ostringstream oss;
	oss << v;
	return oss.str();
}

void ServerConfig::_validatePort(unsigned int port) const
{
	if (port < 1 || port > 65535)
	{
		std::ostringstream oss;
		oss << "Invalid port number: " << port
			<< " (must be between 1 and 65535)";
		throw std::runtime_error(oss.str());
	}
}

void ServerConfig::_validatePath(const std::string &path) const
{
	if (path.empty())
	{
		throw std::runtime_error("Path cannot be empty");
	}
}

void ServerConfig::_validateServerName(const std::string &name) const
{
	if (name.empty())
	{
		throw std::runtime_error("Server name cannot be empty");
	}

	// Check for invalid characters
	for (size_t i = 0; i < name.size(); i++)
	{
		unsigned char c = name[i];
		if (!std::isalnum(c) && c != '-' && c != '.' && c != '_')
		{
			std::ostringstream oss;
			oss << "Invalid character in server name: '" << name << "'";
			throw std::runtime_error(oss.str());
		}
	}
}

void ServerConfig::addServerName(const std::string &name)
{
	_validateServerName(name);

	// Check for duplicates
	if (std::find(_serverNames.begin(), _serverNames.end(), name) == _serverNames.end())
	{
		_serverNames.push_back(name);
	}
}

void ServerConfig::setRoot(const std::string &root)
{
	_validatePath(root);
	_root = root;
}

void ServerConfig::setIndex(const std::vector<std::string> &index)
{
	if (index.empty())
	{
		throw std::runtime_error("Index list cannot be empty");
	}
	_indexes = index;
}

void ServerConfig::setNotFound(const std::string &path)
{
	_validatePath(path);
	_notFound = path;
}

void ServerConfig::setHost(const std::string &host)
{
	if (host.empty())
	{
		throw std::runtime_error("Host cannot be empty");
	}

	if (!_listens.empty())
	{
		ListenConfig &listen = _listens.begin()->second;
		listen = ListenConfig(host + ":" + _intToString(listen.getPort()));
	}
}

void ServerConfig::setPort(unsigned int port)
{
	_validatePort(port);

	if (!_listens.empty())
	{
		ListenConfig &listen = _listens.begin()->second;
		listen = ListenConfig(listen.getIp() + ":" + _intToString(port));
	}
}

void ServerConfig::addLocation(const LocationConfig &loc)
{
	const std::string &path = loc.getPath();
	if (_locations.find(path) != _locations.end())
	{
		throw std::runtime_error("Duplicate location path: " + path);
	}
	_locations[path] = loc;
}

void ServerConfig::addListen(const std::string &token)
{
	try
	{
		ListenConfig listen(token);
		_listens[listen.getIpPortJoin()] = listen;
	}
	catch (const std::exception &e)
	{
		std::ostringstream oss;
		oss << "Invalid listen directive: '" << token << "' - " << e.what();
		throw std::runtime_error(oss.str());
	}
}

void ServerConfig::setClientMaxBodySize(size_t size)
{
	// Set a reasonable maximum (100MB)
	if (size > 100 * 1024 * 1024)
	{
		throw std::runtime_error("Client max body size too large (max 100MB)");
	}
	_clientMaxBodySize = size;
}

size_t ServerConfig::getClientMaxBodySize() const
{
	return _clientMaxBodySize;
}

const std::map<std::string, ListenConfig> &ServerConfig::getListens() const
{
	return _listens;
}

const std::map<std::string, LocationConfig> &ServerConfig::getLocations() const
{
	return _locations;
}

void ServerConfig::setErrorPage(int code, const std::string &path)
{
	if (code < 100 || code > 599)
	{
		throw std::runtime_error("Invalid HTTP status code: " + _intToString(code));
	}
	_validatePath(path);
	_errorPages[code] = path;
}

std::string ServerConfig::getErrorPage(int code) const
{
	std::map<int, std::string>::const_iterator it = _errorPages.find(code);
	return (it != _errorPages.end()) ? it->second : _notFound;
}
