/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 13:21:37 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/05 17:16:00 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/webserv.hpp"
#include "../src/server/Server.hpp"
#include "../src/server/ServerConfig.hpp"
#include "../src/utils/Logger.hpp"

int main()
{
	ServerConfig config;

	Server server(config);
	server.runServer();

	return 0;
}
