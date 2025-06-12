/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 13:21:37 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/11 22:00:44 by meferraz         ###   ########.fr       */
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

		std::string index;
		size_t indexCount = 0;
		while (indexCount < serverConfigs[0].getServerIndexes().size())
		{
			index = serverConfigs[0].getServerIndexes()[indexCount++];
			std::cout << "Index: " << index << std::endl;
		}

		std::ostringstream oss;
		oss << "Loaded " << serverConfigs.size() << " server configuration(s) from " << configPath;
		Logger::info(oss.str());

		if (!serverConfigs.empty())
		{
			for (size_t i = 0; i < serverConfigs.size(); ++i)
			{
				Server server = serverConfigs[i];
				ServerConfig& config = serverConfigs[i];

				Logger::info("Starting web server " + std::to_string(i + 1) + " on " +
					config.getServerHost() + ":" + std::to_string(config.getServerPort()));
				server.runServer();
			}
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
