/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 18:19:04 by jmeirele          #+#    #+#             */
/*   Updated: 2025/07/09 21:56:28 by meferraz         ###   ########.fr       */
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
		static Response handleDeleteMethod(const Request &request);

		// Post Content-Types
		static Response handleMultipartPost(const Request &request, const ServerConfig &config);
		static Response handleFormPost(const Request &request, const ServerConfig &config);
		static Response handleBinaryPost(const Request &request, const ServerConfig &config);
		static Response handleSessionRoute(const Request &request);
		static Response handleCookieDisplayRoute(const Request &request);
		static void setQueryStringAsCookies(const std::string &queryString, Response &response);
		static std::string urlDecode(const std::string &str);
	private:
};

struct MultipartPart
{
	std::string name;
	std::vector<char> content;
	std::string fileName;
	std::string contentType;
};

// Multipurpose Internet Mail Extensions =>  MIME
std::string getMimeType(const std::string &extension);
bool endsWith(const std::string &str, const std::string &suffix);

// Handling multiple indexes of server indexes
std::string resolveMultipleIndexes(const std::string &rootDir, const std::vector<std::string> &indexes);

// Generate unique filename for binary post
std::string generateTimestampFilename(std::string &fileName);

// Validating the location and the allowed methods on the specific location
bool isMethodAllowed(const Request &request, const ServerConfig &config, const std::string &method);

std::string extractLocationPrefix(const Request &request, const ServerConfig &config);

std::string extractFilenameFromPath(const std::string &path);

std::vector<MultipartPart> parseMultiparts(const Request &request);

void finalizePart(std::vector<MultipartPart> &parts, MultipartPart &part, std::vector<char> &buffer);
void parseContentDisposition(const std::string &line, MultipartPart &part);


