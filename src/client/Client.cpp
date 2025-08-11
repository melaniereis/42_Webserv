/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 21:19:42 by jmeirele          #+#    #+#             */
/*   Updated: 2025/08/11 17:16:04 by jmeirele         ###   ########.fr       */
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
	char buffer[8192];
	ssize_t bytesRead = recv(_fd, buffer, sizeof(buffer), 0);

	// NO ERRNO CHECKING - evaluation requirement
	if (bytesRead <= 0) {
		_closed = true;
		return false;
	}

	_readBuffer.append(buffer, bytesRead);

	// Check if we have complete request
	if (_hasCompleteRequest()) {
		// Check body size limit
		size_t clPos = _readBuffer.find("Content-Length:");
		if (clPos != std::string::npos) {
			size_t lineEnd = _readBuffer.find("\r\n", clPos);
			std::string clStr = _readBuffer.substr(clPos + 15, lineEnd - (clPos + 15));
			size_t contentLength = std::atoi(clStr.c_str());

			if (contentLength > _config.getClientMaxBodySize()) {
				Response resp;
				HttpStatus::buildResponse(_config, resp, 413);
				_writeBuffer = resp.toString();
				return true;
			}
		}

		_request = new Request(_readBuffer);

		// Handle request (including CGI synchronously)
		_response = RequestHandler::handle(*_request, _config);
		_writeBuffer = _response.toString();

		_readBuffer.clear();
		delete _request;
		_request = NULL;
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

