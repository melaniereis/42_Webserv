/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 16:25:55 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/06 16:32:33 by jmeirele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(const std::string &rawRequest)
{
	std::istringstream stream(rawRequest);
	std::string line;
	while (std::getline(stream, line))
	{
		std::cout << "here > " << line << std::endl;
	}
}

