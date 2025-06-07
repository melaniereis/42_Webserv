/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 16:50:07 by meferraz          #+#    #+#             */
/*   Updated: 2025/06/07 18:25:49 by meferraz         ###   ########.fr       */
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
	// C++98 compliant map initialization
	_serverHandlers.insert(std::make_pair("listen", &ConfigParser::_handleListen));
	_serverHandlers.insert(std::make_pair("server_name", &ConfigParser::_handleServerName));
	_serverHandlers.insert(std::make_pair("root", &ConfigParser::_handleRoot));
	_serverHandlers.insert(std::make_pair("index", &ConfigParser::_handleIndex));
	_serverHandlers.insert(std::make_pair("error_page", &ConfigParser::_handleErrorPage));
	_serverHandlers.insert(std::make_pair("client_max_body_size", &ConfigParser::_handleMaxBodySize));

	_locationHandlers.insert(std::make_pair("root", &ConfigParser::_handleLocRoot));
	_locationHandlers.insert(std::make_pair("index", &ConfigParser::_handleLocIndex));
	_locationHandlers.insert(std::make_pair("autoindex", &ConfigParser::_handleAutoIndex));
	_locationHandlers.insert(std::make_pair("allow_methods", &ConfigParser::_handleAllowMethods));
	_locationHandlers.insert(std::make_pair("upload_dir", &ConfigParser::_handleUploadDir));
	_locationHandlers.insert(std::make_pair("return", &ConfigParser::_handleLocReturn));
	_locationHandlers.insert(std::make_pair("cgi", &ConfigParser::_handleCgi));
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

std::vector<ServerConfig> ConfigParser::_parseConfig(std::istringstream& stream)
{
	std::map<ServerKey, ServerConfig> serverMap;
	std::string line;
	int lineNum = 0;

	while (std::getline(stream, line))
	{
		++lineNum;
		std::string trimmed = trim(line);
		if (trimmed.empty() || trimmed[0] == '#')
			continue;

		std::istringstream iss(trimmed);
		std::string token;
		iss >> token;
		if (token == "server")
			_parseServerBlock(trimmed, lineNum, stream, serverMap);
		else
		{
			throw std::runtime_error
			(
				"Line " + intToString(lineNum)
				+ ": Unexpected token '"
				+ token + "'"
			);
		}
	}

	std::vector <ServerConfig> servers;
	std::map<ServerKey, ServerConfig>::iterator it = serverMap.begin();
	while (it != serverMap.end())
	{
		servers.push_back(it->second);
		++it;
	}
	return servers;
}

void ConfigParser::_parseServerBlock(const std::string& firstLine, int& lineNum,
									std::istringstream& stream,
									std::map<ServerKey, ServerConfig>& servers)
{
	std::istringstream iline(firstLine);
	std::string token;

	iline >> token;
	iline >> token;
	if (token != "{")
		throw std::runtime_error("Line " + intToString(lineNum) + ": Expected '{' after server");

	ServerConfig currentConfig;
	int braceCount = 1;
	std::string rawLine;

	while (braceCount > 0 && std::getline(stream, rawLine))
	{
		++lineNum;
		std::string line = trim(rawLine);
		if (line.empty() || line[0] == '#')
			continue;

		if (line == "{")
			++braceCount;
		else if (line == "}")
		{
			--braceCount;
			if (braceCount == 0)
			{
				if (currentConfig.getServerPort() == 0)
					throw std::runtime_error("Line " + intToString(lineNum) + ": Missing 'listen' directive");

				ServerKey key;
				key.host = currentConfig.getServerHost();
				key.port = currentConfig.getServerPort();

				// Temporary workaround for server names
				std::vector<std::string> serverNames;
				serverNames.push_back(currentConfig.getServerName());
				key.names.insert(serverNames.begin(), serverNames.end());

				if (servers.find(key) != servers.end())
					throw std::runtime_error("Line " + intToString(lineNum) + ": Duplicate server");
				servers[key] = currentConfig;
				return;
			}
		}
		else
		{
			std::istringstream ls(line);
			std::string key;
			ls >> key;
			std::string remainder;
			std::getline(ls, remainder);
			remainder = trim(remainder);

			std::map<std::string, ServerDirHandler>::iterator it =
				_serverHandlers.find(key);
			if (it != _serverHandlers.end())
				(this->*(it->second))(remainder, currentConfig, lineNum); // Fixed member function pointer call
			else if (key == "location")
			{
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

	if (braceCount != 0)
	{
		throw std::runtime_error(
			"Line " + intToString(lineNum) +
			": Unexpected end of server block"
		);
	}
}

// Removed extra semicolon here
void ConfigParser::_parseLocationBlock(const std::string& firstLine, int& lineNum,
										std::istringstream& stream,
										ServerConfig& currentConfig)
{
	std::istringstream iline(firstLine);
	std::string keyword, path, brace;

	iline >> keyword;
	iline >> path;
	iline >> brace;

	if (brace != "{")
	{
		throw std::runtime_error
		(
			"Line " + intToString(lineNum) +
			": Expected '{' after location " + path
		);
	}

	LocationConfig loc;
	loc.setPath(path);
	int braceCount = 1;
	std::string rawLine;

	while (braceCount > 0 && std::getline(stream, rawLine))
	{
		++lineNum;
		std::string line = trim(rawLine);
		if (line.empty() || line[0] == '#')
			continue;

		if (line == "{")
			++braceCount;
		else if (line == "}") {
			--braceCount;
			if (braceCount == 0)
			{
				currentConfig.addLocation(loc);
				return;
			}
		}
		else
		{
			std::istringstream ls(line);
			std::string        key;
			ls >> key;
			std::string        remainder;
			std::getline(ls, remainder);
			remainder = trim(remainder);

			std::map<std::string, LocationDirHandler>::iterator it =
				_locationHandlers.find(key);
			if (it != _locationHandlers.end())
				(this->*(it->second))(remainder, loc, lineNum);
			else
			{
				throw std::runtime_error(
					"Line " + intToString(lineNum) +
					": Unknown directive '" + key +
					"' in location block"
				);
			}
		}
	}

	currentConfig.addLocation(loc);

	if (braceCount != 0)
	{
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
	size_t colon = args.find(':');
	if (colon == std::string::npos)
		throw std::runtime_error(
			"Line " + intToString(lineNum) +
			": Invalid listen syntax"
		);

	std::string host = args.substr(0, colon);
	int         port = std::atoi(args.substr(colon + 1).c_str());
	if (port <= 0 || port > 65535)
		throw std::runtime_error(
			"Line " + intToString(lineNum) +
			": Invalid port number (must be 1-65535)"
		);

	cfg.setHost(host);
	cfg.setPort(port);
}

void ConfigParser::_handleServerName(const std::string& args,
									ServerConfig&      cfg,
									int                lineNum)
{
	std::istringstream ss(args);
	std::string        name;

	// Temporary implementation for single server name
	if (ss >> name) {
		cfg.setName(name);
	}

	if (cfg.getServerName().empty())
	{
		throw std::runtime_error
		(
			"Line " + intToString(lineNum) +
			": server_name requires a name"
		);
	}
}

void ConfigParser::_handleRoot(const std::string& args,
							ServerConfig&      cfg,
							int                lineNum)
{
	(void)lineNum;
	cfg.setRoot(args);
}

void ConfigParser::_handleIndex(const std::string& args,
								ServerConfig&      cfg,
								int                lineNum)
{
	// Convert string to vector
	(void)lineNum;

	std::istringstream ss(args);
	std::vector<std::string> indexes;
	std::string index;
	while (ss >> index) {
		indexes.push_back(index);
	}
	cfg.setIndex(indexes);
}

void ConfigParser::_handleErrorPage(const std::string& args,
									ServerConfig&      cfg,
									int                lineNum)
{
	std::istringstream ss(args);
	int                code;
	std::string        path;
	ss >> code >> path;
	if (ss.fail() || !ss.eof())
		throw std::runtime_error
		(
			"Line " + intToString(lineNum) +
			": Invalid error_page syntax"
		);
	cfg.setNotFound(path);
}

void ConfigParser::_handleMaxBodySize(const std::string& args,
									ServerConfig&      cfg,
									int                lineNum)
{
	(void)cfg;
	std::istringstream ss(args);
	size_t             sz;
	ss >> sz;
	if (ss.fail() || !ss.eof())
		throw std::runtime_error
		(
			"Line " + intToString(lineNum) +
			": Invalid client_max_body_size"
		);
	// Assuming setClientBodySize exists
	// cfg.setClientBodySize(sz);
}

void ConfigParser::_handleLocRoot(const std::string& args,
								LocationConfig&    loc,
								int                lineNum)
{
	(void)lineNum;
	loc.setRoot(args);
}

void ConfigParser::_handleLocIndex(const std::string& args,
								LocationConfig&    loc,
								int                lineNum)
{
	(void)lineNum;
	loc.addIndex(args);
}

void ConfigParser::_handleAutoIndex(const std::string& args,
									LocationConfig&    loc,
									int                lineNum)
{
	if (args == "on")       loc.setAutoIndex(true);
	else if (args == "off") loc.setAutoIndex(false);
	else
		throw std::runtime_error
		(
			"Line " + intToString(lineNum) +
			": Invalid autoindex value"
		);
}

void ConfigParser::_handleAllowMethods(const std::string& args,
									LocationConfig&    loc,
									int                /*lineNum*/)
{
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
	std::istringstream ss(args);
	int                code;
	std::string        target;
	ss >> code >> target;
	if (ss.fail() || !ss.eof())
		throw std::runtime_error
		(
			"Line " + intToString(lineNum) +
			": Invalid return syntax"
		);
	loc.addRedirect(code, target);
}

void ConfigParser::_handleCgi(const std::string& args,
							LocationConfig&    loc,
							int                lineNum)
{
	std::istringstream ss(args);
	std::string        ext, cgiPath;
	ss >> ext >> cgiPath;

	if (ext.empty() || ext[0] != '.')
	{
		throw std::runtime_error
		(
			"Line " + intToString(lineNum) +
			": Invalid CGI extension (must start with '.')"
		);
	}
	if (access(cgiPath.c_str(), X_OK) != 0)
	{
		throw std::runtime_error
		(
			"Line " + intToString(lineNum) +
			": CGI path '" + cgiPath +
			"' is not executable"
		);
	}
	loc.addCgi(ext, cgiPath);
}
