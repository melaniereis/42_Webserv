/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandlerUtils.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 17:29:28 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/25 18:53:11 by jmeirele         ###   ########.fr       */
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

std::string extractLocationPrefix(const Request &request, const ServerConfig& config)
{
	const std::map<std::string, LocationConfig> &locations = config.getLocations();
	std::string reqPath = request.getReqPath();
	std::string bestMatch = "";
	size_t bestMatchLength = 0;

	for (std::map<std::string, LocationConfig>::const_iterator it = locations.begin(); it != locations.end(); ++it)
	{
		const std::string& locationPath = it->first;

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
	return bestMatch;
}

std::string extractFilenameFromPath(const std::string &path)
{
	size_t pos = path.find_last_of('/');
	if (pos == std::string::npos)
		return path;
	return path.substr(pos + 1);
}