/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 13:21:37 by jmeirele          #+#    #+#             */
/*   Updated: 2025/08/11 15:26:41 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/webserv.hpp"
#include "./servers/WebServer.hpp"

int main(int argc, char** argv)
{
	try
	{
		if (argc > 2)
		{
			throw std::runtime_error("Too many arguments. Usage: " +
									std::string(argv[0]) + " [config_file]");
		}

		std::string configPath = (argc == 2) ? argv[1] : "./config/valid/default.conf";
		if (argc == 1)
		{
			Logger::warn("No configuration file provided. Using default: default.conf");
		}

		WebServer webServer(configPath);
		webServer.run();
	}
	catch (const std::exception& e)
	{
		Logger::error(e.what());
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
