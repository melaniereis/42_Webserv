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

	// Capture errno immediately after recv to avoid overwriting
	int recvErrno = (bytesRead < 0) ? errno : 0;

	if (bytesRead < 0)
	{
		if (recvErrno == EAGAIN || recvErrno == EWOULDBLOCK)
			return true;
		// Restore errno for accurate error reporting
		errno = recvErrno;
		perror("recv failed");
		_closed = true;
		return false;
	}
	else if (bytesRead == 0)
	{
		_closed = true;
		return false;
	}

	buffer[bytesRead] = '\0';
	_readBuffer += buffer;

	if (_readBuffer.find("\r\n\r\n") != std::string::npos)
	{
		_request = new Request(_readBuffer);
		_response = RequestHandler::handle(*_request, _config);
		_writeBuffer = _response.toString();
	}
	Logger::info("Client Request:\n" + _readBuffer);
	return true;
}

bool Client::handleClientResponse()
{
	if (_writeBuffer.empty()) return true;

	Logger::info("Sending response:\n" + _writeBuffer);

	ssize_t bytesSent = send(_fd, _writeBuffer.c_str(), _writeBuffer.size(), 0);
	if (bytesSent <= 0)
	{
		perror("send failed");
		_closed = true;
		return false;
	}

	_writeBuffer.erase(0, bytesSent);

	if (_writeBuffer.empty())
	{
		_readBuffer.clear();
		delete _request;
		_request = NULL;
	}

	return true;
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
