/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpStatus.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/12 18:06:00 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/12 18:18:18 by jmeirele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/webserv.hpp"
#include "Response.hpp"

class HttpStatus
{
	public:
		static std::string getMessage(int code);
		static std::string generateHtmlBody(int code);

		static Response setErrorResponse(Response &response, int code);
	private:
};
