/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 17:34:46 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/05 17:37:07 by jmeirele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/webserv.hpp"

class Response
{
	public:
		Response();

		void setStatus(int code, const std::string &message);
		void setHeader(const std::string &key, const std::string &value);
		void setBody(const std::string &body);
		std::string toString() const;
		
	private:
		int _statusCode;
		std::string _statusMessage;
		std::map<std::string, std::string> _headers;
		std::string _body;
};