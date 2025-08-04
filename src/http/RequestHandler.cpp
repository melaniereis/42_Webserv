/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 18:31:25 by jmeirele          #+#    #+#             */
/*   Updated: 2025/08/04 12:54:02 by jmeirele         ###   ########.fr       */
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
	if (request.getReqMethod() == "GET" && isMethodAllowed(request, config, "GET"))
		return handleGetMethod(request, config);
	else if (request.getReqMethod() == "POST" && isMethodAllowed(request, config, "POST"))
		return handlePostMethod(request, config);
	else if (request.getReqMethod() == "DELETE" && isMethodAllowed(request, config, "DELETE"))
		return handleDeleteMethod(request, config);
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
	// config.getServerNotFound();
	Response response;

	std::string reqPath = request.getReqPath();
	std::string rootDir = config.getServerRoot();
	std::vector<std::string> indexes = config.getServerIndexes();
	std::string locationPrefix = extractLocationPrefix(request, config);
	std::string locationRootDir = config.getLocations().at(locationPrefix).getRoot();
	std::vector<std::string> locationIndex = config.getLocations().at(locationPrefix).getIndexes();
	std::map<int, std::string> locationRedirects = config.getLocations().at(locationPrefix).getRedirects();

	reqPath = normalizeReqPath(reqPath);

	if (!locationRedirects.empty())
		return handleRedirectLocation(response, locationRedirects);
	
	if (locationRootDir[0] == '.')
		locationRootDir.erase(0, locationRootDir.find_first_not_of("."));

	if (reqPath == "/")
	{
		std::string indexFile = resolveMultipleIndexes(rootDir, indexes);
		if (indexFile.empty())
			return HttpStatus::buildResponse(response, 403);
		reqPath = "/" + indexFile;
	}

	if (locationPrefix != "/")
		reqPath = reqPath.substr(locationPrefix.length());

	std::string fullPath;

	if (!locationIndex.empty() && reqPath.empty())
		fullPath = rootDir + locationRootDir + reqPath + "/" + locationIndex.at(0);
	else
		fullPath = rootDir + locationRootDir + reqPath;
	
	std::ifstream file(fullPath.c_str());

	if (!file)
		return HttpStatus::buildResponse(response, 404);

	if (isDirectory(fullPath))
		return generateAutoIndexPage(response, fullPath, reqPath);

	std::ostringstream ss;
	ss << file.rdbuf();

	std::string contentType = getMimeType(reqPath);
	response.setStatus(200, "OK");
	response.setBody(ss.str());
	response.setHeader("Content-Type", contentType);
	return response;
}

// ============
// POST METHOD
// ============
	// if (contentType == "application/octet-stream")
	// return HttpStatus::buildResponse(response, 415);

Response RequestHandler::handlePostMethod(const Request &request, const ServerConfig &config)
{
	Response response;

	std::string contentType = request.getReqHeaderKey("Content-Type");

	std::cout << "content type-> " << contentType << std::endl;

	if (contentType.find("multipart/form-data") != std::string::npos)
		return handleMultipartPost(request, config);
	if (contentType == "application/x-www-form-urlencoded")
		return handleFormPost(request, config);
	if (request.getReqBody().empty())
		return HttpStatus::buildResponse(response, 400);
	return handleBinaryPost(request, config);
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

	if (reqPath.find("..") != std::string::npos)
		return HttpStatus::buildResponse(response, 403);
	
	std::vector<MultipartPart> parsedParts = parseMultiparts(request);
	
	// for (std::vector<MultipartPart>::iterator it = parsedParts.begin(); it != parsedParts.end(); it++)
	// {
	// 	std::string contentStr(it->content.begin(), it->content.end());
	// 	std::cout << "Name ->" << it->name << std::endl;
	// 	std::cout << "Content-Type ->" << it->contentType << std::endl;
	// 	std::cout << "Content ->" << contentStr << std::endl;
	// 	std::cout << "Filename ->" << it->fileName << std::endl;
	// }

	for (std::vector<MultipartPart>::iterator it = parsedParts.begin(); it != parsedParts.end(); ++it)
	{
		if (!it->fileName.empty())
		{
			std::string updatedFileName = generateTimestampFilename(it->fileName);
			std::string fullPath = rootDir + locationRootDir + "/" + updatedFileName;
			
			std::ofstream out(fullPath.c_str(), std::ios::binary);
			if (!out)
				return HttpStatus::buildResponse(response, 500);

			out.write(it->content.data(), it->content.size());
			out.close();
		}
		else
		{
			std::cout << "Key->" << it->name << std::endl;
			std::cout << "Value->" << std::string(it->content.begin(), it->content.end()) << "\n\n";
		}
	}
	return HttpStatus::buildResponse(response, 200);
}

std::vector<MultipartPart> parseMultiparts(const Request &request)
{
	std::vector<MultipartPart> parts;

	const std::string &body = request.getReqBody();
	std::string contentType = request.getReqHeaderKey("Content-Type");

	size_t boundaryPos = contentType.find('=');
	if (boundaryPos == std::string::npos)
		return parts;

	std::string boundary = "--" + contentType.substr(boundaryPos + 1);
	std::string boundaryEnd = boundary + "--";

	size_t pos = 0;
	while (true)
	{
		// Find the next boundary
		size_t boundaryStart = body.find(boundary, pos);
		if (boundaryStart == std::string::npos)
			break;

		// Skip the boundary and next \r\n
		pos = boundaryStart + boundary.length();
		if (body.substr(pos, 2) == "--")
			break; // End boundary

		if (body.substr(pos, 2) == "\r\n")
			pos += 2;

		// Find headers
		size_t headerEnd = body.find("\r\n\r\n", pos);
		if (headerEnd == std::string::npos)
			break;

		std::string headers = body.substr(pos, headerEnd - pos);
		pos = headerEnd + 4; // move to body

		MultipartPart part;
		std::istringstream headerStream(headers);
		std::string headerLine;
		while (std::getline(headerStream, headerLine))
		{
			if (!headerLine.empty() && headerLine[headerLine.size() - 1] == '\r')
					headerLine.erase(headerLine.size() - 1);

			if (headerLine.find("Content-Disposition:") != std::string::npos)
				parseContentDisposition(headerLine, part);
			else if (headerLine.find("Content-Type:") != std::string::npos)
				part.contentType = headerLine.substr(14); // assumes exactly "Content-Type: "
		}

		// Find next boundary to determine content range
		size_t nextBoundary = body.find(boundary, pos);
		if (nextBoundary == std::string::npos)
			break;

		size_t contentLen = nextBoundary - pos;
		part.content = std::vector<char>(body.begin() + pos, body.begin() + pos + contentLen);

		// Trim trailing \r\n from content if present
		if (contentLen >= 2 && part.content[contentLen - 2] == '\r' && part.content[contentLen - 1] == '\n')
			part.content.resize(contentLen - 2);

		parts.push_back(part);
		pos = nextBoundary;
	}
	return parts;
}

Response RequestHandler::handleFormPost(const Request &request, const ServerConfig &config)
{
	(void)config;
	Response response;

	std::string reqPath = request.getReqPath();
	std::string body = request.getReqBody();

	if (reqPath.find("..") != std::string::npos)
		return HttpStatus::buildResponse(response, 403);

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
	
	if (reqPath.find("..") != std::string::npos)
		return HttpStatus::buildResponse(response, 403);
	
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
Response RequestHandler::handleDeleteMethod(const Request &request, const ServerConfig &config)
{
	Response response;

	std::string reqPath = request.getReqPath();
	std::string rootDir = config.getServerRoot();
	std::string locationPrefix = extractLocationPrefix(request, config);
	std::string locationRootDir = config.getLocations().at(locationPrefix).getRoot();

	if (locationRootDir[0] == '.')
		locationRootDir.erase(0, locationRootDir.find_first_not_of("."));

	// Strip location prefix if needed
	if (locationPrefix != "/")
		reqPath = reqPath.substr(locationPrefix.length());

	if (reqPath.find("..") != std::string::npos)
		return HttpStatus::buildResponse(response, 403);

	std::string fullPath = rootDir + locationRootDir + reqPath;

	std::ifstream file(fullPath.c_str());
	if (!file)
		return HttpStatus::buildResponse(response, 404);

	file.close();

	// Verify if it is a directory
	if (isDirectory(fullPath))
		return HttpStatus::buildResponse(response, 403);

	// Actually try to remove the file
	if (std::remove(fullPath.c_str()) != 0)
		return HttpStatus::buildResponse(response, 500);

	return HttpStatus::buildResponse(response, 200);
}
