/**
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Tasks for packing diffrent OS packets
 * @date Wed May  3 12:50:35 PM EEST 2023
 *
 *
 * @license GPL v3.0 or later
 *
 * Copyright (C) 2022  WolodiaM
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
// C++ libraries
#include "fstream"
#include "iostream"
#include "sstream"
#include "string"
// CBuild headers
#include "../CBuild/headers/filesystem++.hpp"
#include "../CBuild/headers/print.hpp"
#include "../CBuild/headers/system.hpp"
#include "../CBuild/headers/task/Task.hpp"
// Code
#ifndef __PACK_HPP__
#define __PACK_HPP__
class pack_base : public CBuild::Task {
       protected:
	std::string work_folder;
	std::string version_str() {
		std::ifstream version("./ppa/ubuntu/version");
		char str[10];
		version.getline(str, 10);
		version.close();
		return std::string(str).substr(0, 3);
	}
	int version_major() {
		std::string vstr = this->version_str();
		return std::stoi(vstr.substr(0, 1));
	}
	int version_minor() {
		std::string vstr = this->version_str();
		return std::stoi(vstr.substr(2));
	}

       public:
	pack_base(std::string name, std::string folder)
	    : CBuild::Task(name, {}) {
		this->work_folder = folder;
	}
};
class pack_deb : public pack_base {
       private:
	std::string changelog = "./deb/changelog.Debian.tmp";

       public:
	pack_deb() : pack_base("pack_deb", "deb/ubuntu") {}
	void call(std::vector<std::string> args __attribute_maybe_unused__) {}
};
#endif	// __PACK_HPP__
