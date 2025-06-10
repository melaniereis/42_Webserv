/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 15:51:54 by meferraz          #+#    #+#             */
/*   Updated: 2025/06/09 17:46:19 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../../inc/webserv.hpp"
#include "LocationConfig.hpp"
#include "ServerConfig.hpp"

class ConfigParser
{
	public:
		ConfigParser(const std::string& path);
		std::vector<ServerConfig> parse();

	private:
		typedef void (ConfigParser::*ServerDirHandler)(const std::string& args,
													ServerConfig& config,
													int lineNum);
		typedef void (ConfigParser::*LocationDirHandler)(const std::string& args,
														LocationConfig& loc,
														int lineNum);

		struct ServerKey
		{
			std::string host;
			int port;
			std::set<std::string> names;

			bool operator<(const ServerKey& other) const
			{
				if (port != other.port)  return port < other.port;
				if (names != other.names)  return names < other.names;
				return host < other.host;
			}
		};

		std::string _path;
		std::map<std::string, ServerDirHandler> _serverHandlers;
		std::map<std::string, LocationDirHandler> _locationHandlers;

		std::string _readFile();
		std::vector<ServerConfig> _parseConfig(std::istringstream& stream);
		void _parseServerBlock(const std::string& firstLine, int& lineNum,
							std::istringstream& stream,
							std::map<ServerKey, ServerConfig>& servers);
		void _parseLocationBlock(const std::string& firstLine, int& lineNum,
								std::istringstream& stream,
								ServerConfig& currentConfig);

		// Server directive handlers
		void _handleListen(const std::string& args, ServerConfig& cfg, int lineNum);
		void _handleServerName(const std::string& args, ServerConfig& cfg, int lineNum);
		void _handleRoot(const std::string& args, ServerConfig& cfg, int lineNum);
		void _handleIndex(const std::string& args, ServerConfig& cfg, int lineNum);
		void _handleErrorPage(const std::string& args, ServerConfig& cfg, int lineNum);
		void _handleClientMaxBodySize(const std::string& args, ServerConfig& cfg, int lineNum);

		// Location directive handlers
		void _handleLocRoot(const std::string& args, LocationConfig& loc, int lineNum);
		void _handleLocIndex(const std::string& args, LocationConfig& loc, int lineNum);
		void _handleAutoIndex(const std::string& args, LocationConfig& loc, int lineNum);
		void _handleAllowMethods(const std::string& args, LocationConfig& loc, int lineNum);
		void _handleUploadDir(const std::string& args, LocationConfig& loc, int lineNum);
		void _handleLocReturn(const std::string& args, LocationConfig& loc, int lineNum);
		void _handleCgi(const std::string& args, LocationConfig& loc, int lineNum);

		// Helpers
		void _initHandlers();
		static std::string trim(const std::string& s);
		static std::string intToString(int v);
};
