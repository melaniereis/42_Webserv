/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListenConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 17:44:42 by meferraz          #+#    #+#             */
/*   Updated: 2025/06/09 17:45:47 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ListenConfig.hpp"

ListenConfig::ListenConfig() : _ip("0.0.0.0"), _port(8080), _ipPortJoin("0.0.0.0:8080") {}

ListenConfig::ListenConfig(const std::string& token)
{
	// Parse IP:PORT format
	size_t colonPos = token.find(':');
	if (colonPos == std::string::npos) {
		_port = atoi(token.c_str());
		_ip = "0.0.0.0";
	} else {
		_ip = token.substr(0, colonPos);
		_port = atoi(token.substr(colonPos + 1).c_str());
	}

	if (_port < 1 || _port > 65535) {
		throw std::runtime_error("Invalid port number");
	}

	std::ostringstream oss;
	oss << _ip << ":" << _port;
	_ipPortJoin = oss.str();
}

const std::string& ListenConfig::getIp() const { return _ip; }
unsigned int ListenConfig::getPort() const { return _port; }
const std::string ListenConfig::getIpPortJoin() const { return _ipPortJoin; }
