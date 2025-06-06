/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 13:21:37 by jmeirele          #+#    #+#             */
<<<<<<< HEAD
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
=======
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
>>>>>>> 67bfcd04716ebe35ec69a6b3ff92227defe2b8b9
