/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/16 11:33:32 by meferraz          #+#    #+#             */
/*   Updated: 2025/06/16 14:32:10 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"
#include "../http/HttpStatus.hpp"

CgiHandler::CgiHandler(const Request &request, const ServerConfig &config, const LocationConfig &location) : _request(request), _config(config), _location(location)
{
	// 1) Determine CGI interpreter from extension
	std::string path = _request.getReqPath();
	size_t dotPos = path.find_last_of('.');
	if (dotPos != std::string::npos)
	{
		std::string ext = path.substr(dotPos);
		std::map<std::string, std::string> cgis = _location.getCgis();
		std::map<std::string, std::string>::const_iterator it = cgis.find(ext);
		if (it != cgis.end())
			_cgiPath = it->second;
		// else leave _cgiPath empty → will return 502 later
	}

	// 2) Build script path safely
	std::string locPath = _location.getPath();
	if (path.compare(0, locPath.length(), locPath) == 0)
	{
		std::string rel = path.substr(locPath.length());
		_scriptPath = _joinPath(_location.getRoot(), rel);
	}
}

CgiHandler::~CgiHandler() {}

Response CgiHandler::execute()
{
	Response response;

	// 0) CGI interpreter must exist
	if (!_isValidScript())
	{
		return HttpStatus::buildResponse(response, 502);
	}

	// 1) Prepare environment
	char **env = _prepareEnvironment();
	if (!env)
	{
		return HttpStatus::buildResponse(response, 500);
	}

	// 2) Execute and capture output
	std::string output = _executeScript(env);

	// 3) Cleanup env pointers
	_cleanupEnvironment(env);

	if (output.empty())
	{
		return HttpStatus::buildResponse(response, 500);
	}

	// 4) Split headers and body
	size_t headerEnd = output.find("\r\n\r\n");
	if (headerEnd == std::string::npos)
	{
		return HttpStatus::buildResponse(response, 500);
	}

	std::string headers = output.substr(0, headerEnd);
	std::string body = output.substr(headerEnd + 4);

	// 5) Parse each header line
	std::istringstream headerStream(headers);
	std::string headerLine;

	while (std::getline(headerStream, headerLine))
	{
		if (!headerLine.empty() && headerLine[headerLine.size() - 1] == '\r')
		{
			headerLine.erase(headerLine.size() - 1);
		}

		size_t colonPos = headerLine.find(':');
		if (colonPos == std::string::npos)
			continue;

		std::string key = headerLine.substr(0, colonPos);
		std::string value = headerLine.substr(colonPos + 1);

		// trim whitespace
		size_t keyStart = key.find_first_not_of(" \t");
		size_t keyEnd = key.find_last_not_of(" \t");
		if (keyStart != std::string::npos && keyEnd != std::string::npos)
		{
			key = key.substr(keyStart, keyEnd - keyStart + 1);
		}
		else
		{
			key = "";
		}
		// Convert key to lowercase for consistency
		std::transform(key.begin(), key.end(), key.begin(), ::tolower);

		// Handle status code specially
		if (key == "status")
		{
			int statusCode = atoi(value.c_str());
			if (statusCode >= 100 && statusCode < 600)
			{
				response.setStatus(statusCode, HttpStatus::getMessage(statusCode));
			}
		}
		else
		{
			response.setHeader(key, value);
		}
	}

	// 6) Finalize
	response.setBody(body);
	if (response.getStatusCode() == 0)
	{
		response.setStatus(200, "OK");
	}
	return response;
}

char **CgiHandler::_prepareEnvironment()
{
	std::vector<std::string> envVars;

	// Standard CGI variables
	envVars.push_back("REQUEST_METHOD=" + _request.getReqMethod());
	envVars.push_back("SCRIPT_FILENAME=" + _scriptPath);
	envVars.push_back("QUERY_STRING=" + _request.getReqQueryString());
	envVars.push_back("SERVER_PROTOCOL=HTTP/1.1");
	envVars.push_back("GATEWAY_INTERFACE=CGI/1.1");

	// Content headers
	std::string contentLength = _request.getReqHeaderKey("Content-Length");
	std::string contentType = _request.getReqHeaderKey("Content-Type");
	if (!contentLength.empty())
	{
		envVars.push_back("CONTENT_LENGTH=" + contentLength);
	}
	if (!contentType.empty())
	{
		envVars.push_back("CONTENT_TYPE=" + contentType);
	}

	// Server information
	std::string srvName = "";
	if (!_config.getServerNames().empty())
	{
		srvName = _config.getServerNames()[0];
	}
	envVars.push_back("SERVER_NAME=" + srvName);

	std::ostringstream portStr;
	portStr << _config.getServerPort();
	envVars.push_back("SERVER_PORT=" + portStr.str());

	// Path information
	envVars.push_back("PATH_INFO=" + _request.getReqPath());
	envVars.push_back("DOCUMENT_ROOT=" + _location.getRoot());

	// Allocate environment array
	char **env = new char *[envVars.size() + 1];
	for (size_t i = 0; i < envVars.size(); ++i)
	{
		env[i] = new char[envVars[i].size() + 1];
		std::strcpy(env[i], envVars[i].c_str());
	}
	env[envVars.size()] = NULL;

	return env;
}

void CgiHandler::_cleanupEnvironment(char **env)
{
	if (!env)
		return;
	for (size_t i = 0; env[i]; i++)
	{
		delete[] env[i];
	}
	delete[] env;
}

std::string CgiHandler::_executeScript(char **env)
{
	int pipe_in[2], pipe_out[2];
	if (pipe(pipe_in))
	{
		perror("pipe in");
		return "";
	}
	if (pipe(pipe_out))
	{
		perror("pipe out");
		close(pipe_in[0]);
		close(pipe_in[1]);
		return "";
	}

	pid_t pid = fork();
	if (pid < 0)
	{
		perror("fork");
		close(pipe_in[0]);
		close(pipe_in[1]);
		close(pipe_out[0]);
		close(pipe_out[1]);
		return "";
	}

	if (pid == 0)
	{ // child process
		// Close unused pipe ends
		close(pipe_in[1]);
		close(pipe_out[0]);

		// Redirect stdin and stdout
		dup2(pipe_in[0], STDIN_FILENO);
		dup2(pipe_out[1], STDOUT_FILENO);

		// Close original pipe ends
		close(pipe_in[0]);
		close(pipe_out[1]);

		// Prepare arguments
		char *argv[3];
		argv[0] = const_cast<char *>(_cgiPath.c_str());
		argv[1] = const_cast<char *>(_scriptPath.c_str());
		argv[2] = NULL;

		// Execute CGI
		execve(_cgiPath.c_str(), argv, env);

		// If execve fails
		perror("execve");
		std::exit(EXIT_FAILURE);
	}

	// Parent process
	close(pipe_in[0]);
	close(pipe_out[1]);

	// Write POST body if exists
	if (_request.getReqMethod() == "POST")
	{
		const std::string &body = _request.getReqBody();
		if (!body.empty())
		{
			ssize_t written = 0;
			size_t total = body.size();
			const char *data = body.c_str();

			while (written < static_cast<ssize_t>(total))
			{
				ssize_t ret = write(pipe_in[1], data + written, total - written);
				if (ret <= 0)
				{
					perror("write to CGI stdin");
					break;
				}
				written += ret;
			}
		}
	}
	close(pipe_in[1]);

	// Read output with timeout
	std::string output;
	char buffer[4096];
	int fd = pipe_out[0];
	const size_t MAX_OUTPUT_SIZE = 10 * 1024 * 1024; // 10 MB
	time_t start_time = time(NULL);
	const time_t TIMEOUT = 10; // 10 seconds

	while (output.size() < MAX_OUTPUT_SIZE)
	{
		fd_set readSet;
		FD_ZERO(&readSet);
		FD_SET(fd, &readSet);

		struct timeval timeout;
		time_t elapsed = time(NULL) - start_time;
		time_t remaining = TIMEOUT - elapsed;

		if (remaining <= 0)
		{
			Logger::error("CGI script timeout");
			break;
		}

		timeout.tv_sec = remaining;
		timeout.tv_usec = 0;

		int ret = select(fd + 1, &readSet, NULL, NULL, &timeout);
		if (ret < 0)
		{
			perror("select");
			break;
		}
		else if (ret == 0)
		{
			// Timeout
			Logger::error("CGI script timeout");
			break;
		}

		if (FD_ISSET(fd, &readSet))
		{
			ssize_t bytes = read(fd, buffer, sizeof(buffer));
			if (bytes < 0)
			{
				if (errno != EAGAIN && errno != EWOULDBLOCK)
				{
					perror("read from CGI stdout");
				}
				break;
			}
			else if (bytes == 0)
			{
				// EOF
				break;
			}
			output.append(buffer, bytes);
		}
	}

	close(fd);

	// Reap child process with timeout
	int status;
	time_t wait_start = time(NULL);
	const time_t WAIT_TIMEOUT = 5; // 5 seconds
	int wait_result = -1;
	const int MAX_ITERATIONS = 50; // 50 * 100ms = 5 seconds
	int iterations = 0;

	while (iterations < MAX_ITERATIONS)
	{
		wait_result = waitpid(pid, &status, WNOHANG);

		if (wait_result == pid)
		{
			// Child exited normally
			break;
		}
		else if (wait_result == 0)
		{
			// Child still running
			time_t elapsed = time(NULL) - wait_start;
			if (elapsed >= WAIT_TIMEOUT)
			{
				kill(pid, SIGKILL);
				waitpid(pid, &status, 0); // Wait for killed child
				break;
			}
			usleep(100000); // Sleep 100ms
			iterations++;
		}
		else
		{
			// Error occurred
			perror("waitpid");
			break;
		}
	}

	// Final safety: kill if still running
	if (iterations >= MAX_ITERATIONS)
	{
		kill(pid, SIGKILL);
		waitpid(pid, &status, 0);
	}

	if (output.size() >= MAX_OUTPUT_SIZE)
	{
		Logger::error("CGI output exceeded maximum size");
		return "";
	}

	return output;
}

std::string CgiHandler::_joinPath(const std::string &a, const std::string &b)
{
	if (a.empty())
		return b;
	if (b.empty())
		return a;

	if (a[a.size() - 1] == '/')
	{
		return a + (b[0] == '/' ? b.substr(1) : b);
	}
	else
	{
		return a + (b[0] == '/' ? b : "/" + b);
	}
}

bool CgiHandler::_isSafePath(const std::string &path)
{
	// Prevent directory traversal
	if (path.find("../") != std::string::npos ||
		path.find("/..") != std::string::npos)
	{
		return false;
	}

	// Check if file exists and is executable
	struct stat statbuf;
	if (stat(path.c_str(), &statbuf))
	{
		return false; // File doesn't exist
	}

	// Check if it's a regular file
	if (!S_ISREG(statbuf.st_mode))
	{
		return false; // Not a regular file
	}

	// Check execute permission
	if (access(path.c_str(), X_OK))
	{
		return false; // Not executable
	}

	return true;
}

bool CgiHandler::_isValidScript()
{
	return !_cgiPath.empty() && _isSafePath(_scriptPath);
}
