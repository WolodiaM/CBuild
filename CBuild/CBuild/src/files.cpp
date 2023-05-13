/**
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Filebuffer for CBuild ecosystem
 * @date Fri May 12 10:58:05 AM EEST 2023
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
#include "string"
// Project headers
#include "../../headers/CBuild_defs.hpp"
#include "../../headers/files.hpp"
#include "../../headers/filesystem++.hpp"
/* files.hpp */
int CBuild::fs::replace(std::string file, std::string token, std::string data) {
	// Path to tmp file
	std::string cache;
	cache = CBUILD_CACHE_DIR + "/" + CBUILD_COPY_CACHE_DIR + "/" +
		CBuild::fs::path_to_file(file) + ".tmp";
	// std::cout << cache << "\n";
	// Open two files
	CBuild::fs::create({cache}, CBuild::fs::FILE);
	std::ofstream tmp_file(cache);
	std::ifstream main_file;
	main_file.open(file);
	// Number of replaced tokens
	static int check_num;
	check_num = 0;
	// Read and copy file to temp file line by line
	std::string line;
	while (std::getline(main_file, line)) {
		// If token found - replace it and increment counter
		while (line.find(token) != std::string::npos) {
			line.replace(line.find(token), token.size(), data);
			check_num++;
		}
		tmp_file << line << "\n";
	}
	// Close files
	main_file.close();
	tmp_file.close();
	// Replace file by it's temp version
	CBuild::fs::remove(file, true);
	CBuild::fs::move(cache, file);
	// Return number of replaced element or -1 if nothing has been replaced
	if (check_num == 0) return -1;
	return check_num;
}
int CBuild::fs::set_var(std::string file, std::string var_type,
			std::string var_name, std::string data) {
	// Path to tmp file
	std::string cache;
	cache = CBUILD_CACHE_DIR + "/" + CBUILD_COPY_CACHE_DIR + "/" +
		CBuild::fs::path_to_file(file) + ".tmp";
	// std::cout << cache << "\n";
	// Open two files
	CBuild::fs::create({cache}, CBuild::fs::FILE);
	std::ofstream tmp_file(cache);
	std::ifstream main_file;
	main_file.open(file);
	// Number of replaced tokens
	static int check_num;
	check_num = 0;
	// Read and copy file to temp file line by line
	std::string line;
	while (std::getline(main_file, line)) {
		// We can refer to #define by define
		if (var_type == "define") var_type = "#define";
		// If variable found
		if ((line.find(var_type) != std::string::npos) &&
		    (line.find(var_name) != std::string::npos)) {
			// All content before variable
			std::string prefix =
			    line.substr(0, line.find(var_type));
			// All content after variable
			std::string postfix =
			    line.substr(line.find(var_name) + var_name.size(),
					line.size() - 1);
			// Variable
			std::string var =
			    var_type + " " + var_name + " = " + data;
			// Recreate line
			line = prefix + var + postfix;
			check_num++;
		}
		tmp_file << line << "\n";
	}
	// Close files
	main_file.close();
	tmp_file.close();
	// Replace file by it's temp version
	CBuild::fs::remove(file, true);
	CBuild::fs::move(cache, file);
	// Return number of replaced element or -1 if nothing has been replaced
	if (check_num == 0) return -1;
	return check_num;
}
std::string CBuild::fs::path_to_file(std::string in) {
	while (in.find("/") != std::string::npos) {
		in.replace(in.find("/"), std::string("/").size(), ".");
	}
	return in;
}
