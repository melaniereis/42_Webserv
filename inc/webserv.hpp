/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmeirele <jmeirele@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 13:18:32 by jmeirele          #+#    #+#             */
/*   Updated: 2025/06/04 20:06:32 by jmeirele         ###   ########.fr       */
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

// Includes
#include "macros.hpp"

