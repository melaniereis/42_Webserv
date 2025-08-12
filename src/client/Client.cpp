/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 21:19:42 by jmeirele          #+#    #+#             */
/*   Updated: 2025/08/12 16:57:37 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(int fd, const struct sockaddr_in& addr, const ServerConfig &config)
	: _fd(fd), _closed(false), _readBuffer(""), _writeBuffer(""),
	_request(NULL), _config(config)
{
	char ipStr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(addr.sin_addr), ipStr, INET_ADDRSTRLEN);
	_clientAddress = ipStr;
	Logger::info("New connection from: " + _clientAddress);
}

Client::~Client()
{
	if (_request) {
		delete _request;
	}
}

int Client::getFd() const { return _fd; }
const std::string& Client::getClientAddress() const { return _clientAddress; }
bool Client::isClientClosed() const { return _closed; }

bool Client::_hasCompleteRequest() const
{
	size_t headerEnd = _readBuffer.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
		return false;

	size_t contentLength = 0;
	size_t clPos = _readBuffer.find("Content-Length:");
	if (clPos != std::string::npos)
	{
		size_t lineEnd = _readBuffer.find("\r\n", clPos);
		std::string clStr = _readBuffer.substr(clPos + 15, lineEnd - (clPos + 15));
		contentLength = std::atoi(clStr.c_str());
	}

	return _readBuffer.size() >= headerEnd + 4 + contentLength;
}

bool Client::handleClientRequest()
{
	// 1. Early body-size check before reading more data
	size_t clPos = _readBuffer.find("Content-Length:");
	if (clPos != std::string::npos)
	{
		size_t lineEnd = _readBuffer.find("\r\n", clPos);
		size_t contentLength = std::atoi(
			_readBuffer.substr(clPos + 15, lineEnd - clPos - 15).c_str()
		);
		if (contentLength > _config.getClientMaxBodySize())
		{
			Response resp;
			HttpStatus::buildResponse(_config, resp, 413);
			_writeBuffer = resp.toString();
			_readBuffer.clear();
			return true;  // schedule send of 413 immediately
		}
	}

	// 2. Now read incoming data
	char buffer[8192];
	ssize_t bytesRead = recv(_fd, buffer, sizeof(buffer), 0);
	if (bytesRead <= 0) { _closed = true; return false; }
	_readBuffer.append(buffer, bytesRead);

	// 3. Only when you’ve detected full headers and body, parse request
	if (_hasCompleteRequest())
	{
		_request = new Request(_readBuffer);
		_response = RequestHandler::handle(*_request, _config);
		_writeBuffer = _response.toString();
		delete _request;
		_request = NULL;
		_readBuffer.clear();
	}
	return true;
}


bool Client::handleClientResponse()
{
	if (_writeBuffer.empty())
		return true;

	ssize_t bytesWritten = send(_fd, _writeBuffer.c_str(), _writeBuffer.size(), 0);

	// NO ERRNO CHECKING - evaluation requirement
	if (bytesWritten <= 0) {
		_closed = true;
		return false;
	}

	_writeBuffer.erase(0, bytesWritten);
	return true;
}

void Client::closeClient()
{
	if (_fd > 0) {
		close(_fd);
		_fd = -1;
	}
	_closed = true;
}

