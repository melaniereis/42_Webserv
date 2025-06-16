/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 17:34:46 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/16 14:32:43 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../../inc/webserv.hpp"

class Response
{
public:
	Response();

	void setStatus(int code, const std::string &message);
	void setHeader(const std::string &key, const std::string &value);
	void setBody(const std::string &body);
	std::string toString() const;

	int getStatusCode() const;

private:
	int _statusCode;
	std::string _statusMessage;
	std::map<std::string, std::string> _headers;
	std::string _body;
};
