/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 16:25:55 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/07 16:13:24 by jmeirele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(const std::string &rawRequest)
{
	std::istringstream stream(rawRequest);
	std::string line;

	if (std::getline(stream, line))
	{
		std::istringstream requestLine(line);
		requestLine >> _method >> _path >> _httpVersion;
		std::cout << "Method: " << _method << std::endl;
		std::cout << "Path: " << _path << std::endl;
		std::cout << "Http: " << _httpVersion << std::endl;
	}
}

