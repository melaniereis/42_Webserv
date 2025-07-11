/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#z+#+   +#+           */
/*   Created: 2025/06/04 21:19:42 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/04 22:19:26 by jmeirele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(int fd, const ServerConfig &config)
	: _fd(fd), _closed(false), _readBuffer(""), _writeBuffer(""), _request(NULL), _config(config)
{
	Logger::info("New connection accepted");
}

Client::~Client() {}

bool Client::handleClientRequest()
{
	char buffer[1024];
	ssize_t bytesRead = recv(_fd, buffer, sizeof(buffer) - 1, 0);

	// Se bytesRead > 0, acumula dados; caso contrário, encerra conexão
	if (bytesRead > 0)
	{
		_readBuffer.append(buffer, bytesRead);
	}
	else
	{
		// bytesRead == 0 → conexão fechada pelo cliente
		// bytesRead < 0 → erro de I/O (sem inspecionar errno)
		_closed = true;
		return false;
	}

	size_t headerEnd = _readBuffer.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
		return true;

	size_t contentLength = 0;
	size_t clPos = _readBuffer.find("Content-Length:");
	if (clPos != std::string::npos)
	{
		size_t lineEnd = _readBuffer.find("\r\n", clPos);
		std::string clStr = _readBuffer.substr(clPos + 15, lineEnd - (clPos + 15));
		contentLength = std::atoi(clStr.c_str());

		if (contentLength > _config.getClientMaxBodySize())
		{
			Response resp;
			HttpStatus::buildResponse(resp, 413);
			_writeBuffer = resp.toString();
			return true;
		}
	}

	if (_readBuffer.size() < headerEnd + 4 + contentLength)
		return true;

	_request = new Request(_readBuffer);
	_response = RequestHandler::handle(*_request, _config);
	_writeBuffer = _response.toString();
	return true;
}

bool Client::handleClientResponse()
{
	if (_writeBuffer.empty())
		return true;

	ssize_t bytesRead = send(_fd, _writeBuffer.c_str(), _writeBuffer.size(), 0);
	if (bytesRead > 0)
	{
		_writeBuffer.erase(0, bytesRead);
		if (_writeBuffer.empty())
		{
			_readBuffer.clear();
			delete _request;
			_request = NULL;
		}
		return true;
	}

	// bytesRead == 0 ou <0 → encerra conexão sem checar errno
	_closed = true;
	return false;
}

void Client::closeClient()
{
	if (_fd > 0)
	{
		close(_fd);
		_fd = -1;
	}
	_closed = true;
}
