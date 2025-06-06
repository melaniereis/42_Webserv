/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 16:20:12 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/06 16:30:06 by jmeirele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../../inc/webserv.hpp"

class Request
{
	public:
		Request(const std::string &rawRequest);

	private:
		std::string _method;
		std::string _path;
		std::string _httpVersion;
		std::map<std::string, std::string> _headers;
		std::string _body;
};