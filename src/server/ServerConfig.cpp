/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 19:42:37 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/03 20:00:43 by jmeirele         ###   ########.fr       */
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

int ServerConfig::getServerPort() { return _port; }
std::string ServerConfig::getServerHost() { return _host; }
std::string ServerConfig::getServerName() { return _name; }
std::string ServerConfig::getServerRoot() { return _root; }
std::string ServerConfig::getServerIndex() { return _index; }
std::string ServerConfig::getServerNotFound() { return _notFound; }
