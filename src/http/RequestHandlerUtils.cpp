/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandlerUtils.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 17:29:28 by jmeirele          #+#    #+#             */
/*   Updated: 2025/07/23 17:32:11 by jmeirele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

std::string generateTimestampFilename(std::string &fileName)
{
	time_t now = time(NULL);
	int randomNum = rand() % 9000 + 1000;
	
	std::stringstream ss;
	
	if (fileName.empty())
		ss << "upload_" << now << "_" << randomNum << ".bin";
	else
	{
		size_t dotPos = fileName.find_last_of('.');
		std::string namePart;
		std::string extPart;
	
		if (dotPos == std::string::npos)
		{
			namePart = fileName;
			extPart = "";
		}
		else
		{
			namePart = fileName.substr(0, dotPos);
			extPart = fileName.substr(dotPos);
		}
		ss << namePart << "_" << now << "_" << randomNum << extPart;
	}
	return ss.str();
}

std::string resolveMultipleIndexes(const std::string &rootDir, const std::vector<std::string> &indexes)
{
	for (size_t i = 0; i < indexes.size(); i++)
	{
		std::string fullPath = rootDir + "/" + indexes[i];
		std::ifstream file(fullPath.c_str());
		if (file)
			return indexes[i];
	}
	return "";
}

std::string getMimeType(const std::string &extension)
{
	if (endsWith(extension, ".html"))
		return "text/html";
	if (endsWith(extension, ".jpg") || endsWith(extension, ".jpeg"))
		return "image/jpeg";
	if (endsWith(extension, ".css"))
		return "text/css";
	if (endsWith(extension, ".js"))
		return "application/javascript";
	if (endsWith(extension, ".png"))
		return "image/png";
	if (endsWith(extension, ".gif"))
		return "image/gif";
	return "application/octet-stream";
}

bool endsWith(const std::string &str, const std::string &suffix)
{
	if (str.length() < suffix.length())
		return false;
	return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

std::string extractLocationPrefix(const Request &request, const ServerConfig &config)
{
	const std::map<std::string, LocationConfig> &locations = config.getLocations();
	std::string reqPath = request.getReqPath();

	std::string bestMatch = "";
	size_t bestMatchLength = 0;

	for (std::map<std::string, LocationConfig>::const_iterator it = locations.begin(); it != locations.end(); ++it)
	{
		const std::string &locationPath = it->first;

		if (reqPath.compare(0, locationPath.length(), locationPath) == 0 &&
			(reqPath.length() == locationPath.length() || reqPath[locationPath.length()] == '/' || locationPath == "/"))
		{
			if (locationPath.length() > bestMatchLength)
			{
				bestMatch = locationPath;
				bestMatchLength = locationPath.length();
			}
		}
	}

	if (bestMatch.empty())
		return locations.count("/") ? "/" : "";

	return bestMatch;
}

std::string extractFilenameFromPath(const std::string &path)
{
	size_t pos = path.find_last_of('/');
	if (pos == std::string::npos)
		return path;
	return path.substr(pos + 1);
}

bool isMethodAllowed(const Request &request, const ServerConfig &config, const std::string &method)
{
	const std::map<std::string, LocationConfig> &locations = config.getLocations();

	std::string locationPrefix = extractLocationPrefix(request, config);

	std::map<std::string, LocationConfig>::const_iterator it = locations.find(locationPrefix);

	if (it == locations.end())
		return false;

	const std::vector<std::string> &methods = it->second.getAllowedMethods();

	for (size_t i = 0; i < methods.size(); ++i)
	{
		if (methods[i] == method)
			return true;
	}
	return false;
}

void finalizePart(std::vector<MultipartPart> &parts, MultipartPart &part, std::vector<char> &buffer)
{
	if (!part.name.empty() || !buffer.empty())
	{
		part.content = buffer;
		parts.push_back(part);
		part = MultipartPart();
		buffer.clear();
	}
}

void parseContentDisposition(const std::string &line, MultipartPart &part)
{
	size_t namePos = line.find("name=\"");
	if (namePos != std::string::npos)
	{
		size_t nameEnd = line.find("\"", namePos + 6);
		part.name = line.substr(namePos + 6, nameEnd - (namePos + 6));
	}
	
	size_t filePos = line.find("filename=\"");
	if (filePos != std::string::npos)
	{
		size_t fileEnd = line.find("\"", filePos + 10);
		part.fileName = line.substr(filePos + 10, fileEnd - (filePos + 10));
	}
}

bool isDirectory(const std::string &path)
{
	struct stat s;

	if (stat(path.c_str(), &s) == 0)
		return S_ISDIR(s.st_mode);
	return false;
}

std::string normalizeReqPath(const std::string &path)
{
	if (path.empty())
		return path;

	std::string normalized = path;
	
	while (normalized.length() > 1 && normalized[normalized.length() - 1] == '/')
		normalized.erase(normalized.length() - 1);
	
	return normalized;
}

std::string generateAutoindexPage(const std::string &dirPath, const std::string &reqPath)
{
	DIR *dir = opendir(dirPath.c_str());
	if (!dir)
		return "<html><body><h1>Forbidden</h1></body></html>";

	std::string html = "<html><body><h1>Index of " + reqPath + "</h1><ul>";

	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL)
	{
		std::string name(entry->d_name);
		html += "<li><a href=\"" + reqPath + name + "\">" + name + "</a></li>";
	}
	closedir(dir);

	html += "</ul></body></html>";
	return html;
}

