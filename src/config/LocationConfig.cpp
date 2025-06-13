/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 16:28:01 by meferraz          #+#    #+#             */
/*   Updated: 2025/06/13 17:36:34 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationConfig.hpp"
#include <cctype> // for std::isalpha

LocationConfig::LocationConfig() :
	_path(""),
	_root(""),
	_autoindex(false),
	_upload_dir("")
{
	// Initialize all containers as empty
}

LocationConfig::~LocationConfig() {}

void LocationConfig::_validatePath(const std::string& path) const
{
	if (path.empty()) {
		throw std::runtime_error("Location path cannot be empty");
	}

	if (path[0] != '/') {
		throw std::runtime_error("Location path must start with '/': " + path);
	}
}

void LocationConfig::_validateMethod(const std::string& method) const
{
	const std::string validMethods[] = {"GET", "POST", "DELETE", "PUT", "HEAD"};
	const int count = sizeof(validMethods)/sizeof(validMethods[0]);

	for (int i = 0; i < count; i++) {
		if (method == validMethods[i]) {
			return;
		}
	}

	throw std::runtime_error("Invalid HTTP method: " + method);
}

void LocationConfig::_validateExtension(const std::string& ext) const
{
	if (ext.empty() || ext[0] != '.') {
		throw std::runtime_error("CGI extension must start with '.': " + ext);
	}

	for (size_t i = 1; i < ext.size(); i++) {
		if (!std::isalnum(ext[i]) && ext[i] != '_' && ext[i] != '-') {
			throw std::runtime_error("Invalid character in CGI extension: " + ext);
		}
	}
}

void LocationConfig::_validateStatusCode(int code) const
{
	if (code < 300 || code > 599) {
		std::ostringstream oss;
		oss << "Invalid redirect status code: " << code
			<< " (must be 3xx, 4xx or 5xx)";
		throw std::runtime_error(oss.str());
	}
}

void LocationConfig::setPath(const std::string& p)
{
	_validatePath(p);
	_path = p;
}

void LocationConfig::setRoot(const std::string& r)
{
	if (r.empty()) {
		throw std::runtime_error("Root path cannot be empty");
	}
	_root = r;
}

void LocationConfig::addIndex(const std::string& idx)
{
	if (idx.empty()) {
		throw std::runtime_error("Index file name cannot be empty");
	}
	_indexes.push_back(idx);
}

void LocationConfig::setAutoIndex(bool a)
{
	_autoindex = a;
}

void LocationConfig::addAllowedMethod(const std::string& m)
{
	_validateMethod(m);

	// Check for duplicates
	for (std::vector<std::string>::const_iterator it = _allowed_methods.begin();
		it != _allowed_methods.end(); ++it)
	{
		if (*it == m) {
			return; // Method already exists
		}
	}
	_allowed_methods.push_back(m);
}

void LocationConfig::setUploadDir(const std::string& dir)
{
	if (dir.empty()) {
		throw std::runtime_error("Upload directory cannot be empty");
	}
	_upload_dir = dir;
}

void LocationConfig::addRedirect(int code, const std::string& target)
{
	_validateStatusCode(code);

	if (target.empty()) {
		throw std::runtime_error("Redirect target cannot be empty");
	}
	_redirects[code] = target;
}

void LocationConfig::addCgi(const std::string& ext, const std::string& cgi_path)
{
	_validateExtension(ext);

	if (cgi_path.empty()) {
		throw std::runtime_error("CGI path cannot be empty");
	}
	_cgis[ext] = cgi_path;
}

void LocationConfig::setIndexes(const std::vector<std::string>& indexes)
{
	if (indexes.empty()) {
		throw std::runtime_error("Index list cannot be empty");
	}
	_indexes = indexes;
}

// Getters remain the same as before
const std::string& LocationConfig::getPath() const { return _path; }
const std::string& LocationConfig::getRoot() const { return _root; }
const std::vector<std::string>& LocationConfig::getIndexes() const { return _indexes; }
bool LocationConfig::isAutoIndex() const { return _autoindex; }
const std::vector<std::string>& LocationConfig::getAllowedMethods() const { return _allowed_methods; }
const std::string& LocationConfig::getUploadDir() const { return _upload_dir; }
const std::map<int, std::string>& LocationConfig::getRedirects() const { return _redirects; }
const std::map<std::string, std::string>& LocationConfig::getCgis() const { return _cgis; }
