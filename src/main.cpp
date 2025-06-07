/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 13:21:37 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/07 11:32:00 by meferraz         ###   ########.fr       */
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

	if (argc == 1)
	{
		std::cerr << "No configuration file provided. Using default: default.conf" << std::endl;
		configPath = "default.conf";
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

	ConfigParser parser(configPath);
	std::vector<ServerConfig> serverConfigs = parser.parse();

	if (serverConfigs.empty())
	{
		std::cerr << "No valid server configurations found in " << configPath << std::endl;
		return 1;
	}

	for (std::vector<ServerConfig>::iterator it = serverConfigs.begin(); it != serverConfigs.end(); ++it)
	{
		Server server(*it);
		server.runServer();
	}

	std::ostringstream oss;
	oss << "Web server started with " << serverConfigs.size() << " server(s).";
	Logger::info(oss.str());

	return 0;
}
