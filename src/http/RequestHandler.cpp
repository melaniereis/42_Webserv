/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 18:31:25 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/16 14:29:25 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"
// #include "../config/ServerConfig.hpp"

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
	return bestMatch;
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
	else if (request.getReqMethod() == "POST")
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

// Response RequestHandler::handlePostMethod(const Request &request, const ServerConfig &config)
// {
// 	Response response;
// 	std::string path = request.getReqPath();
// 	std::string root = config.getServerRoot();
// 	std::string uploadDir = "uploads";

// 	std::string fileName = request.getReqHeaderKey("X-Filename");

// 	if (fileName.empty())
// 		fileName = "default_upload_name";

// 	if (path != "/uploads")
// 		return HttpStatus::buildResponse(response, 403);

// 	std::string fullPath =  root + "/" + uploadDir + "/" + fileName;
// 	std::ofstream outFile(fullPath.c_str(), std::ios::binary);

// 	if (!outFile)
// 		return HttpStatus::buildResponse(response, 500);

// 	outFile.write(request.getReqBody().c_str(), request.getReqBody().size());
// 	outFile.close();

// 	return HttpStatus::buildResponse(response, 200);
// }

// ============
// POST METHOD
// ============
Response RequestHandler::handlePostMethod(const Request &request, const ServerConfig &config)
{
	Response response;
	std::string reqPath = request.getReqPath();
	std::string contentType = request.getReqHeaderKey("Content-Type");
	std::string uploadDir = config.getLocations().at("/upload").getUploadDir();

	if (uploadDir[0] == '.')
		uploadDir.erase(0, uploadDir.find_first_not_of("."));

	if (uploadDir != reqPath)
		return HttpStatus::buildResponse(response, 415);

	if (contentType.find("multipart/form-data") != std::string::npos)
		return handleMultipartPost(request, config);
	if (contentType == "application/x-www-form-urlencoded")
		return handleFormPost(request, config);
	if (contentType == "application/octet-stream")
		return handleBinaryPost(request, config);
	// if (contentType == "application/json")
	// 	return handleJsonPost(request, config);
	return HttpStatus::buildResponse(response, 415);
}

Response RequestHandler::handleMultipartPost(const Request &request, const ServerConfig &config)
{
	Response response;
	(void)request;
	(void)config;
	return response;
}

Response RequestHandler::handleFormPost(const Request &request, const ServerConfig &config)
{
	Response response;
	(void)request;
	(void)config;
	return response;
}

Response RequestHandler::handleBinaryPost(const Request &request, const ServerConfig &config)
{
	Response response;

	std::string body = request.getReqBody();
	std::string rootDir = config.getServerRoot();
	std::string uploadDir = config.getLocations().at("/upload").getUploadDir();
	std::string fileName = "default_file_name";

	if (uploadDir[0] == '.')
		uploadDir.erase(0, uploadDir.find_first_not_of("."));

	std::string fullPath = rootDir + uploadDir + "/" + fileName;
	std::ofstream out(fullPath.c_str(), std::ios::binary);

	if (!out)
		return HttpStatus::buildResponse(response, 500);

	out.write(body.c_str(), body.size());
	out.close();

	return HttpStatus::buildResponse(response, 200);
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
