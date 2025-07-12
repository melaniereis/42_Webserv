/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/16 11:28:41 by meferraz          #+#    #+#             */
/*   Updated: 2025/07/12 10:51:03 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../http/Request.hpp"
#include "../http/Response.hpp"
#include "../config/ServerConfig.hpp"
#include "../config/LocationConfig.hpp"
#include "../utils/Logger.hpp"
#include "CgiManager.hpp"

class Client;

class CgiHandler
{
	public:
		CgiHandler(const Request &request,
				const ServerConfig &config,
				const LocationConfig &location,
				Client& client);
		~CgiHandler();

		Response execute();
		std::string getCookies();

	private:
		Request _request;
		ServerConfig _config;
		LocationConfig _location;
		Client& _client;
		std::map<std::string, std::string> _env;

		std::string _resolveScriptPath() const;

		void _initEnv();
		std::string _executeCgiScript(const std::string &scriptPath);
		void _parseCgiOutput(const std::string &output, Response &response);
		std::string _intToString(int value) const;
		char** _createEnvArray() const;
		void _cleanupEnvArray(char** env) const;
		bool _setupPipes(int pipeIn[2], int pipeOut[2]) const;
		void _childProcess(int pipeIn[2], int pipeOut[2],
						const std::string& scriptPath);
		bool _validateScript(const std::string& scriptPath, Response& response);
		void _handleCgiError(Response& response);
		bool _setupPipes(int pipeIn[2], int pipeOut[2]);

		CgiHandler();
		CgiHandler(const CgiHandler&);
		CgiHandler& operator=(const CgiHandler&);

};
