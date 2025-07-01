/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 16:25:55 by jmeirele          #+#    #+#             */
/*   Updated: 2025/07/01 17:05:31 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(const std::string &rawRequest) : _isChunked(false)
{
	std::istringstream stream(rawRequest);
	std::string line;

	// Storing the first line of the request in _method, _path and _httpVerion
	if (std::getline(stream, line))
	{
		std::istringstream requestLine(line);
		requestLine >> _method >> _path >> _httpVersion;
	}

	// Extract query string
	size_t qpos = _path.find('?');
	if (qpos != std::string::npos) {
		_queryString = _path.substr(qpos + 1);
		_path = _path.substr(0, qpos);
	}

	// Storing all headers in _headers and trimming both key and value
	while (std::getline(stream, line))
	{
		if (line == "\r" || line.empty())
			break;

		std::size_t colonPos = line.find(":");
		if (colonPos != std::string::npos)
		{
			std::string key = line.substr(0, colonPos);
			std::string value = line.substr(colonPos + 1);

			key.erase(key.find_last_not_of(" \t\r\n") + 1);
			value.erase(0, value.find_first_not_of(" \t"));
			value.erase(value.find_last_not_of(" \t\r\n") + 1);
			_headers[key] = value;
		}
	}

	// After headers are parsed, check for chunked encoding
	_processTransferEncoding();

	// Getting the body
	std::ostringstream bodyStream;
	bodyStream << stream.rdbuf();
	_body = bodyStream.str();

	// If chunked encoding, decode the body
	if (_isChunked && !_body.empty()) {
		_body = _decodeChunkedBody(_body);

		// Update Content-Length header
		std::ostringstream oss;
		oss << _body.size();
		_headers["Content-Length"] = oss.str();

		// Remove Transfer-Encoding header
		_headers.erase("Transfer-Encoding");
	}
}

const std::string &Request::getReqMethod() const { return _method; }
const std::string &Request::getReqPath() const { return _path; }
const std::string &Request::getReqHttpVersion() const { return _httpVersion; }
const std::string &Request::getReqBody() const { return _body; }

const std::string &Request::getReqHeaderKey(const std::string &key) const
{
	static const std::string empty = "";
	std::map<std::string, std::string>::const_iterator it = _headers.find(key);
	if (it != _headers.end())
		return it->second;
	return empty;
}

const std::map<std::string, std::string> &Request::getReqHeaders() const
{
	std::map<std::string, std::string>::const_iterator it = _headers.begin();
	while (it != _headers.end())
	{
		std::cout << it->first << "|" << it->second << std::endl;
		it++;
	}
	return _headers;
}

const std::string &Request::getReqQueryString() const { return _queryString; }

void Request::_processTransferEncoding()
{
	std::string transferEncoding = getReqHeaderKey("Transfer-Encoding");
	if (transferEncoding == "chunked") {
		_isChunked = true;
	}
}

std::string Request::_decodeChunkedBody(const std::string& chunkedBody)
{
	std::string decodedBody;
	size_t pos = 0;

	while (pos < chunkedBody.length()) {
		// Find end of chunk size line
		size_t crlfPos = chunkedBody.find("\r\n", pos);
		if (crlfPos == std::string::npos) break;

		// Parse chunk size (hexadecimal)
		std::string chunkSizeStr = chunkedBody.substr(pos, crlfPos - pos);
		if (chunkSizeStr.empty()) break;

		size_t chunkSize;
		try {
			chunkSize = std::stoul(chunkSizeStr, nullptr, 16);
		} catch (const std::exception&) {
			break; // Invalid chunk size
		}

		// If chunk size is 0, we've reached the end
		if (chunkSize == 0) break;

		// Move to start of chunk data
		pos = crlfPos + 2;

		// Ensure we don't read beyond buffer
		if (pos + chunkSize > chunkedBody.length()) break;

		// Extract chunk data
		decodedBody += chunkedBody.substr(pos, chunkSize);

		// Move past chunk data and trailing CRLF
		pos += chunkSize + 2;
	}

	return decodedBody;
}
