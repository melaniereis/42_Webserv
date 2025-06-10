/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 13:21:37 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/09 21:40:35 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/webserv.hpp"
#include "./server/Server.hpp"
#include "./config/ServerConfig.hpp"
#include "./config/ConfigParser.hpp"
#include "./config/LocationConfig.hpp"
#include "./utils/Logger.hpp"

int main(int argc, char** argv)
{
	std::string configPath;

	// Check if a configuration file is provided as an argument
	if (argc == 1)
	{
		std::cerr << "No configuration file provided. Using default: default.conf" << std::endl;
		configPath = "./config/valid/default.conf";
	}
	else if (argc > 2)
	{
		std::cerr << "Too many arguments. Usage: " << argv[0] << " <config_file>" << std::endl;
		return 1;
	}
	else
	{
		configPath = argv[1];
	}

	try
	{
		// Parse the configuration file
		Logger::info("Parsing configuration file: " + configPath);
		ConfigParser parser(configPath);
		std::vector<ServerConfig> serverConfigs = parser.parse();

		if (serverConfigs.empty())
		{
			std::cerr << "No valid server configurations found in " << configPath << std::endl;
			return 1;
		}

		std::ostringstream oss;
		oss << "Loaded " << serverConfigs.size() << " server configuration(s) from " << configPath;
		Logger::info(oss.str());

		// For now we only start the first server configuration
		// In a real-world scenario, you might want to handle multiple server configurations
		// and start a server for each one.
		// This is a simplification for the sake of this example.
		if (!serverConfigs.empty())
		{
			Server server(serverConfigs[0]);

			Logger::info("Starting web server...");
			server.runServer();
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
	catch (...)
	{
		std::cerr << "Unknown error occurred" << std::endl;
		return 1;
	}

	return 0;
}
