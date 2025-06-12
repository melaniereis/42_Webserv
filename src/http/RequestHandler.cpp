/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 18:31:25 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/12 18:26:46 by jmeirele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

Response RequestHandler::handle(const Request &request, const ServerConfig &config)
{
	if (request.getReqMethod() == "GET")
		return handleGetMethod(request, config);
	else if (request.getReqMethod() == "POST")
		return handlePostMethod(request, config);
	else if (request.getReqMethod() == "DELETE")
		return handleDeleteMethod(request);
	else
	{
		Response response;
		return HttpStatus::setErrorResponse(response, 405);
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
			return HttpStatus::setErrorResponse(response, 403);
		path = "/" + indexFile;
	}
	
	if (path.find("..") != std::string::npos)
		return HttpStatus::setErrorResponse(response, 403);
	
	std::string fullPath = rootDir + path;
	std::ifstream file(fullPath.c_str());
	
	if (!file)
		return HttpStatus::setErrorResponse(response, 404);
	
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
	std::string path = request.getReqPath();
	std::string uploadDir = "uploads";
	
	(void)config;
	return response;
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
	if (endsWith(extension, ".html")) return "text/html";
	if (endsWith(extension, ".jpg") || endsWith(extension, ".jpeg")) return "image/jpeg";
	if (endsWith(extension, ".css")) return "text/css";
	if (endsWith(extension, ".js")) return "application/javascript";
	if (endsWith(extension, ".png")) return "image/png";
	if (endsWith(extension, ".gif")) return "image/gif";
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
