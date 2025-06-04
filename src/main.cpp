/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 13:21:37 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/04 20:03:04 by jmeirele         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc/webserv.hpp"
#include "src/server/Server.hpp"
#include "src/server/ServerConfig.hpp"
#include "src/utils/Logger.hpp"

int main() {
	ServerConfig config;
	
	Server server(config);
	server.runServer();
	
	return 0;
}