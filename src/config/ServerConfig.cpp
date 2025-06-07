/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 19:42:37 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/06 16:47:19 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"

ServerConfig::~ServerConfig(){}

ServerConfig::ServerConfig()
{
	_port = 4000;
	_host = "0.0.0.0";
	_name = "local";
	_root = "./pages";
	_indexes.push_back("./pages/index.html");
	_notFound = "./pages/404.html";
}

int ServerConfig::getServerPort() const { return _port; }
std::string ServerConfig::getServerHost() const { return _host; }
std::string ServerConfig::getServerName() const { return _name; }
const std::string& ServerConfig::getServerRoot() const { return _root; }
const std::vector<std::string>& ServerConfig::getServerIndexes() const { return _indexes; }
std::string ServerConfig::getServerNotFound() const { return _notFound; }

void ServerConfig::setPort(int port) { _port = port; }
void ServerConfig::setHost(const std::string& host) { _host = host; }
void ServerConfig::setName(const std::string& name) { _name = name; }
void ServerConfig::setRoot(const std::string& root) { _root = root; }
void ServerConfig::setIndex(const std::vector<std::string>& index) { _indexes = index; }
void ServerConfig::setNotFound(const std::string& path) { _notFound = path; }
// void ServerConfig::setMaxClients(size_t maxClients) { _maxClients = maxClients; }
// void ServerConfig::setClientTimeout(size_t timeout) { _clientTimeout = timeout; }
