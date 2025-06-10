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

	if (bytesRead < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return true;
		}
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

	if (_readBuffer.size() > _config.getClientMaxBodySize())
	{
		Response res;
		res.setStatus(413, "Payload Too Large");
		res.setHeader("Content-Type", "text/html");
		res.setBody("<html><body><h1>413 Payload Too Large</h1></body></html>");
		_writeBuffer = res.toString();
		return true;
	}

	if (_readBuffer.find("\r\n\r\n") != std::string::npos)
	{
		if (_readBuffer.find("HTTP/") == std::string::npos)
		{
			Response res;
			res.setStatus(400, "Bad Request");
			res.setHeader("Content-Type", "text/html");
			res.setBody("<html><body><h1>400 Bad Request</h1></body></html>");
			_writeBuffer = res.toString();
			return true;
		}

		Logger::info("Received request: " + _readBuffer);

		std::string root = _config.getServerRoot();
		std::vector<std::string> indexes = _config.getServerIndexes();
		std::string index_file = indexes.empty() ? "index.html" : indexes[0];

		std::ifstream file((root + "/" + index_file).c_str());
		std::ostringstream ss;
		ss << file.rdbuf();

		Response res;
		res.setStatus(200, "OK");
		res.setHeader("Content-Type", "text/html");
		res.setBody(ss.str());

		_writeBuffer = res.toString();
		_request = new Request(_readBuffer);
		_response = RequestHandler::handle(*_request);
		_writeBuffer = _response.toString();

	}
	std::cout << "Client Request:\n" << _readBuffer << std::endl;
	return true;
}

bool Client::handleClientResponse()
{
	if (_writeBuffer.empty()) return true;

	Logger::info("Sending response: " + _writeBuffer);

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
