/**
 * @file hash.cpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief SHA256 implementation
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
#include "fstream"
#include "iomanip"
#include "iostream"
#include "openssl/sha.h"
#include "sstream"
#include "string"
#include "vector"
// Project headers
#include "../../headers/hash.hpp"
/* sha256 */
std::string sha256(const std::string str) {
	unsigned char hash[SHA256_DIGEST_LENGTH];

	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, str.c_str(), str.size());
	SHA256_Final(hash, &sha256);

	std::stringstream buff;

	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
		buff << std::hex << std::setw(2) << std::setfill('0')
		     << static_cast<int>(hash[i]);
	}
	return buff.str();
}
std::vector<std::string> changed;
/* hash.hpp */
bool CBuild::hash_match_store(std::string hash_file, std::string code_file) {
	// Open files
	std::ifstream hashf(hash_file);
	std::ifstream file(code_file);
	// Load code file
	std::stringstream buffer;
	buffer << file.rdbuf();
	// Get hash of file
	std::string hash = sha256(buffer.str());
	// Load saved hash
	std::stringstream saved;
	saved << hashf.rdbuf();
	// Compare hashes
	bool ret = false;
	if (saved.str() == std::string("-")) {
		ret = false;
	} else if (saved.str() != hash) {
		ret = false;
	} else if (saved.str() == hash) {
		ret = true;
	}
	// Close hash file ond overwrite its content with new hash
	hashf.close();
	std::ofstream hashfw(hash_file);
	hashfw << hash;
	// Close all files and return value
	file.close();
	hashfw.close();
	return ret;
}

bool CBuild::load_hash(std::string hash, std::string code) {  // Load files
	std::ifstream hfile(hash);
	std::ifstream cfile(code);
	// Load code file content
	std::stringstream buffer;
	buffer << cfile.rdbuf();
	// Get hash of file
	std::string hash_str = sha256(buffer.str());
	// Load saved hash
	std::stringstream saved;
	saved << hfile.rdbuf();
	// Compare hashes
	// True  - match
	// False - not match
	bool ret = false;
	if (saved.str() == std::string("-")) {
		ret = false;
	} else if (saved.str() != hash_str) {
		ret = false;
	} else if (saved.str() == hash_str) {
		ret = true;
	}
	// Close files
	cfile.close();
	hfile.close();
	// Return
	return ret;
}
void CBuild::save_hashes(std::vector<std::string> hash_files,
			 std::vector<std::string> code_files) {
	// Error
	if (hash_files.size() != code_files.size()) {
		puts("Error, too many files in one of list given for hashing!");
		exit(0xAA);
	}
	for (unsigned int i = 0; i < hash_files.size(); i++) {
		// Load files from list
		std::string hash = hash_files.at(i);
		std::string code = code_files.at(i);
		// Load files
		std::ofstream hfile(hash);
		std::ifstream cfile(code);
		// Load code file content
		std::stringstream buffer;
		buffer << cfile.rdbuf();
		// Get hash of file
		std::string hash_str = sha256(buffer.str());
		// Write hash to file
		hfile << hash_str;
		// Close files
		cfile.close();
		hfile.close();
	}
}

// New hasher
std::vector<std::string> CBuild::get_files(std::vector<std::string> files,
					   std::string toolchain_id) {
	std::vector<std::string> ret;

	return ret;
}
