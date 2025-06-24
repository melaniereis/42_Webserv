/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 15:05:28 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/24 14:52:37 by meferraz         ###   ########.fr       */
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
