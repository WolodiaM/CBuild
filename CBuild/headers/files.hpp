/**
 * @file files.hpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief File parsing api
 * @version 1.0
 * @date 2023-01-23
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
#include "string"
#ifndef __CBUILD_FILE_HPP__
#define __CBUILD_FILE_HPP__
namespace CBuild {
namespace fs {
/**
 * @brief Replace [token] by [data] in [file]
 * @param file => std::string -> Path to file
 * @param token => std::string -> Token for replacing
 * @param data => std::string -> Data that replace token
 * @return int -> -1 = error, else - number of replaced tokens
 */
int replace(std::string file, std::string token, std::string data);
/**
 * @brief Set value of variable with type var_type wth name var_name to data
 * @param file => std::string -> Path to file for replacing
 * @param var_type => std::string -> Type of variable
 * @param var_name => std::string -> Name of variable
 * @param data => std::string -> New value of variable
 * @return int -> -1 - error, else - number of replaced tokens
 */
int set_var(std::string file, std::string var_type, std::string var_name,
	    std::string data);
/**
 * @brief Convert path to filename by replacing '\' by '.'
 *
 * @param in => std::string -> Input path
 * @return std::string -> Filename
 */
std::string path_to_file(std::string in);
}  // namespace fs
}  // namespace CBuild

#endif	// __CBUILD_FILE_HPP__
