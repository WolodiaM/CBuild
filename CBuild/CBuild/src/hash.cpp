/**
 * @file hash.cpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief File hashing implementation
 * @version 1.0
 * @date 2023-02-13
 *
 *
 * @license GPL v3.0 or later
 *
 * Copyright (C) 2023  WolodiaM
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
#include "filesystem"
#include "fstream"
#include "iomanip"
#include "iostream"
#include "regex"
#include "sstream"
#include "string"
#include "vector"
// Project headers
#include "../../headers/CBuild_defs.hpp"
#include "../../headers/hash.hpp"
#include "../../headers/map.hpp"
#include "../../headers/print.hpp"
/* sha256 */
// std::string sha256(const std::string str) {
// 	unsigned char hash[SHA256_DIGEST_LENGTH];
//
// 	SHA256_CTX sha256;
// 	SHA256_Init(&sha256);
// 	SHA256_Update(&sha256, str.c_str(), str.size());
// 	SHA256_Final(hash, &sha256);
//
// 	std::stringstream buff;
//
// 	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
// 		buff << std::hex << std::setw(2) << std::setfill('0')
// 		     << static_cast<int>(hash[i]);
// 	}
// 	return buff.str();
// }
// std::vector<std::string> changed;
// /* hash.hpp */
// bool CBuild::hash_match_store(std::string hash_file, std::string code_file) {
// 	// Open files
// 	std::ifstream hashf(hash_file);
// 	std::ifstream file(code_file);
// 	// Load code file
// 	std::stringstream buffer;
// 	buffer << file.rdbuf();
// 	// Get hash of file
// 	std::string hash = sha256(buffer.str());
// 	// Load saved hash
// 	std::stringstream saved;
// 	saved << hashf.rdbuf();
// 	// Compare hashes
// 	bool ret = false;
// 	if (saved.str() == std::string("-")) {
// 		ret = false;
// 	} else if (saved.str() != hash) {
// 		ret = false;
// 	} else if (saved.str() == hash) {
// 		ret = true;
// 	}
// 	// Close hash file ond overwrite its content with new hash
// 	hashf.close();
// 	std::ofstream hashfw(hash_file);
// 	hashfw << hash;
// 	// Close all files and return value
// 	file.close();
// 	hashfw.close();
// 	return ret;
// }
//
// bool CBuild::load_hash(std::string hash, std::string code) {  // Load files
// 	std::ifstream hfile(hash);
// 	std::ifstream cfile(code);
// 	// Load code file content
// 	std::stringstream buffer;
// 	buffer << cfile.rdbuf();
// 	// Get hash of file
// 	std::string hash_str = sha256(buffer.str());
// 	// Load saved hash
// 	std::stringstream saved;
// 	saved << hfile.rdbuf();
// 	// Compare hashes
// 	// True  - match
// 	// False - not match
// 	bool ret = false;
// 	if (saved.str() == std::string("-")) {
// 		ret = false;
// 	} else if (saved.str() != hash_str) {
// 		ret = false;
// 	} else if (saved.str() == hash_str) {
// 		ret = true;
// 	}
// 	// Close files
// 	cfile.close();
// 	hfile.close();
// 	// Return
// 	return ret;
// }
// void CBuild::save_hashes(std::vector<std::string> hash_files,
// 			 std::vector<std::string> code_files) {
// 	// Error
// 	if (hash_files.size() != code_files.size()) {
// 		CBuild::print(
// 		    "Error, too many files in one of list given for hashing!");
// 		exit(0xAA);
// 	}
// 	for (unsigned int i = 0; i < hash_files.size(); i++) {
// 		// Load files from list
// 		std::string hash = hash_files.at(i);
// 		std::string code = code_files.at(i);
// 		// Load files
// 		std::ofstream hfile(hash);
// 		std::ifstream cfile(code);
// 		// Load code file content
// 		std::stringstream buffer;
// 		buffer << cfile.rdbuf();
// 		// Get hash of file
// 		std::string hash_str = sha256(buffer.str());
// 		// Write hash to file
// 		hfile << hash_str;
// 		// Close files
// 		cfile.close();
// 		hfile.close();
// 	}
// }
// New hasher
namespace CBuild {
/**
 * @class cpp
 * @brief Struct for saving properties of file
 *
 */
struct cpp {
	/**
	 * @brief Absolute path to cpp file
	 */
	std::string cpp_path_abs;
	/**
	 * @brief Absolute pathes to included headers
	 */
	std::vector<std::string> hpp_paths_abs;
};
// Strip c++-stile comments
std::regex comment_regex1("\\/\\/.*");
// Strip c-style and doxygen comments
std::regex comment_regex2("/\\*[^*]*\\*+(?:[^/*][^*]*\\*+)*/");
// Include search regex
std::regex include_search("#include\\s+[\"<]([^\">]+)[\">]");
// Table of file-hash
lib::map<std::string, uint64_t> hash_table_old, hash_table_new;
// Headers return variable
std::vector<CBuild::cpp> files;
// Last file
CBuild::cpp last_file_tmp;
/**
 * @brief Get content of file, stripped from comments
 *
 * @param path => std::string -> Path to file
 * @return std::string -> Content of file
 */
std::string get_file_content(std::string path) {
	std::string ret = "";
	std::ifstream f(path);
	std::string line;
	CBuild::cpp file;
	file.cpp_path_abs = std::filesystem::absolute(path).string();
	// Load file and strip comments
	while (std::getline(f, line)) {
		line = std::regex_replace(line, CBuild::comment_regex1, "");
		ret += line;
		ret += "\n";
	}
	f.close();
	ret = std::regex_replace(ret, CBuild::comment_regex2, "");
	// Get included files
	std::smatch match;
	std::string::const_iterator search_start(ret.cbegin());
	auto base = std::filesystem::path(path).parent_path();
	while (std::regex_search(search_start, ret.cend(), match,
				 CBuild::include_search)) {
		std::string head = match[1];
		bool err = false;
		try {
			head = std::filesystem::canonical(
				   base / std::filesystem::path(head))
				   .string();
		} catch (const std::exception& e) {
			err = true;
		}
		if (!err) file.hpp_paths_abs.push_back(head);
		search_start = match.suffix().first;
	}
	CBuild::last_file_tmp = file;
	return ret;
}
/**
 * @brief Get hash of file
 *
 * @param path => std::string -> Path to file
 * @return uint64_t -> Hash of file
 */
uint64_t hash(std::string path) {
	std::string str = CBuild::get_file_content(path);
	// FNV-1a
	// uint64_t hash = 14695981039346656037ULL;
	// for (char c : str) {
	// 	hash ^= static_cast<uint64_t>(c);
	// 	hash *= 1099511628211ULL;
	// }
	// djb2
	uint64_t hash = 5381;
	for (auto ch : str) {
		hash = ((hash << 5) + hash) + ch;
	}
	return hash;
}
/**
 * @brief Save hash table from map
 *
 * @param toolchain => std::string -> Toolchain id
 */
void save_hash_table(std::string toolchain) {
	std::filesystem::remove(
	    std::filesystem::path(CBUILD_BUILD_DIR + "/" + toolchain + "/" +
				  CBUILD_HASH_DIR + "/" + CBUILD_HASH_FILE));
	std::ofstream file(CBUILD_BUILD_DIR + "/" + toolchain + "/" +
			   CBUILD_HASH_DIR + "/" + CBUILD_HASH_FILE);
	for (unsigned int i = 0; i < CBuild::hash_table_new.size(); i++) {
		file << CBuild::hash_table_new.at(i).key << " " << std::hex
		     << CBuild::hash_table_new.at(i).data << "\n";
	}
	file.close();
}
/**
 * @brief Load hash table to map
 *
 * @param toolchain => std::string -> Toolchain id
 */
void load_hash_table(std::string toolchain) {
	std::ifstream file(CBUILD_BUILD_DIR + "/" + toolchain + "/" +
			   CBUILD_HASH_DIR + "/" + CBUILD_HASH_FILE);
	std::string sbuff;
	uint64_t ubuff;
	while (file >> sbuff >> std::hex >> ubuff) {
		CBuild::hash_table_old.push_back_check(sbuff, ubuff);
	}
	file.close();
}
}  // namespace CBuild
std::vector<std::string> CBuild::get_files(std::vector<std::string> files,
					   std::string toolchain_id) {
	// Load old hashes
	CBuild::load_hash_table(toolchain_id);
	std::vector<std::string> ret;
	// Generate new hashes
	for (auto elem : files) {
		bool err = false;
		try {
			CBuild::hash_table_new.push_back_check(
			    std::filesystem::canonical(
				std::filesystem::path(elem))
				.string(),
			    CBuild::hash(elem));
		} catch (std::exception& e) {
			err = true;
		}
		if (!err) CBuild::files.push_back(CBuild::last_file_tmp);
	}
	for (unsigned int i = 0; i < CBuild::files.size(); i++) {
		auto cpp = CBuild::files.at(i);
		for (unsigned int j = 0; j < cpp.hpp_paths_abs.size(); j++) {
			auto hpp = cpp.hpp_paths_abs.at(j);
			try {
				CBuild::hash_table_new.push_back_check(
				    std::filesystem::canonical(
					std::filesystem::path(hpp))
					.string(),
				    CBuild::hash(hpp));
			} catch (std::exception& e) {
			}
		}
	}
	// Check files
	CBuild::print_full("CBuild hash v2.0 - dbg:");
	std::stringstream str;
	for (auto file : CBuild::files) {
		const uint64_t* old_hash =
		    CBuild::hash_table_old.get(file.cpp_path_abs);
		const uint64_t* new_hash =
		    CBuild::hash_table_new.get(file.cpp_path_abs);
		str.str("");
		str << "File (c): " << file.cpp_path_abs << ", hash: old - "
		    << (old_hash == NULL ? 0 : *old_hash) << ", new - "
		    << (new_hash == NULL ? 0 : *new_hash)
		    << ", match: " << std::boolalpha
		    << ((old_hash != NULL && new_hash != NULL)
			    ? (*old_hash == *new_hash)
			    : false);
		CBuild::print_full(str.str());
		if (old_hash == NULL && new_hash != NULL) {
			ret.push_back(file.cpp_path_abs);
		} else if (old_hash != NULL && new_hash != NULL) {
			if (*old_hash != *new_hash) {
				ret.push_back(file.cpp_path_abs);
			} else {
				for (auto hpp : file.hpp_paths_abs) {
					auto old_hpp_hash =
					    CBuild::hash_table_old.get(hpp);
					auto new_hpp_hash =
					    CBuild::hash_table_new.get(hpp);
					str.str("");
					str << "File (h): " << hpp
					    << ", hash: old - "
					    << ((old_hpp_hash == NULL)
						    ? 0
						    : *old_hpp_hash)
					    << ", new - "
					    << ((new_hpp_hash == NULL)
						    ? 0
						    : *new_hpp_hash)
					    << ", match: " << std::boolalpha
					    << ((old_hpp_hash != NULL &&
						 new_hpp_hash != NULL)
						    ? (*old_hpp_hash ==
						       *new_hpp_hash)
						    : false);
					CBuild::print_full(str.str());
					if (old_hpp_hash == NULL &&
					    new_hpp_hash != NULL) {
						ret.push_back(
						    file.cpp_path_abs);
					} else if (old_hpp_hash != NULL &&
						   new_hpp_hash != NULL) {
						if (*old_hpp_hash !=
						    *new_hpp_hash)
							ret.push_back(
							    file.cpp_path_abs);
					}
				}
			}
		}
	}
	// for (auto elem : ret) {
	// 	std::cout << elem << "\n";
	// }
	// Save new hashes
	CBuild::save_hash_table(toolchain_id);
	return ret;
}
