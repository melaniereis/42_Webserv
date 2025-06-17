/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/16 11:28:41 by meferraz          #+#    #+#             */
/*   Updated: 2025/06/16 14:30:59 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../http/Request.hpp"
#include "../http/Response.hpp"
#include "../config/ServerConfig.hpp"
#include "../config/LocationConfig.hpp"
#include "../utils/Logger.hpp"

class HttpStatus;

class CgiHandler
{
	public:
		CgiHandler(const Request &request, const ServerConfig &config, const LocationConfig &location);
		~CgiHandler();

		Response execute();
	private:
		std::string _resolveScriptPath();
		void _initEnv();
		std::string _executeCgiScript(const std::string &scriptPath);
		void _parseCgiOutput(const std::string &output, Response &response);
		std::string _intToString(int value) const;

		Request _request;
		ServerConfig _config;
		LocationConfig _location;
		std::map<std::string, std::string> _env; // Environment variables for CGI
};
