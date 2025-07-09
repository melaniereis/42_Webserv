/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SessionManager.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/08 15:21:48 by meferraz          #+#    #+#             */
/*   Updated: 2025/07/09 21:24:30 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SessionManager.hpp"

SessionManager& SessionManager::getInstance() {
	static SessionManager instance;
	return instance;
}

SessionManager::SessionManager()
{
	std::srand(static_cast<unsigned int>(std::time(NULL)));
    Logger::debug("SessionManager initialized");
}

std::string SessionManager::createSession() {
	const std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	const int length = 32;
	std::string sessionId;

	for (int i = 0; i < length; ++i) {
		sessionId += charset[std::rand() % charset.size()];
	}

	_sessions[sessionId] = std::make_pair("", std::time(NULL));
	Logger::debug("Created new session: " + sessionId);
	return sessionId;
}
bool SessionManager::hasSession(const std::string& sessionId) {
	std::map<std::string, std::pair<std::string, time_t> >::iterator it = _sessions.find(sessionId);
	if (it == _sessions.end()) {
		Logger::debug("Session not found: " + sessionId);
		return false;
	}

	time_t now = std::time(NULL);
	if (now - it->second.second > SESSION_TIMEOUT) {
		Logger::debug("Session expired: " + sessionId);
		_sessions.erase(it);
		return false;
	}

	return true;
}

void SessionManager::updateSessionAccessTime(const std::string& sessionId) {
	std::map<std::string, std::pair<std::string, time_t> >::iterator it = _sessions.find(sessionId);
	if (it != _sessions.end()) {
		it->second.second = std::time(NULL);
		Logger::debug("Updated access time for session: " + sessionId);
	}
}

// src/session/SessionManager.cpp
std::string& SessionManager::getSessionData(const std::string& sessionId) {
	std::map<std::string, std::pair<std::string, time_t> >::iterator it = _sessions.find(sessionId);
	if (it == _sessions.end()) {
		// This should never happen, but handle it gracefully
		Logger::error("Attempt to access non-existent session: " + sessionId);
		static std::string empty;
		return empty;
	}
	return it->second.first;
}

// src/session/SessionManager.cpp
void SessionManager::cleanupSessions() {
	time_t now = std::time(NULL);
	int count = 0;
	std::map<std::string, std::pair<std::string, time_t> >::iterator it = _sessions.begin();

	Logger::debug("Session cleanup started. Current sessions: " + intToString(_sessions.size()));

	while (it != _sessions.end()) {
		time_t age = now - it->second.second;
		if (age > SESSION_TIMEOUT) {
			Logger::debug("Cleaning up expired session: " + it->first + " (age: " + intToString(age) + "s)");
			_sessions.erase(it++);
			count++;
		} else {
			++it;
		}
	}

	Logger::info("Session cleanup: Removed " + intToString(count) + " expired sessions");
	Logger::debug("Sessions remaining: " + intToString(_sessions.size()));
}

std::string SessionManager::intToString(int value) {
	std::ostringstream oss;
	oss << value;
	return oss.str();
}
