/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 18:31:25 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/17 17:25:05 by jmeirele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"
// #include "../config/ServerConfig.hpp"

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

// ============
// POST METHOD
// ============
Response RequestHandler::handlePostMethod(const Request &request, const ServerConfig &config)
{
	Response response;

	std::string reqPath = request.getReqPath();
	std::string contentType = request.getReqHeaderKey("Content-Type");
	std::string uploadDir = config.getLocations().at("/upload").getUploadDir();

	std::string fullPath = config.getServerRoot() + reqPath;
	std::ifstream file(fullPath.c_str());
	
	if (!file)
		return HttpStatus::buildResponse(response, 404);

	if (uploadDir[0] == '.')
		uploadDir.erase(0, uploadDir.find_first_not_of("."));

	if (uploadDir != reqPath)
		return HttpStatus::buildResponse(response, 403);

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
	
	std::string body = request.getReqBody();

	std::cout << "\n\n\n\n" << "Printing req body" << body << "\n\n\n\n";
	(void)request;
	(void)config;
	return response;
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
		std::cout << it->first << it->second << std::endl;
		it++;
	}

	return HttpStatus::buildResponse(response, 200);
}

Response RequestHandler::handleBinaryPost(const Request &request, const ServerConfig &config)
{
	Response response;
	
	std::string body = request.getReqBody();
	std::string rootDir = config.getServerRoot();
	std::string uploadDir = config.getLocations().at("/upload").getUploadDir();
	
	if (uploadDir[0] == '.')
		uploadDir.erase(0, uploadDir.find_first_not_of("."));
	
	// Check if client provided a filename via header X-Filename (optional)
	std::string fileName = request.getReqHeaderKey("X-Filename");
	std::string updatedFileName = generateTimestampFilename(fileName);

	std::string fullPath = rootDir + uploadDir + "/" + updatedFileName;
	std::ofstream out(fullPath.c_str(), std::ios::binary);
	
	if (!out)
		return HttpStatus::buildResponse(response, 500);

	out.write(body.c_str(), body.size());
	out.close();

	return HttpStatus::buildResponse(response, 200);
}

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
