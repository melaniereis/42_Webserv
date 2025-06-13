/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListenConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 17:43:57 by meferraz          #+#    #+#             */
/*   Updated: 2025/06/13 21:54:59 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../../inc/webserv.hpp"

class ListenConfig
{
	public:
		ListenConfig();
		explicit ListenConfig(const std::string& token);

		const std::string& getIp() const;
		unsigned int getPort() const;
		const std::string& getIpPortJoin() const;

	private:
		std::string _ip;
		unsigned int _port;
		std::string _ipPortJoin;

		void _parseToken(const std::string& token);
		void _validateIp() const;
		void _validatePort() const;
		bool _isValidIpComponent(const std::string& component) const;
};
