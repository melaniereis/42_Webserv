/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 15:51:54 by meferraz          #+#    #+#             */
/*   Updated: 2025/06/06 16:48:51 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../../inc/webserv.hpp"
#include "ServerConfig.hpp"
#include "LocationConfig.hpp"


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

		std::string _path;
		std::map<std::string, ServerDirHandler> _serverHandlers;
		std::map<std::string, LocationDirHandler> _locationHandlers;

		std::string _readFile();
		std::vector<ServerConfig> _parseConfig(const std::string& content);
		void _parseServerBlock(const std::string& firstLine,
							int& lineNum,
							const std::string& content,
							size_t& pos,
							std::vector<ServerConfig>& servers);
		void _parseLocationBlock(const std::string& firstLine,
								int& lineNum,
								const std::string& content,
								size_t& pos,
								ServerConfig& currentConfig);

		// Handler registration
		void _initHandlers();

		// Server directive handlers
		void _handleListen(const std::string& args, ServerConfig& cfg, int lineNum);
		void _handleServerName(const std::string& args, ServerConfig& cfg, int lineNum);
		void _handleRoot(const std::string& args, ServerConfig& cfg, int lineNum);
		void _handleIndex(const std::string& args, ServerConfig& cfg, int lineNum);
		void _handleErrorPage(const std::string& args, ServerConfig& cfg, int lineNum);
		void _handleMaxBodySize(const std::string& args, ServerConfig& cfg, int lineNum);

		// Location directive handlers
		void _handleLocRoot(const std::string& args, LocationConfig& loc, int lineNum);
		void _handleLocIndex(const std::string& args, LocationConfig& loc, int lineNum);
		void _handleAutoIndex(const std::string& args, LocationConfig& loc, int lineNum);
		void _handleAllowMethods(const std::string& args, LocationConfig& loc, int lineNum);
		void _handleUploadDir(const std::string& args, LocationConfig& loc, int lineNum);
		void _handleLocReturn(const std::string& args, LocationConfig& loc, int lineNum);
		void _handleCgi(const std::string& args, LocationConfig& loc, int lineNum);

		// Helpers
		static std::string trim(const std::string& s);
		static std::string intToString(int v);
};
