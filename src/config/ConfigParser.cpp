/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 16:50:07 by meferraz          #+#    #+#             */
/*   Updated: 2025/06/07 08:58:12 by meferraz         ###   ########.fr       */
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
	std::istringstream stream(content);
	return _parseConfig(stream);
}

std::string ConfigParser::_readFile()
{
	std::ifstream file(_path.c_str());
	if (!file.is_open())
		throw std::runtime_error("Failed to open config file: " + _path);

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

std::vector<ServerConfig> ConfigParser::_parseConfig(std::istringstream& stream) {
	std::vector<ServerConfig> servers;
	std::string               rawLine;
	int                       lineNum = 0;

	while (std::getline(stream, rawLine))
	{
		++lineNum;
		std::string line = trim(rawLine);
		if (line.empty() || line[0] == '#')
			continue;

		std::istringstream iss(line);
		std::string        token;
		iss >> token;

		if (token == "server")
		{
			// Pass entire line ("server {") to parseServerBlock
			_parseServerBlock(line, lineNum, stream, servers);
		}
		else
		{
			throw std::runtime_error
			(
				"Line " + intToString(lineNum) +
				": Unexpected token '" + token + "'"
			);
		}
	}

	return servers;
}

void ConfigParser::_parseServerBlock(const std::string&       firstLine,
									int&                     lineNum,
									std::istringstream&      stream,
									std::vector<ServerConfig>& servers)
{
	// firstLine should be "server {"
	std::istringstream iline(firstLine);
	std::string        word;
	iline >> word; // "server"
	iline >> word; // should be "{"
	if (word != "{")
		throw std::runtime_error(
			"Line " + intToString(lineNum) +
			": Expected '{' after 'server'"
		);

	ServerConfig               currentConfig;
	std::vector<LocationConfig> locations;
	int                        braceCount = 1;
	std::string                rawLine;

	// Read until matching closing brace
	while (braceCount > 0 && std::getline(stream, rawLine)) {
		++lineNum;
		std::string line = trim(rawLine);
		if (line.empty() || line[0] == '#')
			continue;

		if (line == "{") {
			++braceCount;
		}
		else if (line == "}") {
			--braceCount;
			if (braceCount == 0) {
				// Attach any collected locations
				for (size_t i = 0; i < locations.size(); ++i) {
					currentConfig.addLocation(locations[i]);
				}
				// Merge or append this server config
				bool merged = false;
				for (size_t i = 0; i < servers.size(); ++i) {
					if (servers[i].getServerPort() ==
						currentConfig.getServerPort()) {
						if (servers[i].getServerName() ==
							currentConfig.getServerName()) {
							throw std::runtime_error(
								"Line " + intToString(lineNum) +
								": Duplicate server_name on same port"
							);
						}
						servers[i] = currentConfig;
						merged = true;
						break;
					}
				}
				if (!merged)
					servers.push_back(currentConfig);
				return;
			}
		}
		else {
			// Inside server block: split directive key + remainder
			std::istringstream ls(line);
			std::string        key;
			ls >> key;
			std::string        remainder;
			std::getline(ls, remainder);
			remainder = trim(remainder);

			std::map<std::string, ServerDirHandler>::iterator it =
				_serverHandlers.find(key);
			if (it != _serverHandlers.end()) {
				ServerDirHandler handler = it->second;
				(this->*handler)(remainder, currentConfig, lineNum);
			}
			else if (key == "location") {
				// Pass “location <path> {” to nested parser
				_parseLocationBlock(line, lineNum, stream, currentConfig);
			}
			else {
				throw std::runtime_error(
					"Line " + intToString(lineNum) +
					": Unknown directive '" + key +
					"' in server block"
				);
			}
		}
	}

	// If we exit without matching braces
	if (braceCount != 0) {
		throw std::runtime_error(
			"Line " + intToString(lineNum) +
			": Unexpected end of server block"
		);
	}
}

void ConfigParser::_parseLocationBlock(const std::string& firstLine,
									int&               lineNum,
									std::istringstream& stream,
									ServerConfig&       currentConfig)
{
	// firstLine e.g. "location /images/ {"
	std::istringstream iline(firstLine);
	std::string        keyword, path, brace;
	iline >> keyword; // "location"
	iline >> path;    // e.g. "/images/"
	iline >> brace;   // should be "{"
	if (brace != "{")
		throw std::runtime_error(
			"Line " + intToString(lineNum) +
			": Expected '{' after location " + path
		);

	LocationConfig loc;
	loc.setPath(path);
	int            braceCount = 1;
	std::string    rawLine;

	while (braceCount > 0 && std::getline(stream, rawLine)) {
		++lineNum;
		std::string line = trim(rawLine);
		if (line.empty() || line[0] == '#')
			continue;

		if (line == "{") {
			++braceCount;
		}
		else if (line == "}") {
			--braceCount;
			if (braceCount == 0) {
				currentConfig.addLocation(loc);
				return;
			}
		}
		else {
			// Split key + remainder
			std::istringstream ls(line);
			std::string        key;
			ls >> key;
			std::string        remainder;
			std::getline(ls, remainder);
			remainder = trim(remainder);

			std::map<std::string, LocationDirHandler>::iterator it =
				_locationHandlers.find(key);
			if (it != _locationHandlers.end()) {
				LocationDirHandler handler = it->second;
				(this->*handler)(remainder, loc, lineNum);
			}
			else {
				throw std::runtime_error(
					"Line " + intToString(lineNum) +
					": Unknown directive '" + key +
					"' in location block"
				);
			}
		}
	}

	if (braceCount != 0) {
		throw std::runtime_error(
			"Line " + intToString(lineNum) +
			": Unexpected end of location block"
		);
	}
}

void ConfigParser::_handleListen(const std::string& args,
								ServerConfig&      cfg,
								int                lineNum)
{
	// args e.g. "127.0.0.1:8080"
	size_t colon = args.find(':');
	if (colon == std::string::npos)
		throw std::runtime_error(
			"Line " + intToString(lineNum) +
			": Invalid listen syntax"
		);

	std::string host = args.substr(0, colon);
	int         port = std::atoi(args.substr(colon + 1).c_str());
	if (port <= 0)
		throw std::runtime_error(
			"Line " + intToString(lineNum) +
			": Invalid port number"
		);

	cfg.setHost(host);
	cfg.setPort(port);
}

void ConfigParser::_handleServerName(const std::string& args,
									ServerConfig&      cfg,
									int                /*lineNum*/)
{
	cfg.setName(args);
}

void ConfigParser::_handleRoot(const std::string& args,
							ServerConfig&      cfg,
							int                /*lineNum*/)
{
	cfg.setRoot(args);
}

void ConfigParser::_handleIndex(const std::string& args,
								ServerConfig&      cfg,
								int                /*lineNum*/)
{
	cfg.setIndex(args);
}

void ConfigParser::_handleErrorPage(const std::string& args,
									ServerConfig&      cfg,
									int                lineNum)
{
	// args e.g. "404 /404.html"
	std::istringstream ss(args);
	int                code;
	std::string        path;
	ss >> code >> path;
	if (ss.fail() || !ss.eof())
		throw std::runtime_error(
			"Line " + intToString(lineNum) +
			": Invalid error_page syntax"
		);
	cfg.addErrorPage(code, path);
}

void ConfigParser::_handleMaxBodySize(const std::string& args,
									ServerConfig&      cfg,
									int                lineNum)
{
	// args e.g. "1000000"
	std::istringstream ss(args);
	size_t             sz;
	ss >> sz;
	if (ss.fail() || !ss.eof())
		throw std::runtime_error(
			"Line " + intToString(lineNum) +
			": Invalid client_max_body_size"
		);
	cfg.setMaxBodySize(sz);
}

void ConfigParser::_handleLocRoot(const std::string& args,
								LocationConfig&    loc,
								int                /*lineNum*/)
{
	loc.setRoot(args);
}

void ConfigParser::_handleLocIndex(const std::string& args,
								LocationConfig&    loc,
								int                /*lineNum*/)
{
	loc.addIndex(args);
}

void ConfigParser::_handleAutoIndex(const std::string& args,
									LocationConfig&    loc,
									int                lineNum)
{
	if (args == "on")       loc.setAutoIndex(true);
	else if (args == "off") loc.setAutoIndex(false);
	else
		throw std::runtime_error(
			"Line " + intToString(lineNum) +
			": Invalid autoindex value"
		);
}

void ConfigParser::_handleAllowMethods(const std::string& args,
									LocationConfig&    loc,
									int                /*lineNum*/)
{
	// args e.g. "GET POST DELETE"
	std::istringstream ss(args);
	std::string        m;
	while (ss >> m) {
		loc.addAllowedMethod(m);
	}
}

void ConfigParser::_handleUploadDir(const std::string& args,
									LocationConfig&    loc,
									int                /*lineNum*/)
{
	loc.setUploadDir(args);
}

void ConfigParser::_handleLocReturn(const std::string& args,
									LocationConfig&    loc,
									int                lineNum)
{
	// args e.g. "302 /"
	std::istringstream ss(args);
	int                code;
	std::string        target;
	ss >> code >> target;
	if (ss.fail() || !ss.eof())
		throw std::runtime_error(
			"Line " + intToString(lineNum) +
			": Invalid return syntax"
		);
	loc.addRedirect(code, target);
}

void ConfigParser::_handleCgi(const std::string& args,
							LocationConfig&    loc,
							int                lineNum)
{
	// args e.g. ".py /usr/bin/python"
	std::istringstream ss(args);
	std::string        ext, cgiPath;
	ss >> ext >> cgiPath;
	if (ss.fail() || !ss.eof())
		throw std::runtime_error(
			"Line " + intToString(lineNum) +
			": Invalid cgi syntax"
		);
	loc.addCgi(ext, cgiPath);
}
