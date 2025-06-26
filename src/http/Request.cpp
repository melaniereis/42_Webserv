/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 16:25:55 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/26 20:46:28 by jmeirele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(const std::string &rawRequest)
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

	_path = normalizePath(_path);

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

	// Getting the body
	std::ostringstream bodyStream;
	bodyStream << stream.rdbuf();
	_body = bodyStream.str();
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


const std::string Request::normalizePath(const std::string &path)
{
	std::string result;
	bool prevSlash = false;

	for (size_t i = 0; i < path.length(); ++i)
	{
		if (path[i] == '/')
		{
			if (!prevSlash)
			{
				result += '/';
				prevSlash = true;
			}
		}
		else
		{
			result += path[i];
			prevSlash = false;
		}
	}
	return result.empty() ? "/" : result;
}
