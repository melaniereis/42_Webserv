/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiManager.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025-07-12 08:24:47 by meferraz          #+#    #+#             */
/*   Updated: 2025-07-12 10:49:12 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once


#include "../client/Client.hpp"

class Client;

struct CgiProcess
{
	pid_t pid;
	int in_fd;
	int out_fd;
	std::string input;
	size_t bytes_written;
	std::string output;
	Client* client;
	bool input_complete;
	bool output_complete;
	time_t start_time;
};

class CgiManager
{
	public:
		CgiManager();
		~CgiManager();

		void addProcess(pid_t pid, int in_fd, int out_fd,
					const std::string& input, Client* client);
		void removeProcess(pid_t pid);
		void checkProcesses();
		void fillPollfds(std::vector<struct pollfd>& fds);
		void handlePollEvents(const std::vector<struct pollfd>& fds);

	private:
		std::map<pid_t, CgiProcess> processes_;
		std::map<int, pid_t> fd_to_pid_;
		std::string intToString(int value);
};
