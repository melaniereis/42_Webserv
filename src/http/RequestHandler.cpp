/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 18:31:25 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/25 22:45:57 by jmeirele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

// Helper function to find the best matching location for a request
const LocationConfig *findMatchingLocation(const Request &request, const ServerConfig &config)
{
	const std::string &path = request.getReqPath();
	const std::map<std::string, LocationConfig> &locations = config.getLocations();

	// First try exact match
	if (locations.find(path) != locations.end()) {
		return &locations.at(path);
	}

	// Then try prefix matching
	const LocationConfig *bestMatch = NULL;
	size_t bestLength = 0;

	for (std::map<std::string, LocationConfig>::const_iterator it = locations.begin();
		it != locations.end(); ++it)
	{
		const std::string &locPath = it->first;
		if (path.compare(0, locPath.length(), locPath) == 0)
		{
			if (locPath.length() > bestLength)
			{
				bestLength = locPath.length();
				bestMatch = &(it->second);
			}
		}
	}
	if (bestMatch && bestMatch->getCgis().size() > 0)
	{
		return bestMatch;
	}
	return NULL;
}

Response RequestHandler::handle(const Request &request, const ServerConfig &config)
{
	// First find matching location
	const LocationConfig *location = findMatchingLocation(request, config);

	// Check if CGI request
	if (location)
	{
		std::map<std::string, std::string> cgis = location->getCgis();
		std::string path = request.getReqPath();
		size_t dotPos = path.find_last_of('.');
		if (dotPos != std::string::npos)
		{
			std::string ext = path.substr(dotPos);
			if (cgis.find(ext) != cgis.end())
			{
				try
				{
					CgiHandler cgi(request, config, *location);
					return cgi.execute();
				}
				catch (const std::exception &e)
				{
					Response errorResponse;
					return HttpStatus::buildResponse(errorResponse, 500);
				}
			}
		}
	}

	// Handle standard methods
	if (request.getReqMethod() == "GET")
		return handleGetMethod(request, config);
	else if (request.getReqMethod() == "POST" && isValidPostRequest(request, config))
		return handlePostMethod(request, config);
	else if (request.getReqMethod() == "DELETE")
		return handleDeleteMethod(request);
	else
	{
		Response response;
		return HttpStatus::buildResponse(response, 405);
	}
}


// ============
// GET METHOD
// ============
Response RequestHandler::handleGetMethod(const Request &request, const ServerConfig &config)
{
	Response response;

	std::string path = request.getReqPath();
	std::string rootDir = config.getServerRoot();
	std::vector<std::string> indexes = config.getServerIndexes();

	if (path == "/")
	{
		std::string indexFile = resolveMultipleIndexes(rootDir, indexes);
		if (indexFile.empty())
			return HttpStatus::buildResponse(response, 403);
		path = "/" + indexFile;
	}

	if (path.find("..") != std::string::npos)
		return HttpStatus::buildResponse(response, 403);

	std::string fullPath = rootDir + path;
	std::ifstream file(fullPath.c_str());

	if (!file)
		return HttpStatus::buildResponse(response, 404);

	std::ostringstream ss;
	ss << file.rdbuf();

	std::string contentType = getMimeType(path);
	response.setStatus(200, "OK");
	response.setBody(ss.str());
	response.setHeader("Content-Type", contentType);
	return response;
}

// ============
// POST METHOD
// ============
Response RequestHandler::handlePostMethod(const Request &request, const ServerConfig &config)
{
	Response response;

	std::string contentType = request.getReqHeaderKey("Content-Type");
	
	if (contentType.find("multipart/form-data") != std::string::npos)
		return handleMultipartPost(request, config);
	if (contentType == "application/x-www-form-urlencoded")
		return handleFormPost(request, config);
	if (contentType == "application/octet-stream")
		return handleBinaryPost(request, config);
	return HttpStatus::buildResponse(response, 415);
}

Response RequestHandler::handleMultipartPost(const Request &request, const ServerConfig &config)
{
	Response response;
	
	std::string reqPath = request.getReqPath();
	std::string rootDir = config.getServerRoot();
	std::string locationPrefix = extractLocationPrefix(request, config);
	std::string locationRootDir = config.getLocations().at(locationPrefix).getRoot();
	
	if (locationRootDir[0] == '.')
		locationRootDir.erase(0, locationRootDir.find_first_not_of("."));

	std::vector<MultipartPart> parsedParts = parseMultiparts(request);
	
	
	return HttpStatus::buildResponse(response, 200);
}

std::vector<MultipartPart> parseMultiparts(const Request &request)
{
	std::vector<MultipartPart> parts;

	std::string body = request.getReqBody();
	std::string contentType = request.getReqHeaderKey("Content-Type");

	size_t boundaryPos = contentType.find('=');
	
	if (boundaryPos == std::string::npos)
		return parts;

	std::string boundary = "--" + contentType.substr(boundaryPos + 1);
	std::string boundaryEnd = boundary + "--";
	
	std::istringstream stream(body);
	std::string line;
	
	MultipartPart currPart;
	
	bool readingHeaders = false;
	bool readingBody = false;
	std::vector<char> bodyBuffer;
	
	while (std::getline(stream, line))
	{
		if (!line.empty() && line.back() == '\r')
			line.pop_back();

		if (line == boundary)
		{
			finalizePart(parts, currPart, bodyBuffer);
			readingHeaders = true;
			readingBody = false;
		}
		else if (line == boundaryEnd)
		{
			finalizePart(parts, currPart, bodyBuffer);
			break;
		}
		else if (readingHeaders)
		{
			if (line.empty())
			{
				readingHeaders = false;
				readingBody = true;
			}
			else if (line.find("Content-Disposition") != std::string::npos)
				parseContentDisposition(line, currPart);
		}
		else if (readingBody)
			bodyBuffer += line + "\n";
	}
	return parts;
}

Response RequestHandler::handleFormPost(const Request &request, const ServerConfig &config)
{
	Response response;
	(void)config;
	std::string body = request.getReqBody();

	std::map<std::string, std::string> clientData;
	size_t start = 0;

	while(start < body.length())
	{
		size_t end = body.find('&', start);
		if (end == std::string::npos) end = body.length();

		size_t equal = body.find('=', start);
		if (equal != std::string::npos && equal < end)
		{
			std::string key = body.substr(start, equal - start);
			std::string value = body.substr(equal + 1, end - equal - 1);
			clientData[key] = value;
		}
		start = end + 1;
	}

	std::map<std::string, std::string>::iterator it = clientData.begin();
	
	while (it != clientData.end())
	{
		std::cout << "key->" << it->first << std::endl;
		std::cout << "Value->" << it->second << std::endl;
		it++;
	}

	return HttpStatus::buildResponse(response, 200);
}

Response RequestHandler::handleBinaryPost(const Request &request, const ServerConfig &config)
{
	Response response;
	
	std::string reqPath = request.getReqPath();
	std::string body = request.getReqBody();
	std::string rootDir = config.getServerRoot();
	std::string locationPrefix = extractLocationPrefix(request, config);
	std::string locationRootDir = config.getLocations().at(locationPrefix).getRoot();
	
	if (locationRootDir[0] == '.')
		locationRootDir.erase(0, locationRootDir.find_first_not_of("."));
	
	std::string fileName = extractFilenameFromPath(reqPath);
	std::string updatedFileName = generateTimestampFilename(fileName);

	std::string fullPath = rootDir + locationRootDir + "/" + updatedFileName;
	std::ofstream out(fullPath.c_str(), std::ios::binary);
	
	if (!out)
		return HttpStatus::buildResponse(response, 500);

	out.write(body.c_str(), body.size());
	out.close();

	return HttpStatus::buildResponse(response, 200);
}

// ============
// DELETE METHOD
// ============
Response RequestHandler::handleDeleteMethod(const Request &request)
{
	// std::remove
	// In my case, the webserv accepts DELETE method and the CGI handles the delete thing
	(void)request;
	Response res;
	return res;
}
