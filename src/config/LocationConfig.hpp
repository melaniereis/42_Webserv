/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: meferraz <meferraz@student.42porto.pt>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/06 16:21:53 by meferraz          #+#    #+#             */
/*   Updated: 2025/06/13 17:36:42 by meferraz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include "ServerConfig.hpp"

class ServerConfig;

class LocationConfig
{
public:
	LocationConfig();
	~LocationConfig();

	// Setters with validation
	void setPath(const std::string& p);
	void setRoot(const std::string& r);
	void addIndex(const std::string& idx);
	void setAutoIndex(bool a);
	void addAllowedMethod(const std::string& m);
	void setUploadDir(const std::string& dir);
	void addRedirect(int code, const std::string& target);
	void addCgi(const std::string& ext, const std::string& cgi_path);
	void setIndexes(const std::vector<std::string>& indexes);

	// Getters
	const std::string& getPath() const;
	const std::string& getRoot() const;
	const std::vector<std::string>& getIndexes() const;
	bool isAutoIndex() const;
	const std::vector<std::string>& getAllowedMethods() const;
	const std::string& getUploadDir() const;
	const std::map<int, std::string>& getRedirects() const;
	const std::map<std::string, std::string>& getCgis() const;

private:
	std::string _path;
	std::string _root;
	std::vector<std::string> _indexes;
	bool _autoindex;
	std::vector<std::string> _allowed_methods;
	std::string _upload_dir;
	std::map<int, std::string> _redirects;
	std::map<std::string, std::string> _cgis;

	void _validatePath(const std::string& path) const;
	void _validateMethod(const std::string& method) const;
	void _validateExtension(const std::string& ext) const;
	void _validateStatusCode(int code) const;
};
