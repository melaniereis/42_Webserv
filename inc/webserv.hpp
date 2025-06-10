/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 13:18:32 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/09 22:18:08 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// C++ Standard
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

// Networking / Sockets
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>

// Filesystem & CGI
#include <sys/stat.h>
#include <dirent.h>
#include <signal.h>
#include <sys/wait.h>

// Error Handling
#include <cerrno>
#include <cstring>

#include <set>
#include <arpa/inet.h>

// Includes
#include "macros.hpp"

