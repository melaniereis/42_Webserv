/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 18:31:25 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/09 18:48:20 by jmeirele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

Response RequestHandler::handle(const Request &request)
{
	if (request.getReqMethod() == "GET")
		return RequestHandler::handleGetMethod(request);
	else if (request.getReqMethod() == "POST")
		return RequestHandler::handlePostMethod(request);
	else if (request.getReqMethod() == "DELETE")
		return RequestHandler::handleDeleteMethod(request);
	else
	{
		Response response;
		response.setStatus(405, "Method not allowed");
		return response;
	}
}

Response RequestHandler::handleGetMethod(const Request &request)
{
	(void)request;
	Response res;
	return res;
}

Response RequestHandler::handlePostMethod(const Request &request)
{
	(void)request;
	Response res;
	return res;
}

Response RequestHandler::handleDeleteMethod(const Request &request)
{
	(void)request;
	Response res;
	return res;
}
