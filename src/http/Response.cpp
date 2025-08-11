/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 17:38:56 by jmeirele          #+#    #+#             */
/*   Updated: 2025/08/11 15:08:55 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response() : _statusCode(200), _statusMessage("OK") {}

void Response::setStatus(int code, const std::string &message)
{
	_statusCode = code;
	_statusMessage = message;
}

void Response::setHeader(const std::string &key, const std::string &value)
{
	_headers[key] = value;
}

void Response::setBody(const std::string &body)
{
	_body = body;

	std::ostringstream oss;
	oss << _body.size();
	_headers["Content-Length"] = oss.str();
}

std::string Response::toString() const
{
	std::ostringstream responseStream;

	responseStream << "HTTP/1.1 " << _statusCode << " " << _statusMessage << "\r\n";

	// Add regular headers first
	for (std::map<std::string, std::string>::const_iterator it = _headers.begin();
		it != _headers.end(); ++it)
	{
		responseStream << it->first << ": " << it->second << "\r\n";
	}

	responseStream << "\r\n";
	responseStream << _body;

	return responseStream.str();
}

int Response::getStatusCode() const
{
	return _statusCode;
}
