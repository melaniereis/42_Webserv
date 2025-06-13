/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 15:51:54 by meferraz          #+#    #+#             */
/*   Updated: 2025/06/13 22:02:58 by meferraz         ###   ########.fr       */
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

		// Public typedefs for handler maps
		typedef void (ConfigParser::*ServerDirHandler)(const std::string& args,
													ServerConfig& config,
													int lineNum);
		typedef void (ConfigParser::*LocationDirHandler)(const std::string& args,
														LocationConfig& loc,
														int lineNum);
		typedef std::map<std::string, ServerDirHandler> ServerHandlerMap;
		typedef std::map<std::string, LocationDirHandler> LocationHandlerMap;

	private:
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
		ServerHandlerMap _serverHandlers;
		LocationHandlerMap _locationHandlers;

		std::string _readFile();
		std::vector<ServerConfig> _parseConfig(std::istringstream& stream);
		void _parseServerBlock(const std::string& firstLine, int& lineNum,
							std::istringstream& stream,
							std::map<ServerKey, ServerConfig>& servers);
		void _parseLocationBlock(const std::string& firstLine, int& lineNum,
								std::istringstream& stream,
								ServerConfig& currentConfig);
		void _processServerLine(const std::string& line, int lineNum,
							std::istringstream& stream,
							ServerConfig& currentConfig);
		void _processLocationLine(const std::string& line, int lineNum,
								std::istringstream& stream,
								LocationConfig& loc);
		void _finalizeServerBlock(int lineNum, ServerConfig& currentConfig,
								std::map<ServerKey, ServerConfig>& servers);
		void _throwError(int lineNum, const std::string& msg) const;

		// Server directive handlers
		void _handleListen(const std::string& args, ServerConfig& cfg, int lineNum);
		void _handleServerName(const std::string& args, ServerConfig& cfg, int lineNum);
		void _handleRoot(const std::string& args, ServerConfig& cfg, int lineNum);
		void _handleIndex(const std::string& args, ServerConfig& cfg, int lineNum);
		void _handleErrorPage(const std::string& args, ServerConfig& cfg, int lineNum);
		void _handleClientMaxBodySize(const std::string& args,
									ServerConfig& cfg, int lineNum);

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
