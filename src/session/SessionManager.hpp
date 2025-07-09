/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SessionManager.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 15:20:53 by meferraz          #+#    #+#             */
/*   Updated: 2025/07/08 17:53:57 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once

#include "../../inc/webserv.hpp"
#include "../utils/Logger.hpp"

class SessionManager
{
	public:
		static SessionManager& getInstance();
		std::string createSession();
		bool hasSession(const std::string& sessionId);
		std::string& getSessionData(const std::string& sessionId);
		void cleanupSessions();
		std::string intToString(int value);
		void updateSessionAccessTime(const std::string& sessionId);

	private:
		SessionManager();
		SessionManager(const SessionManager&);
		SessionManager& operator=(const SessionManager&);

		std::map<std::string, std::pair<std::string, time_t> > _sessions;
		static const int SESSION_TIMEOUT = 3600; // 1 hour
};
