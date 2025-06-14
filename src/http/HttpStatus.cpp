/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpStatus.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 18:06:02 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/12 20:33:54 by jmeirele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpStatus.hpp"

std::string HttpStatus::getMessage(int code)
{
	switch (code)
	{
		case 200: return "OK";
		case 201: return "Created";
		case 204: return "No Content";
		case 301: return "Moved Permanently";
		case 302: return "Found";
		case 400: return "Bad Request";
		case 403: return "Forbidden";
		case 404: return "Not Found";
		case 405: return "Method Not Allowed";
		case 413: return "Payload Too Large";
		case 415: return "Unsupported Media Type";
		case 500: return "Internal Server Error";
		case 501: return "Not Implemented";
		default:  return "Unknown Status";
	}
}

std::string HttpStatus::generateHtmlBody(int code)
{
	std::string message = getMessage(code);

	std::ostringstream html;
	html << "<!DOCTYPE html>\n<html>\n<head><title>"
			<< code << " " << message
			<< "</title></head>\n<body><h1>"
			<< code << " " << message
			<< "</h1></body>\n</html>";
	return html.str();
}

Response HttpStatus::buildResponse(Response &response, int code)
{
	response.setStatus(code, getMessage(code));
	response.setHeader("Content-Type", "text/html");
	response.setBody(generateHtmlBody(code));
	return response;
}