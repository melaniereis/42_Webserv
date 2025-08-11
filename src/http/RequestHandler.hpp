/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 18:19:04 by jmeirele          #+#    #+#             */
/*   Updated: 2025/08/11 15:10:26 by meferraz         ###   ########.fr       */
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
	private:
};

struct MultipartPart
{
	std::string name;
	std::vector<char> content;
	std::string fileName;
	std::string contentType;
};

// Helper functions
std::string getMimeType(const std::string &extension);
bool endsWith(const std::string &str, const std::string &suffix);
std::string resolveMultipleIndexes(const std::string &rootDir, const std::vector<std::string> &indexes);
std::string generateTimestampFilename(std::string &fileName);
bool isMethodAllowed(const Request &request, const ServerConfig &config, const std::string &method);
std::string extractLocationPrefix(const Request &request, const ServerConfig &config);
std::string extractFilenameFromPath(const std::string &path);
std::vector<MultipartPart> parseMultiparts(const Request &request);
void finalizePart(std::vector<MultipartPart> &parts, MultipartPart &part, std::vector<char> &buffer);
void parseContentDisposition(const std::string &line, MultipartPart &part);
