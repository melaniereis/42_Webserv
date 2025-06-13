/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 18:31:25 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/13 16:37:32 by jmeirele         ###   ########.fr       */
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
	std::string path = request.getReqPath();
	std::string contentType = request.getReqHeaderKey("Content-Type");

	if (path != "/uploads")
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
	(void)request;
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
