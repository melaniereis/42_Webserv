/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListenConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 17:43:57 by meferraz          #+#    #+#             */
/*   Updated: 2025/06/09 17:44:30 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../../inc/webserv.hpp"

class ListenConfig
{
	public:
		ListenConfig();
		ListenConfig(const std::string& token);

		const std::string& getIp() const;
		unsigned int getPort() const;
		const std::string getIpPortJoin() const;

	private:
		std::string _ip;
		unsigned int _port;
		std::string _ipPortJoin;

		bool checkIpPort();
		bool isNotInRangeIp(const std::string& str, int j, int i);
};
