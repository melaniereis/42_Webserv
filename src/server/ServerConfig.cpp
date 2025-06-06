/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 19:42:37 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/04 21:59:55 by jmeirele         ###   ########.fr       */
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
