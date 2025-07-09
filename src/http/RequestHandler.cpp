/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 18:31:25 by jmeirele          #+#    #+#             */
/*   Updated: 2025/07/09 21:49:25 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"
#include "../session/SessionManager.hpp"

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
	// Add cookie display route
	if (request.getReqPath() == "/cookies") {
		return handleCookieDisplayRoute(request);
	}

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

	if (request.getReqPath() == "/session") {
		return handleSessionRoute(request);
	}

	// Handle standard methods
	if (request.getReqMethod() == "GET" && isMethodAllowed(request, config, "GET"))
		return handleGetMethod(request, config);
	else if (request.getReqMethod() == "POST" && isMethodAllowed(request, config, "POST"))
		return handlePostMethod(request, config);
	else if (request.getReqMethod() == "DELETE" && isMethodAllowed(request, config, "DELETE"))
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
	std::string locationPrefix = extractLocationPrefix(request, config);
	std::string locationRootDir = config.getLocations().at(locationPrefix).getRoot();

	if (locationRootDir[0] == '.')
		locationRootDir.erase(0, locationRootDir.find_first_not_of("."));

	if (path == "/")
	{
		std::string indexFile = resolveMultipleIndexes(rootDir, indexes);
		if (indexFile.empty())
			return HttpStatus::buildResponse(response, 403);
		path = "/" + indexFile;
	}

	if (path.find("..") != std::string::npos)
		return HttpStatus::buildResponse(response, 403);

	if (locationPrefix != "/")
		path = path.substr(locationPrefix.length());

	std::string fullPath = rootDir + locationRootDir + path;
	std::cout << fullPath << std::endl;
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
		if (body.substr(pos, 2) == "--") break; // End boundary
		if (body.substr(pos, 2) == "\r\n") pos += 2;

		// Find headers
		size_t headerEnd = body.find("\r\n\r\n", pos);
		if (headerEnd == std::string::npos) break;

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

// src/http/RequestHandler.cpp
// src/http/RequestHandler.cpp
Response RequestHandler::handleSessionRoute(const Request& request) {
	Response response;
	SessionManager& sessionMgr = SessionManager::getInstance();

	std::string sessionId;
	bool createNewSession = true;

	// Check if we have a session cookie
	const std::map<std::string, std::string>& cookies = request.getCookies();
	for (std::map<std::string, std::string>::const_iterator it = cookies.begin();
		it != cookies.end(); ++it)
	{
		Logger::debug("Cookie found: " + it->first + "=" + it->second);
	}
	std::map<std::string, std::string>::const_iterator it = cookies.find("sessionid");

	if (it != cookies.end()) {
		sessionId = it->second;
		if (sessionMgr.hasSession(sessionId)) {
			// Valid session found
			createNewSession = false;
			sessionMgr.updateSessionAccessTime(sessionId);
			Logger::debug("Using existing session: " + sessionId);
		} else {
			Logger::debug("Invalid session ID provided: " + sessionId);
			// Don't create session yet - we'll create after this block
		}
	}

	if (createNewSession) {
		sessionId = sessionMgr.createSession();
		response.setCookie("sessionid", sessionId);
		Logger::debug("Created new session: " + sessionId);
	}

	// Update session data
	std::string& sessionData = sessionMgr.getSessionData(sessionId);
	int count = 0;
	if (!sessionData.empty()) {
		count = atoi(sessionData.c_str());
	}
	count++;

	// Store updated count
	std::ostringstream oss;
	oss << count;
	sessionData = oss.str();

	// Build response
	response.setStatus(200, "OK");
	response.setBody("Session ID: " + sessionId + "\nCount: " + sessionData);
	return response;
}

const std::map<std::string, std::string>& Request::getCookies() const
{
	return _cookies;
}

Response RequestHandler::handleCookieDisplayRoute(const Request& request) {
	Response response;

	// Handle query string parameters for cookie setting (like your friend's PHP)
	std::string queryString = request.getReqQueryString();
	if (!queryString.empty()) {
		setQueryStringAsCookies(queryString, response);
	}

	std::string html = "<!DOCTYPE html>\n<html>\n<body>\n";
	html += "<h1>Current Cookies</h1>\n<ul>\n";

	const std::map<std::string, std::string>& cookies = request.getCookies();
	for (std::map<std::string, std::string>::const_iterator it = cookies.begin();
		it != cookies.end(); ++it) {
		html += "<li><strong>" + it->first + "</strong>=" + it->second + "</li>\n";
	}

	html += "</ul>\n</body>\n</html>\n";

	response.setStatus(200, "OK");
	response.setHeader("Content-Type", "text/html");
	response.setBody(html);
	return response;
}

// Add method to convert query parameters to cookies:
void RequestHandler::setQueryStringAsCookies(const std::string& queryString, Response& response) {
	size_t start = 0;
	while (start < queryString.length()) {
		size_t end = queryString.find('&', start);
		if (end == std::string::npos) end = queryString.length();

		std::string pair = queryString.substr(start, end - start);
		size_t equalPos = pair.find('=');

		if (equalPos != std::string::npos) {
			std::string key = pair.substr(0, equalPos);
			std::string value = pair.substr(equalPos + 1);

			// URL decode if needed
			key = urlDecode(key);
			value = urlDecode(value);

			response.setCookie(key, value);
			Logger::debug("Set cookie from query: " + key + "=" + value);
		}

		start = end + 1;
	}
}

// Add URL decode helper:
std::string RequestHandler::urlDecode(const std::string& str) {
	std::string result;
	for (size_t i = 0; i < str.length(); ++i) {
		if (str[i] == '%' && i + 2 < str.length()) {
			int value;
			std::istringstream ss(str.substr(i + 1, 2));
			ss >> std::hex >> value;
			result += static_cast<char>(value);
			i += 2;
		} else if (str[i] == '+') {
			result += ' ';
		} else {
			result += str[i];
		}
	}
	return result;
}
