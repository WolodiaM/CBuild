/**
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Hasher v3.0 implementation
 * @date Fri May 19 11:32:50 AM EEST 2023
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
#include "inttypes.h"
#include "iostream"
#include "regex"
#include "sstream"
#include "string"
#include "vector"
// CBuild headers
#include "../../headers/CBuild_defs.hpp"
#include "../../headers/filesystem++.hpp"
#include "../../headers/hash.hpp"
#include "../../headers/map.hpp"
#include "../../headers/optional.hpp"
#include "../../headers/print.hpp"
// Code
/* namespace CBuild */
namespace CBuild {
/**
 * @brief Constants
 */
namespace consts {
/**
 * @brief Regex for stripping c++ line comments from file
 */
std::regex cxx_line_comment("//.*");
/**
 * @brief Regex for stripping c multiline comments from file comtent
 */
std::regex c_multiline_comment("/\\*([\\s\\S]*?)\\*/",
			       std::regex_constants::multiline);
/**
 * @brief Regex for stripping doxygen comments from file comtent
 */
std::regex doxygen_comment("/\\*\\*([\\s\\S]*?)\\*/",
			   std::regex_constants::multiline);
/**
 * @brief Get include directives from file and parse file name in it
 */
std::regex include_parser("#include\\s+[\"<]([^\">]+)[\">]");
/**
 * @brief For checking for source file
 */
std::regex check_type("\\.(cpp|cxx|cc|c)");
}  // namespace consts
namespace types {
/**
 * @class file
 * @brief File structure
 */
struct file {
	/**
	 * @brief Absolute file path
	 */
	std::string abs_path;
	/**
	 * @brief Project includes
	 */
	std::vector<CBuild::types::file> includes;
	/**
	 * @brief New file hash
	 */
	uint64_t hash_new;
	/**
	 * @brief Old file hash
	 */
	lib::optional<uint64_t> hash_old;
	/**
	 * @brief Does file is cpp file
	 */
	bool cpp;
	/**
	 * @brief Does file changed
	 */
	bool change;
	/**
	 * @brief Create new file structure, simply init all values
	 *
	 * @param path => string -> Absolute file path
	 */
	file(std::string path) {
		this->change = false;
		this->hash_new = 0;
		this->hash_old.clear();
		this->abs_path = path;
		this->includes.clear();
		if (std::regex_search(this->abs_path,
				      CBuild::consts::check_type)) {
			this->cpp = true;
		} else {
			this->cpp = false;
		}
	}
};
/**
 * @class file_content
 * @brief Temporary struct for file content
 */
struct file_content {
	/**
	 * @brief Content of file
	 */
	std::string content;
	/**
	 * @brief Absolute path to file
	 */
	std::string path;
	/**
	 * @brief Includes
	 */
	std::vector<std::string> includes;
};
}  // namespace types
namespace vars {
std::vector<CBuild::types::file> filelist;
lib::map<std::string, uint64_t> headers;
lib::map<std::string, uint64_t> old_hashes;
}  // namespace vars
/**
 * @brief Get content of file
 *
 * @param path => std::string -> Path to file
 * @return std::string -> File content
 */
CBuild::types::file_content get_file_data(std::string path,
					  std::string included_from = "") {
	// Return variable
	std::string p = path;
	// Set path to file
	try {
		p = CBuild::fs::normalize_path(path, included_from);
	} catch (const std::exception& e) {
	}
	CBuild::types::file_content f;
	f.path = p;
	// Load file content to string
	std::ifstream file(path);
	std::stringstream buff;
	buff << file.rdbuf();
	file.close();
	std::string str = buff.str();
	// Strip comments from file
	str = std::regex_replace(str, CBuild::consts::cxx_line_comment, "");
	str = std::regex_replace(str, CBuild::consts::c_multiline_comment, "");
	str = std::regex_replace(str, CBuild::consts::doxygen_comment, "");
	f.content = str;
	// Get includes
	std::smatch match;
	std::string::const_iterator start(str.cbegin());
	// Base file path
	std::string base = CBuild::fs::base(path);
	while (std::regex_search(start, str.cend(), match,
				 CBuild::consts::include_parser)) {
		// Get file
		std::string head = match[1];
		bool err = false;
		// Convert to absolute path and check for system includes
		try {
			head = CBuild::fs::normalize_path(head, base);
		} catch (const std::exception& e) {
			err = true;
		}
		// Push path, if it is project include
		if (!err) {
			f.includes.push_back(head);
		}
		start = match.suffix().first;
	}
	return f;
}
/**
 * @brief djb2 hashing function for std::string
 *
 * @param str => std::string -> Input string
 * @return uint64_t -> Hash of data
 */
uint64_t hash(std::string str) {
	uint64_t hash = 5381;
	for (auto ch : str) {
		hash = ((hash << 5) + hash) + ch;
	}
	return hash;
}
// /**
//  * @brief FNV-1a hashing function for std::string
//  *
//  * @param str => std::string -> Input string
//  * @return uint64_t -> Hash of data
//  */
// uint64_t hash(std::string str) {
// 	uint64_t hash = 14695981039346656037ULL;
//	for (char c : str) {
// 		hash ^= static_cast<uint64_t>(c);
// 		hash *= 1099511628211ULL;
// 	}
// 	return hash;
// }
/**
 * @brief
 *
 * @param files
 * @param toolchain_id
 */
void procces_files(std::vector<std::string> files, std::string toolchain_id) {
	CBuild::print_full("\t\t\tCBuild hash v3.0", CBuild::color::MAGENTA);
	// For every input file
	for (auto file : files) {
		// Get file data
		auto data = CBuild::get_file_data(file);
		auto cpp = CBuild::types::file(data.path);
		// Get file hash
		cpp.hash_new = CBuild::hash(data.content);
		CBuild::print_full(
		    "Calculating hash for \"" + cpp.abs_path + "\"",
		    CBuild::color::GREEN);
		CBuild::print_full("Need to calculate hash...",
				   CBuild::color::RED);
		auto old = CBuild::vars::old_hashes.get(cpp.abs_path);
		if (old != NULL) {
			cpp.hash_old.set(*old);
		}
		// For every invclude
		for (auto inc : data.includes) {
			// Get file data
			auto hdata = CBuild::get_file_data(inc, cpp.abs_path);

			auto hpp = CBuild::types::file(hdata.path);
			// Check if hash is already stored in hash
			const uint64_t* hash = NULL;
			CBuild::print_full(
			    "Calculating hash for \"" + hpp.abs_path + "\"",
			    CBuild::color::GREEN);
			if ((hash = CBuild::vars::headers.get(hpp.abs_path)) !=
			    NULL) {
				// Use stored hash
				hpp.hash_new = *hash;
				CBuild::print_full("Using hash from cache.",
						   CBuild::color::GREEN);
			} else {
				// Calculate file hash
				hpp.hash_new = CBuild::hash(hdata.content);
				CBuild::print_full("Need to calculate hash...",
						   CBuild::color::RED);
			}
			auto old = CBuild::vars::old_hashes.get(hpp.abs_path);
			if (old != NULL) {
				hpp.hash_old.set(*old);
			}
			// Store hash
			try {
				CBuild::vars::headers.push_back_check(
				    hpp.abs_path, hpp.hash_new);
			} catch (std::exception& e) {
			}
			// Store proccesed include
			cpp.includes.push_back(hpp);
		}
		// Store proccesed file
		CBuild::vars::filelist.push_back(cpp);
	}
}
/**
 * @brief Store new hashes to file
 *
 * @param toolchain_id => std::string -> Id of toolchain
 */
void store_hash(std::string toolchain_id) {
	std::ofstream file(CBUILD_BUILD_DIR + "/" + toolchain_id + "/" +
			   CBUILD_HASH_DIR + "/" + CBUILD_HASH_FILE);
	for (auto cpp : CBuild::vars::filelist) {
		file << cpp.abs_path << " " << std::hex << cpp.hash_new << "\n";
	}
	for (int i = 0; i < CBuild::vars::headers.size(); i++) {
		auto hpp = CBuild::vars::headers.at(i);
		file << hpp.key << " " << std::hex << hpp.data << "\n";
	}
	file.close();
}
/**
 * @brief Load old hashes
 *
 * @param toolchain_id => std::string -> Id of toolchain
 */
void load_hash(std::string toolchain_id) {
	std::string path = CBUILD_BUILD_DIR + "/" + toolchain_id + "/" +
			   CBUILD_HASH_DIR + "/" + CBUILD_HASH_FILE;
	if (!CBuild::fs::exists(path)) {
		return;
	}
	std::ifstream file(path);
	std::string line;
	while (std::getline(file, line)) {
		std::istringstream iss(line);
		std::string key;
		uint64_t value;
		iss >> key >> std::hex >> value;

		try {
			CBuild::vars::old_hashes.push_back_check(key, value);
		} catch (const std::exception& e) {
		}
	}
	file.close();
}
}  // namespace CBuild
/* hash.hpp */
void CBuild::print_files() {
	CBuild::print_full("\t\t\tCBuild hash v3.0 - Filelist",
			   CBuild::color::MAGENTA);
	std::stringstream buff;
	for (auto file : CBuild::vars::filelist) {
		buff << "\t\t" << file.abs_path << "\n"
		     << std::hex << "New hash: " << file.hash_new
		     << ", old hash: ";
		if (file.hash_old.is()) {
			buff << std::hex << file.hash_old.get() << "\n";
		} else {
			buff << " null\n";
		}
		CBuild::print_full(buff.str());
		buff.str("");
		for (auto hfile : file.includes) {
			buff << "\t\t\t" << hfile.abs_path << "\n"
			     << std::hex << "\tNew hash: " << hfile.hash_new
			     << ", old hash: ";
			if (hfile.hash_old.is()) {
				buff << std::hex << hfile.hash_old.get()
				     << "\n";
			} else {
				buff << " null\n";
			}
			CBuild::print_full(buff.str());
			buff.str("");
		}
	}
}
std::vector<std::string> CBuild::get_files(std::vector<std::string> files,
					   std::string toolchain_id) {
	// Variables
	std::vector<std::string> ret;
	// Gather all file info and relevant hashes
	CBuild::load_hash(toolchain_id);
	CBuild::procces_files(files, toolchain_id);
	CBuild::store_hash(toolchain_id);
	// Check differences in hashes
	for (auto file : CBuild::vars::filelist) {
		if ((!file.hash_old.is()) ||
		    (file.hash_new != file.hash_old.get())) {
			ret.push_back(file.abs_path);
		} else {
			for (auto inc : file.includes) {
				if ((!inc.hash_old.is()) ||
				    (inc.hash_new != inc.hash_old.get())) {
					ret.push_back(file.abs_path);
				} else {
				}
			}
		}
	}
	// Return value
	return ret;
}
