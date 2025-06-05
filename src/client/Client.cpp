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

bool Client::handleClientRead() {
	char buffer[1024];
	ssize_t bytesRead = recv(_fd, buffer, sizeof(buffer) - 1, 0);

	if (bytesRead < 0)
	{
		perror("recv failed");
		return false;
	}
	else if (bytesRead == 0)
	{
		_closed = true;
		return false;
	}

	buffer[bytesRead] = '\0';
	_readBuffer += buffer;

	std::cout << "Client Request:\n" << _readBuffer << std::endl;
	return true;
}

bool Client::handleClientWrite()
{
	if (_writeBuffer.empty())
	{
		// Send index.html as first response
		std::ifstream file(_index.c_str());
		if (!file)
		{
			_writeBuffer = "HTTP/1.1 404 Not Found\r\n\r\nFile not found";
		}
		else
		{
			std::ostringstream ss;
			ss << file.rdbuf();
			_writeBuffer = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + ss.str();
		}
	}

	ssize_t bytesSent = send(_fd, _writeBuffer.c_str(), _writeBuffer.size(), 0);
	if (bytesSent <= 0)
		return false;

	_writeBuffer.erase(0, bytesSent);
	return true;
}
