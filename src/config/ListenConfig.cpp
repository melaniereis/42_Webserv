/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListenConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 17:44:42 by meferraz          #+#    #+#             */
/*   Updated: 2025/06/13 17:27:14 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ListenConfig.hpp"
#include <sstream>
#include <cstdlib>

ListenConfig::ListenConfig() :
	_ip("0.0.0.0"),
	_port(8080),
	_ipPortJoin("0.0.0.0:8080")
{}

ListenConfig::ListenConfig(const std::string& token)
{
	_parseToken(token);
	_validatePort();
	_validateIp();

	std::ostringstream oss;
	oss << _ip << ":" << _port;
	_ipPortJoin = oss.str();
}

const std::string& ListenConfig::getIp() const { return _ip; }
unsigned int ListenConfig::getPort() const { return _port; }
const std::string& ListenConfig::getIpPortJoin() const { return _ipPortJoin; }

void ListenConfig::_parseToken(const std::string& token)
{
	size_t colonPos = token.find(':');

	if (colonPos == std::string::npos) {
		// Port only format
		_ip = "0.0.0.0";
		_port = std::atoi(token.c_str());
	} else {
		// IP:PORT format
		_ip = token.substr(0, colonPos);
		_port = std::atoi(token.substr(colonPos + 1).c_str());
	}
}

void ListenConfig::_validatePort() const
{
	if (_port < 1 || _port > 65535) {
		std::ostringstream oss;
		oss << "Invalid port number: " << _port
			<< " (must be between 1 and 65535)";
		throw std::runtime_error(oss.str());
	}
}

void ListenConfig::_validateIp() const
{
	if (_ip == "0.0.0.0" || _ip == "localhost" || _ip == "*") {
		return; // Special values are acceptable
	}

	size_t start = 0;
	size_t end = 0;
	int components = 0;

	while ((end = _ip.find('.', start)) != std::string::npos) {
		components++;
		std::string component = _ip.substr(start, end - start);

		if (!_isValidIpComponent(component)) {
			std::ostringstream oss;
			oss << "Invalid IP address component: " << component;
			throw std::runtime_error(oss.str());
		}

		start = end + 1;
	}

	// Last component after the final dot
	std::string component = _ip.substr(start);
	if (!_isValidIpComponent(component)) {
		std::ostringstream oss;
		oss << "Invalid IP address component: " << component;
		throw std::runtime_error(oss.str());
	}
	components++;

	if (components != 4) {
		std::ostringstream oss;
		oss << "Invalid IP address format: " << _ip
			<< " (expected 4 components)";
		throw std::runtime_error(oss.str());
	}
}

bool ListenConfig::_isValidIpComponent(const std::string& component) const
{
	if (component.empty() || component.size() > 3) {
		return false;
	}
	for (size_t i = 0; i < component.size(); i++) {
		if (!std::isdigit(component[i])) {
			return false;
		}
	}
	int value = std::atoi(component.c_str());
	return (value >= 0 && value <= 255);
}
