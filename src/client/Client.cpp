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

Client::Client(int fd)
{
	_fd = fd;
	_closed = false;
	_index = _config.getServerIndex();
}

Client::~Client(){}

bool Client::handleClientRequest()
{
	char buffer[1024];
	ssize_t bytesRead = recv(_fd, buffer, sizeof(buffer) - 1, 0);

	if (bytesRead < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return true;
		perror("recv failed");
		_closed = true;
		return false;
	}
	else if (bytesRead == 0) {
		_closed = true;
		return false;
	}

	buffer[bytesRead] = '\0';
	_readBuffer += buffer;

	if (_readBuffer.find("\r\n\r\n") != std::string::npos)
	{
		_request = new Request(_readBuffer);
		
	}
	std::cout << "Client Request:\n" << _readBuffer << std::endl;
	return true;
}


bool Client::handleClientResponse()
{
	if (_writeBuffer.empty()) return true;

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


