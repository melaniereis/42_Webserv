/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 15:05:28 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/04 15:27:19 by jmeirele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Logger.hpp"

void Logger::info(const std::string &msg)
{
	std::cout << "[INFO] " << msg << std::endl;
}

void Logger::warn(const std::string &msg)
{
	std::cout << "[WARN] " << msg << std::endl;
}

void Logger::error(const std::string &msg)
{
	std::cout << "[ERROR] " << msg << std::endl;
}

void Logger::debug(const std::string &msg)
{
	std::cout << "[DEBUG] " << msg << std::endl;
}
