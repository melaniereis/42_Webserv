/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpStatus.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 18:06:00 by jmeirele          #+#    #+#             */
/*   Updated: 2025/08/11 15:15:41 by jmeirele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../../inc/webserv.hpp"
#include "Response.hpp"
#include "../config/ServerConfig.hpp"
#include "../utils/Logger.hpp"
#include "RequestHandler.hpp"

class HttpStatus
{
	public:
		static std::string getMessage(int code);
		static std::string generateHtmlBody(int code);
		static Response buildResponse(const ServerConfig &config, Response &response, int code);
	private:
};
