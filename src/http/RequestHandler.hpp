/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/09 18:19:04 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/09 18:44:51 by jmeirele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Request.hpp"
#include "Response.hpp"

class RequestHandler
{
	public:
		static Response handle(const Request &request);
		static Response handleGetMethod(const Request &request);
		static Response handlePostMethod(const Request &request);
		static Response handleDeleteMethod(const Request &request);
	private:
};
