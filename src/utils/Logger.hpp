/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 15:05:37 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/04 15:27:10 by jmeirele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "inc/webserv.hpp"

class Logger
{
	public:
		static void info(const std::string &msg);
		static void warn(const std::string &msg);
		static void error(const std::string &msg);
		static void debug(const std::string &msg);

	private:
};