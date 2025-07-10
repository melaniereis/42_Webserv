/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/16 11:33:32 by meferraz          #+#    #+#             */
/*   Updated: 2025/07/10 14:41:07 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"
#include "../http/HttpStatus.hpp"

CgiHandler::CgiHandler(const Request &request,
					const ServerConfig &config,
					const LocationConfig &location)
	: _request(request), _config(config), _location(location)
{}

CgiHandler::~CgiHandler() {}

Response CgiHandler::execute()
{
	Response response;
	std::string scriptPath = _resolveScriptPath();
	Logger::info("Executing CGI script: " + scriptPath);

	if (!_validateScript(scriptPath, response)) {
		return response;
	}

	_initEnv();
	std::string output = _executeCgiScript(scriptPath);
	if (output.empty()) {
		_handleCgiError(response);
		return response;
	}

	_parseCgiOutput(output, response);
	return response;
}
bool CgiHandler::_validateScript(const std::string& scriptPath, Response& response)
{
	if (access(scriptPath.c_str(), F_OK) != 0) {
		HttpStatus::buildResponse(response, 404);
		return false;
	}

	// Check if it's a regular file
	struct stat stat_buf;
	if (stat(scriptPath.c_str(), &stat_buf) != 0 || !S_ISREG(stat_buf.st_mode)) {
		HttpStatus::buildResponse(response, 403);
		return false;
	}

	// Verify execute permissions
	if (access(scriptPath.c_str(), X_OK) != 0) {
		HttpStatus::buildResponse(response, 403);
		return false;
	}

	return true;
}

void CgiHandler::_handleCgiError(Response& response)
{
	Logger::error("CGI execution failed");
	HttpStatus::buildResponse(response, 500);
}

std::string CgiHandler::_resolveScriptPath() const
{
	std::string base = _location.getRoot();
	if (base.empty()) {
		base = _config.getServerRoot();
	}

	std::string path = _request.getReqPath();
	std::string locPath = _location.getPath();

	if (path.find(locPath) == 0) {
		path = path.substr(locPath.length());
	}

	// Handle trailing slashes
	if (base[base.length() - 1] == '/') {
		return (path[0] == '/')
			? base + path.substr(1)
			: base + path;
	} else {
		return (path[0] != '/')
			? base + "/" + path
			: base + path;
	}
}

void CgiHandler::_initEnv()
{
	_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	_env["SERVER_SOFTWARE"] = "webserv/1.0";
	_env["QUERY_STRING"] = _request.getReqQueryString();
	_env["REQUEST_METHOD"] = _request.getReqMethod();
	_env["SCRIPT_NAME"] = _request.getReqPath();
	_env["SCRIPT_FILENAME"] = _resolveScriptPath();
	_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	_env["SERVER_NAME"] = _config.getServerHost();
	_env["SERVER_PORT"] = _intToString(_config.getServerPort());
	_env["PATH_INFO"] = _request.getReqPath();
	_env["PATH_TRANSLATED"] = _resolveScriptPath();
	// Set CONTENT_LENGTH based on actual body size (after dechunking)
	std::ostringstream contentLength;
	contentLength << _request.getReqBody().size();
	_env["CONTENT_LENGTH"] = contentLength.str();

	// Enhanced cookie handling
	std::string cookies = getCookies();
	if (!cookies.empty()) {
		_env["HTTP_COOKIE"] = cookies;
		Logger::debug("Set HTTP_COOKIE: " + cookies);
	}

	// Handle query string cookie setting (like your friend's PHP)
	std::string queryString = _request.getReqQueryString();
	if (!queryString.empty()) {
		_env["QUERY_STRING"] = queryString;
		// You could also pre-process query params for cookie setting here
	}

	// Add all headers as HTTP_* variables
	const std::map<std::string, std::string>& headers = _request.getReqHeaders();
	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
		std::string env_var = "HTTP_" + it->first;
		std::replace(env_var.begin(), env_var.end(), '-', '_');
		std::transform(env_var.begin(), env_var.end(), env_var.begin(), ::toupper);
		_env[env_var] = it->second;
	}
}

char** CgiHandler::_createEnvArray() const
{
	char** env = new char*[_env.size() + 1];
	int i = 0;
	for (std::map<std::string, std::string>::const_iterator it = _env.begin();
		it != _env.end(); ++it)
	{
		std::string envVar = it->first + "=" + it->second;
		env[i] = new char[envVar.length() + 1];
		std::strcpy(env[i], envVar.c_str());
		i++;
	}
	env[i] = NULL;
	return env;
}

void CgiHandler::_cleanupEnvArray(char** env) const
{
	for (int i = 0; env[i]; i++) {
		delete[] env[i];
	}
	delete[] env;
}

bool CgiHandler::_setupPipes(int pipeIn[2], int pipeOut[2])
{
	if (pipe(pipeIn) < 0) {
		int err = errno;
		Logger::error("pipeIn failed: " + std::string(strerror(err)));
		return false;
	}
	if (pipe(pipeOut) < 0) {
		int err = errno;
		Logger::error("pipeOut failed: " + std::string(strerror(err)));
		close(pipeIn[0]);
		close(pipeIn[1]);
		return false;
	}
	return true;
}

void CgiHandler::_childProcess(int pipeIn[2], int pipeOut[2],
							const std::string& scriptPath)
{
	close(pipeIn[1]);
	close(pipeOut[0]);

	// Convert relative script path to absolute path BEFORE chdir
	char* absolutePath = realpath(scriptPath.c_str(), NULL);
	if (!absolutePath) {
		perror("realpath failed");
		exit(EXIT_FAILURE);
	}

	std::string absScriptPath(absolutePath);
	free(absolutePath);

	// Extract directory for chdir (RFC 3875 compliance)
	size_t lastSlash = absScriptPath.find_last_of('/');
	if (lastSlash != std::string::npos) {
		std::string scriptDir = absScriptPath.substr(0, lastSlash);

		// REQUIRED: Set working directory to script's directory
		if (chdir(scriptDir.c_str()) != 0) {
			perror("chdir failed - RFC 3875 compliance error");
			exit(EXIT_FAILURE);
		}
	}

	if (dup2(pipeIn[0], STDIN_FILENO) < 0) {
		perror("dup2 stdin failed");
		exit(EXIT_FAILURE);
	}
	if (dup2(pipeOut[1], STDOUT_FILENO) < 0) {
		perror("dup2 stdout failed");
		exit(EXIT_FAILURE);
	}

	char** env = _createEnvArray();
	std::map<std::string, std::string> cgiMap = _location.getCgis();
	std::string interpreter;
	std::string resolvedScript = absScriptPath;

	// Extract extension and find matching interpreter
	size_t dotPos = scriptPath.find_last_of('.');
	if (dotPos != std::string::npos)
	{
		std::string ext = scriptPath.substr(dotPos);
		if (cgiMap.find(ext) != cgiMap.end())
		{
			interpreter = cgiMap[ext];
		}
	}

	// Prepare arguments for execve
	char* argv[3] = { NULL, NULL, NULL };
	if (!interpreter.empty())
	{
		argv[0] = const_cast<char*>(interpreter.c_str());
		argv[1] = const_cast<char*>(resolvedScript.c_str());
	} else
	{
		argv[0] = const_cast<char*>(resolvedScript.c_str());
	}

	execve(argv[0], argv, env);
	perror("execve failed");
	_cleanupEnvArray(env);
	exit(EXIT_FAILURE);
}

void CgiHandler::_parentProcess(int pipeIn[2], int pipeOut[2],
							pid_t pid, std::string& output)
{
	close(pipeIn[0]);
	close(pipeOut[1]);

	const std::string& body = _request.getReqBody();
	if (!body.empty()) {
		ssize_t bytesWritten = write(pipeIn[1], body.c_str(), body.size());
		if (bytesWritten < 0) {
			int err = errno;
			Logger::error("write to CGI failed: " + std::string(strerror(err)));
		}
	}
	close(pipeIn[1]);

	char buffer[4096];
	ssize_t bytesRead;
	while ((bytesRead = read(pipeOut[0], buffer, sizeof(buffer) - 1)) > 0) {
		buffer[bytesRead] = '\0';
		output.append(buffer, bytesRead);
	}
	close(pipeOut[0]);

	int status;
	waitpid(pid, &status, 0);
	if (WIFEXITED(status)) {
		if (WEXITSTATUS(status) != EXIT_SUCCESS) {
			Logger::error("CGI script exited with status: " +
						_intToString(WEXITSTATUS(status)));
			output.clear();
		}
	} else {
		Logger::error("CGI script did not exit normally");
		output.clear();
	}
}

std::string CgiHandler::_executeCgiScript(const std::string& scriptPath)
{
	int pipeIn[2], pipeOut[2];
	pid_t pid;
	std::string output;

	if (!_setupPipes(pipeIn, pipeOut)) {
		return "";
	}

	pid = fork();
	if (pid < 0) {
		int err = errno;
		Logger::error("fork failed: " + std::string(strerror(err)));
		close(pipeIn[0]); close(pipeIn[1]);
		close(pipeOut[0]); close(pipeOut[1]);
		return "";
	}

	if (pid == 0) {
		_childProcess(pipeIn, pipeOut, scriptPath);
	} else {
		_parentProcess(pipeIn, pipeOut, pid, output);
	}
	return output;
}

static std::string trim(const std::string& str)
{
	size_t first = str.find_first_not_of(" \t\r\n");
	if (first == std::string::npos) {
		return "";
	}
	size_t last = str.find_last_not_of(" \t\r\n");
	return str.substr(first, (last - first + 1));
}

void CgiHandler::_parseCgiOutput(const std::string& output, Response& response)
{
	size_t headerEnd = output.find("\r\n\r\n");
	bool doubleNewline = false;
	if (headerEnd == std::string::npos) {
		headerEnd = output.find("\n\n");
		doubleNewline = true;
		if (headerEnd == std::string::npos) {
			response.setBody(output);
			response.setStatus(200, "OK");
			return;
		}
	}

	std::string headers = output.substr(0, headerEnd);
	std::string body = output.substr(headerEnd + (doubleNewline ? 2 : 4));
	response.setBody(body);

	std::istringstream headerStream(headers);
	std::string headerLine;
	while (std::getline(headerStream, headerLine)) {
		if (headerLine.empty()) continue;
		if (headerLine[headerLine.size()-1] == '\r') {
			headerLine.erase(headerLine.size()-1);
		}

		if (headerLine.find("Status:") == 0) {
			std::string status = headerLine.substr(7);
			status = trim(status);
			size_t spacePos = status.find(' ');
			if (spacePos != std::string::npos) {
				std::string code = status.substr(0, spacePos);
				std::string message = status.substr(spacePos + 1);
				response.setStatus(atoi(code.c_str()), message);
			} else {
				response.setStatus(atoi(status.c_str()), "OK");
			}
		}
		else {
			size_t colonPos = headerLine.find(':');
			if (colonPos != std::string::npos) {
				std::string key = headerLine.substr(0, colonPos);
				std::string value = headerLine.substr(colonPos + 1);
				value = trim(value);
				response.setHeader(key, value);
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

// Enhanced getCookies method:
std::string CgiHandler::getCookies() {
	std::string cookies;
	const std::map<std::string, std::string>& cookieMap = _request.getCookies();

	for (std::map<std::string, std::string>::const_iterator it = cookieMap.begin();
		it != cookieMap.end(); ++it) {
		if (!cookies.empty()) {
			cookies += "; ";
		}
		cookies += it->first + "=" + it->second;
	}

	Logger::debug("Formatted cookies for CGI: " + cookies);
	return cookies;
}
