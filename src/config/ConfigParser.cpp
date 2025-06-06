/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 16:50:07 by meferraz          #+#    #+#             */
/*   Updated: 2025/06/06 17:32:29 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"

ConfigParser::ConfigParser(const std::string& path) : _path(path)
{
	_initHandlers();
}

std::vector<ServerConfig> ConfigParser::parse()
{
	std::string content = _readFile();
	return _parseConfig(content);
}

std::string ConfigParser::_readFile()
{
	std::ifstream file(_path.c_str());
	if (!file.is_open())
		throw std::runtime_error("Could not open config file: " + _path);

	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

void ConfigParser::_initHandlers()
{
	_serverHandlers["listen"] = &ConfigParser::_handleListen;
	_serverHandlers["server_name"] = &ConfigParser::_handleServerName;
	_serverHandlers["root"] = &ConfigParser::_handleRoot;
	_serverHandlers["index"] = &ConfigParser::_handleIndex;
	_serverHandlers["error_page"] = &ConfigParser::_handleErrorPage;
	_serverHandlers["client_max_body_size"] = &ConfigParser::_handleMaxBodySize;

	_locationHandlers["root"] = &ConfigParser::_handleLocRoot;
	_locationHandlers["index"] = &ConfigParser::_handleLocIndex;
	_locationHandlers["autoindex"] = &ConfigParser::_handleAutoIndex;
	_locationHandlers["allow_methods"] = &ConfigParser::_handleAllowMethods;
	_locationHandlers["upload_dir"] = &ConfigParser::_handleUploadDir;
	_locationHandlers["return"] = &ConfigParser::_handleLocReturn;
	_locationHandlers["cgi"] = &ConfigParser::_handleCgi;
}

std::string ConfigParser::trim(const std::string &s)
{
	size_t first = s.find_first_not_of(" \t\r\n");
	size_t last  = s.find_last_not_of(" \t\r\n");
	if (first == std::string::npos || last == std::string::npos)
		return "";
	return s.substr(first, last - first + 1);
}

std::string ConfigParser::intToString(int v)
{
	std::ostringstream oss;
	oss << v;
	return oss.str();
}

std::vector<ServerConfig> ConfigParser::_parseConfig(const std::string& content)
{
	std::vector<ServerConfig> servers;
	size_t pos = 0;
	int lineNum = 0;

	while (pos < content.size())
	{
		size_t nextLine = content.find('\n', pos);
		std::string line = content.substr(pos, nextLine - pos);
		line = trim(line);

		if (line.empty() || line[0] == '#') {
			pos = nextLine + 1;
			lineNum++;
			continue;
		}

		if (line.find("server") == 0) {
			_parseServerBlock(line, lineNum, content, pos, servers);
		} else if (line.find("location") == 0) {
			if (!servers.empty()) {
				_parseLocationBlock(line, lineNum, content, pos, servers.back());
			} else {
				throw std::runtime_error("Location block found without a server block at line " + intToString(lineNum));
			}
		} else {
			throw std::runtime_error("Unknown directive at line " + intToString(lineNum));
		}

		pos = nextLine == std::string::npos ? content.size() : nextLine + 1;
		lineNum++;
	}

	return servers;
}
