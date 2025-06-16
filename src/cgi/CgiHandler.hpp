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
	Request _request;
	ServerConfig _config;
	LocationConfig _location;
	std::string _cgiPath;
	std::string _scriptPath;

	char **_prepareEnvironment();
	void _cleanupEnvironment(char **env);
	std::string _executeScript(char **env);
	std::string _joinPath(const std::string &a, const std::string &b);
	bool _isSafePath(const std::string &path);
	bool _isValidScript();
};
