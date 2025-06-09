/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 18:31:25 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/09 20:47:12 by jmeirele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

Response RequestHandler::handle(const Request &request)
{
	if (request.getReqMethod() == "GET")
		return RequestHandler::handleGetMethod(request);
	else if (request.getReqMethod() == "POST")
		return RequestHandler::handlePostMethod(request);
	else if (request.getReqMethod() == "DELETE")
		return RequestHandler::handleDeleteMethod(request);
	else
	{
		Response response;
		response.setStatus(405, "Method not allowed");
		return response;
	}
}

Response RequestHandler::handleGetMethod(const Request &request)
{
	Response response;
	std::string path = request.getReqPath();

	if (path == "/")
		path = "index.html";
	
	std::string fullPath = "pages/" + path;
	std::ifstream file(fullPath.c_str());
	
	// Prevent directory traversal
	if (path.find("..") != std::string::npos)
	{
		response.setStatus(403, "Forbidden");
		response.setBody("<h1>403 Forbidden</h1>");
		response.setHeader("Content-Type", "text/html");
		return response;
	}

	if (!file)
	{
		response.setStatus(404, "Not Found");
		response.setBody("<h1>404 Not Found</h1>");
		response.setHeader("Content-Type", "text/html");
		return response;
	}
	
	std::ostringstream ss;
	ss << file.rdbuf();

	response.setStatus(200, "OK");
	response.setBody(ss.str());
	response.setHeader("Content-Type", "text/html");
	return response;
	
}

Response RequestHandler::handlePostMethod(const Request &request)
{
	(void)request;
	Response res;
	return res;
}

Response RequestHandler::handleDeleteMethod(const Request &request)
{
	(void)request;
	Response res;
	return res;
}
