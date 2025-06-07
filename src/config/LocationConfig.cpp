/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 16:28:01 by meferraz          #+#    #+#             */
/*   Updated: 2025/06/06 16:32:31 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationConfig.hpp"

// Constructor: initialize members in the body
LocationConfig::LocationConfig()
{
	_path.clear();
	_root.clear();
	_indexes.clear();
	_autoindex = false;
	_allowed_methods.clear();
	_upload_dir.clear();
	_redirects.clear();
	_cgis.clear();
}

LocationConfig::~LocationConfig() {}

// Setters
void LocationConfig::setPath(const std::string& p) { _path = p; }
void LocationConfig::setRoot(const std::string& r) { _root = r; }
void LocationConfig::addIndex(const std::string& idx) { _indexes.push_back(idx);}
void LocationConfig::setAutoIndex(bool a) { _autoindex = a; }
void LocationConfig::addAllowedMethod(const std::string& m) { _allowed_methods.push_back(m);}
void LocationConfig::setUploadDir(const std::string& dir) { _upload_dir = dir;}
void LocationConfig::addRedirect(int code, const std::string& target) { _redirects[code] = target; }
void LocationConfig::addCgi(const std::string& ext, const std::string& cgi_path) { _cgis[ext] = cgi_path; }
void LocationConfig::setIndexes(const std::vector<std::string>& indexes)
{
	_indexes = indexes;
}
// Getters
const std::string& LocationConfig::getPath() const { return _path; }
const std::string& LocationConfig::getRoot() const { return _root; }
const std::vector<std::string>& LocationConfig::getIndexes() const { return _indexes; }
bool LocationConfig::isAutoIndex() const { return _autoindex; }
const std::vector<std::string>& LocationConfig::getAllowedMethods() const { return _allowed_methods; }
const std::string& LocationConfig::getUploadDir() const { return _upload_dir; }
const std::map<int, std::string>& LocationConfig::getRedirects() const { return _redirects; }
const std::map<std::string, std::string>& LocationConfig::getCgis() const { return _cgis; }
