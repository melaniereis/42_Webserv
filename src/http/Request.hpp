/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 16:20:12 by jmeirele          #+#    #+#             */
/*   Updated: 2025/08/11 17:05:35 by jmeirele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../../inc/webserv.hpp"
#include "../utils/Logger.hpp"

class Request
{
	public:
		Request(const std::string &rawRequest);

	const std::string &getReqMethod() const;
	const std::string &getReqPath() const;
	const std::string &getReqHttpVersion() const;
	const std::string &getReqBody() const;
	const std::string &getReqHeaderKey(const std::string &key) const;
	const std::map<std::string, std::string> &getReqHeaders() const;
	const std::string &getReqQueryString() const;
	const std::string normalizePath(const std::string &path);

private:
	std::string _method;
	std::string _path;
	std::string _httpVersion;
	std::string _body;
	std::map<std::string, std::string> _headers;
	std::string _queryString;
	bool _isChunked;
	std::string _decodeChunkedBody(const std::string& chunkedBody);
	void _processTransferEncoding();
};

// headers will hold
// Host | User-Agent | Accept | Content-Type
// Content-Length | Connection | Authorization
