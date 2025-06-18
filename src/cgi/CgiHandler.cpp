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

CgiHandler::CgiHandler(const Request &request, const ServerConfig &config, const LocationConfig &location)
	: _request(request), _config(config), _location(location)
{}

CgiHandler::~CgiHandler() {}

Response CgiHandler::execute()
{
	Response response;
	std::string scriptPath = _resolveScriptPath();
	std::cout << "Executing CGI script: " << scriptPath << std::endl;

	// Verify script exists and is executable
	if (access(scriptPath.c_str(), F_OK)) {
		return HttpStatus::buildResponse(response, 404);
	}
	if (access(scriptPath.c_str(), X_OK)) {
		return HttpStatus::buildResponse(response, 403);
	}

	// Prepare environment
	_initEnv();

	// Execute CGI
	std::string output = _executeCgiScript(scriptPath);
	if (output.empty()) {
		return HttpStatus::buildResponse(response, 500);
	}

	// Parse CGI output
	_parseCgiOutput(output, response);
	return response;
}
std::string CgiHandler::_resolveScriptPath()
{
	// Use location's root if available, otherwise server root
	std::string base = _location.getRoot();
	if (base.empty()) {
		base = _config.getServerRoot();
	}

	std::string path = _request.getReqPath();
	std::string locPath = _location.getPath();

	// Remove location prefix from request path
	if (path.find(locPath) == 0) {
		path = path.substr(locPath.length());
	}

	// Properly join paths with directory separator
	if (base[base.length()-1] == '/' && !path.empty() && path[0] == '/') {
		return base + path.substr(1);
	} else if (base[base.length()-1] != '/' && !path.empty() && path[0] != '/') {
		return base + "/" + path;
	} else {
		return base + path;
	}
}

void CgiHandler::_initEnv()
{
	// Set required CGI environment variables
	_env["QUERY_STRING"] = _request.getReqQueryString();
	_env["REQUEST_METHOD"] = _request.getReqMethod();
	_env["SCRIPT_NAME"] = _request.getReqPath();
	_env["SCRIPT_FILENAME"] = _resolveScriptPath();
	_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	_env["SERVER_NAME"] = _config.getServerHost();
	_env["SERVER_PORT"] = _intToString(_config.getServerPort());
	_env["CONTENT_TYPE"] = _request.getReqHeaderKey("Content-Type");
	_env["CONTENT_LENGTH"] = _request.getReqHeaderKey("Content-Length");
	_env["HTTP_COOKIE"] = _request.getReqHeaderKey("Cookie");
	_env["HTTP_USER_AGENT"] = _request.getReqHeaderKey("User-Agent");
	_env["PATH_INFO"] = _request.getReqPath();
}

std::string CgiHandler::_executeCgiScript(const std::string& scriptPath)
{
	int pipeIn[2], pipeOut[2];
	pid_t pid;
	std::string output;

	if (pipe(pipeIn) || pipe(pipeOut)) {
		return "";
	}

	pid = fork();
	if (pid == -1) {
		close(pipeIn[0]); close(pipeIn[1]);
		close(pipeOut[0]); close(pipeOut[1]);
		return "";
	}

	if (pid == 0) { // Child process
		close(pipeIn[1]);
		close(pipeOut[0]);

		dup2(pipeIn[0], STDIN_FILENO);
		dup2(pipeOut[1], STDOUT_FILENO);

		// Convert environment map to char **
		char **env = new char*[_env.size() + 1];
		int i = 0;
		for (std::map<std::string, std::string>::iterator it = _env.begin();
			it != _env.end(); ++it)
		{
			std::string envVar = it->first + "=" + it->second;
			env[i] = new char[envVar.length() + 1];
			strcpy(env[i], envVar.c_str());
			i++;
		}
		env[i] = NULL;

		// Get interpreter for script extension
		std::map<std::string, std::string> cgiMap = _location.getCgis();
		size_t dotPos = scriptPath.find_last_of('.');
		std::string interpreter;
		if (dotPos != std::string::npos) {
			std::string ext = scriptPath.substr(dotPos);
			if (cgiMap.find(ext) != cgiMap.end()) {
				interpreter = cgiMap[ext];
			}
		}

		char *argv[3] = { NULL, NULL, NULL };
		if (!interpreter.empty()) {
			argv[0] = const_cast<char*>(interpreter.c_str());
			argv[1] = const_cast<char*>(scriptPath.c_str());
		} else {
			argv[0] = const_cast<char*>(scriptPath.c_str());
		}

		execve(argv[0], argv, env);

		// Cleanup if execve fails
		for (int j = 0; env[j]; j++) {
			delete[] env[j];
		}
		delete[] env;
		exit(1);
	} else { // Parent process
		close(pipeIn[0]);
		close(pipeOut[1]);

		// Write request body to CGI stdin
		if (!_request.getReqBody().empty()) {
			write(pipeIn[1], _request.getReqBody().c_str(), _request.getReqBody().size());
		}
		close(pipeIn[1]);

		// Read CGI output
		char buffer[4096];
		ssize_t bytesRead;
		while ((bytesRead = read(pipeOut[0], buffer, sizeof(buffer)-1)) > 0) {
			buffer[bytesRead] = '\0';
			output.append(buffer);
		}
		close(pipeOut[0]);

		// Wait for child to finish
		int status;
		waitpid(pid, &status, 0);
		if (WIFEXITED(status)) {
			if (WEXITSTATUS(status)) {
				return "";
			}
		} else {
			return "";
		}
	}

	return output;
}

void CgiHandler::_parseCgiOutput(const std::string& output, Response& response)
{
	size_t headerEnd = output.find("\r\n\r\n");
	if (headerEnd == std::string::npos) {
		headerEnd = output.find("\n\n");
		if (headerEnd == std::string::npos) {
			response.setBody(output);
			response.setStatus(200, "OK");
			return;
		}
	}

	std::string headers = output.substr(0, headerEnd);
	std::string body = output.substr(headerEnd + 4);

	response.setBody(body);

	// Parse headers
	std::istringstream headerStream(headers);
	std::string headerLine;
	while (std::getline(headerStream, headerLine)) {
		size_t colonPos = headerLine.find(':');
		if (colonPos != std::string::npos) {
			std::string key = headerLine.substr(0, colonPos);
			std::string value = headerLine.substr(colonPos + 1);
			// Trim whitespace
			key.erase(key.find_last_not_of(" \t\r\n") + 1);
			value.erase(0, value.find_first_not_of(" \t"));
			value.erase(value.find_last_not_of(" \t\r\n") + 1);
			response.setHeader(key, value);
		} else if (!headerLine.empty()) {
			// Handle status line
			if (headerLine.substr(0, 7) == "Status:") {
				std::string status = headerLine.substr(7);
				status.erase(0, status.find_first_not_of(" \t"));
				size_t spacePos = status.find(' ');
				if (spacePos != std::string::npos) {
					response.setStatus(std::atoi(status.substr(0, spacePos).c_str()),
								status.substr(spacePos+1));
				} else {
					response.setStatus(std::atoi(status.c_str()), "OK");
				}
			}
		}
	}

	if (response.getStatusCode() == 0) {
		response.setStatus(200, "OK");
	}
}

std::string CgiHandler::_intToString(int value) const
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}
