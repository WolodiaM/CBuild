/**
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Hasher v3.0 for CBuild
 * @date Fri May 19 11:21:22 AM EEST 2023
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
#ifndef __HASH_HPP__
#define __HASH_HPP__
// C++ libraries
#include "string"
#include "vector"
// Code
namespace CBuild {
/**
 * @brief Get changed files
 *
 * @param files => std::vector<std::string> -> List of code files (.cpp/.c)
 * @param objects => std::vector<std::string> -> List of object files (.o/.obj)
 * @param toolchain_id => std::string -> Id of toolchain, to get hash file
 */
std::vector<std::string> get_files(std::vector<std::string> files, std::vector<std::string> objects,
                                   std::string toolchain_id);
/**
 * @brief Print temporary file array
 */
void print_files();
} // namespace CBuild
#endif // __HASH_HPP__
