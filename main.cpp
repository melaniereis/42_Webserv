/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 13:21:37 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/03 16:31:11 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc/webserv.hpp"

#include "Server.hpp"
#include <iostream>

int main(int argc, char** argv) {
    std::string config_path = "config/default.conf";
    if (argc == 2)
        config_path = argv[1];

    try {
        Server server(config_path);
        server.run(); // Handles poll loop and connections
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
