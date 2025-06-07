/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 16:20:12 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/07 16:01:15 by jmeirele         ###   ########.fr       */
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
		std::string _body;
		std::map<std::string, std::string> _headers;
};