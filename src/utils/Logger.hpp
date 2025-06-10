/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 15:05:37 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/06 15:45:40 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../../inc/webserv.hpp"

class Logger
{
	public:
		static void info(const std::string &msg);
		static void warn(const std::string &msg);
		static void error(const std::string &msg);
		static void debug(const std::string &msg);

	private:
};
