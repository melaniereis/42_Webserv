/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 18:19:04 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/12 18:17:32 by jmeirele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Request.hpp"
#include "Response.hpp"
#include "../config/ServerConfig.hpp"
#include "HttpStatus.hpp"

class RequestHandler
{
	public:
		static Response handle(const Request &request, const ServerConfig &config);
		static Response handleGetMethod(const Request &request, const ServerConfig &config);
		static Response handlePostMethod(const Request &request, const ServerConfig &config);
		static Response handleDeleteMethod(const Request &request);

		private:
};

//Multipurpose Internet Mail Extensions
std::string getMimeType(const std::string &extension);
bool endsWith(const std::string &str, const std::string &suffix);

// Handling multiple indexes of server indexes
std::string resolveMultipleIndexes(const std::string &rootDir, const std::vector<std::string> &indexes);
