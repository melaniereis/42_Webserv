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
	_index = "./pages/index.html";
	_notFound = "./pages/404.html";
}

int ServerConfig::getServerPort() const { return _port; }
std::string ServerConfig::getServerHost() const { return _host; }
std::string ServerConfig::getServerName() const { return _name; }
std::string ServerConfig::getServerRoot() const { return _root; }
std::string ServerConfig::getServerIndex() const { return _index; }
std::string ServerConfig::getServerNotFound() const { return _notFound; }

void ServerConfig::setPort(int port) { _port = port; }
void ServerConfig::setHost(const std::string& host) { _host = host; }
void ServerConfig::setName(const std::string& name) { _name = name; }
void ServerConfig::setRoot(const std::string& root) { _root = root; }
void ServerConfig::setIndex(const std::string& index) { _index = index; }
void ServerConfig::setNotFound(const std::string& path) { _notFound = path; }

