/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 13:21:37 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/12 22:19:46 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/webserv.hpp"
#include "./server/Server.hpp"
#include "./config/ServerConfig.hpp"
#include "./config/ConfigParser.hpp"
#include "./config/LocationConfig.hpp"
#include "./utils/Logger.hpp"

void testLocationFinding(const ServerConfig& server) {
	std::cout << "\n===== Testing Location Finding =====" << std::endl;
	std::cout << "Server root: " << server.getServerRoot() << "\n" << std::endl;

	// Print all locations first
	const std::map<std::string, LocationConfig>& locations = server.getLocations();
	std::cout << "All Locations in Server:" << std::endl;
	for (std::map<std::string, LocationConfig>::const_iterator it = locations.begin();
		it != locations.end(); ++it) {
		std::cout << "  - " << it->first << " => " << it->second.getPath() << std::endl;
	}

	// Define test paths with expected matches
	const struct {
		const char* path;
		const char* expected;
	} testCases[] = {
		{"/", "/"},
		{"/index.html", "/"},
		{"/images/logo.png", "/images/"},
		{"/images", "/images/"},      // Updated expectation
		{"/images/", "/images/"},
		{"/cgi-bin/test.py", "/cgi-bin/"},
		{"/cgi-bin", "/cgi-bin/"},    // Updated expectation
		{"/cgi-bin/", "/cgi-bin/"},
		{"/upload", "/upload"},
		{"/upload/file.txt", "/upload"},
		{"/docs", "/"},
		{"/nonexistent", "/"},
		{"/images/subdir/file.jpg", "/images/"},
		{"/cgi-bin/subdir/script.pl", "/cgi-bin/"}
	};

	const int testCount = sizeof(testCases) / sizeof(testCases[0]);

	for (int i = 0; i < testCount; ++i) {
		const std::string& path = testCases[i].path;
		const std::string& expected = testCases[i].expected;
		const LocationConfig* loc = server.findLocation(path);

		std::cout << "\nTesting path: '" << path << "'" << std::endl;
		std::cout << "  Expected: '" << expected << "'" << std::endl;

		if (loc) {
			std::cout << "  Found:    '" << loc->getPath() << "'";

			// Check if we got the expected location
			if (loc->getPath() == expected) {
				std::cout << " \033[32m[PASS]\033[0m";
			} else {
				std::cout << " \033[31m[FAIL]\033[0m";
				// Print why it might have failed
				std::cout << "\n  Why: Found '" << loc->getPath() << "' but expected '" << expected << "'";
			}
		} else {
			std::cout << "  Found:    NULL";
			std::cout << " \033[31m[FAIL]\033[0m";
			std::cout << "\n  Why: No location found for path '" << path << "'";
		}
		std::cout << std::endl;
	}
}

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

		if (!serverConfigs.empty())
		{
			for (size_t i = 0; i < serverConfigs.size(); ++i)
			{
				Server server = serverConfigs[i];
				ServerConfig& config = serverConfigs[i];

				for (size_t i = 0; i < serverConfigs.size(); ++i) {
				std::cout << "\n\n==== Testing Server " << i + 1 << " ====" << std::endl;
				testLocationFinding(serverConfigs[i]);
				}

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
