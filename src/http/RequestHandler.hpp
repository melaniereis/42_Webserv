/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 18:19:04 by jmeirele          #+#    #+#             */
/*   Updated: 2025/08/04 12:22:15 by jmeirele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Request.hpp"
#include "Response.hpp"
#include "../config/ServerConfig.hpp"
#include "HttpStatus.hpp"
#include "../cgi/CgiHandler.hpp"

class RequestHandler
{
public:
	static Response handle(const Request &request, const ServerConfig &config);
	static Response handleGetMethod(const Request &request, const ServerConfig &config);
	static Response handlePostMethod(const Request &request, const ServerConfig &config);
	static Response handleDeleteMethod(const Request &request, const ServerConfig &config);

	// POST Content-Types
	static Response handleMultipartPost(const Request &request, const ServerConfig &config);
	static Response handleFormPost(const Request &request, const ServerConfig &config);
	static Response handleBinaryPost(const Request &request, const ServerConfig &config);
private:
};

struct MultipartPart
{
	std::string name;
	std::vector<char> content;
	std::string fileName;
	std::string contentType;
};

std::vector<MultipartPart> parseMultiparts(const Request &request);

// RequestHandlerUtils.cpp // 

// Generate unique filename for binary post
std::string generateTimestampFilename(std::string &fileName);

// Handling multiple indexes of server indexes
std::string resolveMultipleIndexes(const std::string &rootDir, const std::vector<std::string> &indexes);

// Multipurpose Internet Mail Extensions =>  MIME
std::string getMimeType(const std::string &extension);

bool endsWith(const std::string &str, const std::string &suffix);

std::string extractLocationPrefix(const Request &request, const ServerConfig &config);

std::string extractFilenameFromPath(const std::string &path);

// Validating the location and the allowed methods on the specific location
bool isMethodAllowed(const Request &request, const ServerConfig &config, const std::string &method);

void finalizePart(std::vector<MultipartPart> &parts, MultipartPart &part, std::vector<char> &buffer);

void parseContentDisposition(const std::string &line, MultipartPart &part);

bool isDirectory(const std::string &path);

std::string normalizeReqPath(const std::string &path);

Response generateAutoIndexPage(Response &response, const std::string &dirPath, const std::string &reqPath);

Response &handleRedirectLocation(Response &response, std::map<int, std::string> &locationRedirects);
