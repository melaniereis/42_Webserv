/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/04 21:19:42 by jmeirele          #+#    #+#             */
/*   Updated: 2025/07/12 10:48:05 by jmeirele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(int fd, const struct sockaddr_in& addr, const ServerConfig &config)
	: _fd(fd), _closed(false), _readBuffer(""), _writeBuffer(""),
	_cgi_processing(false), _request(NULL), _config(config)
{
	char ipStr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(addr.sin_addr), ipStr, INET_ADDRSTRLEN);
	_clientAddress = ipStr;
	Logger::info("New connection from: " + _clientAddress);
}

Client::~Client() {}

int Client::getFd() const
{
	return _fd;
}

const std::string& Client::getClientAddress() const {
	return _clientAddress;
}

bool Client::handleClientRequest()
{
	if (_cgi_processing) {
		// Still processing CGI, wait for completion
		return true;
	}

	char buffer[1024];
	ssize_t bytesRead = recv(_fd, buffer, sizeof(buffer) - 1, 0);

	if (bytesRead > 0)
	{
		_readBuffer.append(buffer, bytesRead);
	}
	else
	{
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
	_response = RequestHandler::handle(*_request, _config, *this);
	_writeBuffer = _response.toString();
	return true;
}

bool Client::handleClientResponse()
{
	if (_cgi_processing) {
		// Still processing CGI, wait for completion
		return true;
	}

	if (_writeBuffer.empty())
		return true;

	ssize_t bytesWritten = send(_fd, _writeBuffer.c_str(), _writeBuffer.size(), 0);
	if (bytesWritten > 0)
	{
		_writeBuffer.erase(0, bytesWritten);
		if (_writeBuffer.empty())
		{
			_readBuffer.clear();
			delete _request;
			_request = NULL;
		}
		return true;
	}

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

void Client::setCgiOutput(const std::string& output)
{
	_cgi_output = output;
	_cgi_processing = false;

	// Create proper response from CGI output
	Response response;
	_response = response;
	_response.setBody(_cgi_output);
	_response.setStatus(200, "OK");

	_writeBuffer = _response.toString();
}

bool Client::isCgiProcessing() const {
	return _cgi_processing;
}

void Client::setCgiProcessing(bool state) {
	_cgi_processing = state;
}
