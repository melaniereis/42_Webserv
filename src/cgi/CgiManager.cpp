/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiManager.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025-07-12 08:26:03 by meferraz          #+#    #+#             */
/*   Updated: 2025-07-12 10:50:24 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiManager.hpp"

CgiManager::CgiManager() {}

CgiManager::~CgiManager() {
	for (std::map<pid_t, CgiProcess>::iterator it = processes_.begin();
		it != processes_.end(); ++it) {
		close(it->second.in_fd);
		close(it->second.out_fd);
	}
}

void CgiManager::addProcess(pid_t pid, int in_fd, int out_fd,
							const std::string& input, Client* client) {
	fcntl(in_fd, F_SETFL, O_NONBLOCK);
	fcntl(out_fd, F_SETFL, O_NONBLOCK);

	CgiProcess process;
	process.pid = pid;
	process.in_fd = in_fd;
	process.out_fd = out_fd;
	process.input = input;
	process.bytes_written = 0;
	process.client = client;
	process.input_complete = input.empty();
	process.output_complete = false;
	process.start_time = time(NULL);

	processes_[pid] = process;
	fd_to_pid_[in_fd] = pid;
	fd_to_pid_[out_fd] = pid;

	// Mark client as in CGI processing
	if (client) {
		client->setCgiProcessing(true);
	}
}

void CgiManager::removeProcess(pid_t pid) {
	std::map<pid_t, CgiProcess>::iterator it = processes_.find(pid);
	if (it != processes_.end()) {
		close(it->second.in_fd);
		close(it->second.out_fd);
		fd_to_pid_.erase(it->second.in_fd);
		fd_to_pid_.erase(it->second.out_fd);
		processes_.erase(it);
	}
}

void CgiManager::checkProcesses() {
	std::vector<pid_t> to_remove;

	for (std::map<pid_t, CgiProcess>::iterator it = processes_.begin();
		it != processes_.end(); ++it) {
		int status;
		pid_t result = waitpid(it->first, &status, WNOHANG);

		if (result == it->first) {
			to_remove.push_back(it->first);
		} else if (result == -1) {
			to_remove.push_back(it->first);
		}
	}

	for (std::vector<pid_t>::iterator it = to_remove.begin();
		it != to_remove.end(); ++it) {
		removeProcess(*it);
	}
}

void CgiManager::fillPollfds(std::vector<struct pollfd>& fds) {
	for (std::map<pid_t, CgiProcess>::iterator it = processes_.begin();
		it != processes_.end(); ++it) {
		struct pollfd pfd;

		if (!it->second.input_complete) {
			pfd.fd = it->second.in_fd;
			pfd.events = POLLOUT;
			pfd.revents = 0;
			fds.push_back(pfd);
		}

		if (!it->second.output_complete) {
			pfd.fd = it->second.out_fd;
			pfd.events = POLLIN;
			pfd.revents = 0;
			fds.push_back(pfd);
		}
	}
}

void CgiManager::handlePollEvents(const std::vector<struct pollfd>& fds) {
	time_t now = time(NULL);
	const time_t CGI_TIMEOUT = 10;  // 10 seconds timeout

	// Check for timed-out processes first
	std::map<pid_t, CgiProcess>::iterator it = processes_.begin();
	while (it != processes_.end()) {
		CgiProcess& process = it->second;
		if (!process.output_complete && (now - process.start_time > CGI_TIMEOUT)) {
			Logger::warn("CGI timeout for PID: " + intToString(process.pid));
			kill(process.pid, SIGKILL);

			// Close file descriptors
			close(process.in_fd);
			close(process.out_fd);

			// Remove from fd_to_pid_ map
			fd_to_pid_.erase(process.in_fd);
			fd_to_pid_.erase(process.out_fd);

			// Notify client
			if (process.client) {
				process.client->setCgiOutput("HTTP/1.1 504 Gateway Timeout\r\n"
											"Content-Type: text/html\r\n\r\n"
											"<h1>504 Gateway Timeout</h1>");
				process.client->setCgiProcessing(false);
			}

			// Erase the process and get the next iterator
			processes_.erase(it++);
		} else {
			++it;
		}
	}

	for (size_t i = 0; i < fds.size(); ++i) {
		int fd = fds[i].fd;
		short revents = fds[i].revents;

		if (fd_to_pid_.find(fd) == fd_to_pid_.end()) continue;

		pid_t pid = fd_to_pid_[fd];
		CgiProcess& process = processes_[pid];

		if (fd == process.in_fd && (revents & POLLOUT)) {
			size_t to_write = process.input.size() - process.bytes_written;
			ssize_t written = write(process.in_fd,
									process.input.data() + process.bytes_written,
									to_write);

			if (written > 0) {
				process.bytes_written += written;
				if (process.bytes_written >= process.input.size()) {
					process.input_complete = true;
					close(process.in_fd);
				}
			} else if (written < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
				process.input_complete = true;
				close(process.in_fd);
			}
			Logger::debug("Wrote " + intToString(written) + " bytes to CGI stdin");
		}

		if (fd == process.out_fd && (revents & POLLIN)) {
			char buffer[4096];
			ssize_t bytes_read = read(process.out_fd, buffer, sizeof(buffer));

			if (bytes_read > 0) {
				process.output.append(buffer, bytes_read);
				Logger::debug("Read " + intToString(bytes_read) + " bytes from CGI stdout");
			} else if (bytes_read <= 0) {
				Logger::debug("CGI stdout closed");
				process.output_complete = true;
				if (process.client) {
					Logger::debug("Sending CGI output to client: " + intToString(process.output.size()) + " bytes");
					if (process.client) {  // Add NULL check
						process.client->setCgiOutput(process.output);
						process.client->setCgiProcessing(false);
					}
				}
				close(process.out_fd);
			}
		}

		if (revents & (POLLERR | POLLHUP | POLLNVAL)) {
			if (fd == process.in_fd) {
				close(process.in_fd);
				process.input_complete = true;
			}
			if (fd == process.out_fd) {
				close(process.out_fd);
				process.output_complete = true;
			}
		}
	}
}

std::string CgiManager::intToString(int value) {
	std::ostringstream oss;
	oss << value;
	return oss.str();
}
