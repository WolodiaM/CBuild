/**
 * @file system.hpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Custom system() wraper
 * @version 1.0
 * @date 2023-03-11
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
#include "string"
#include "vector"
// Code
#ifndef __CBUILD_SYSTEM_HPP__
#define __CBUILD_SYSTEM_HPP__
namespace CBuild {
/**
 * @brief Call stdlib system() and print cmd to shell
 *
 * @param cmd => std::string -> command
 */
void system(std::string cmd);
/**
 * @brief Execute command and return it's output
 *
 * @param cmd => stt:;string -> command
 * @param buffsize => unsigned int -> Size of output buffer, malloc and fread
 */
std::string system_piped(std::string cmd, unsigned int buffsize);
/**
 * @brief Get the internall command log
 *
 * @return std::vector<std::string>* -> pointer to command log
 */
std::vector<std::string>* get_log();
}  // namespace CBuild
#endif	// __CBUILD_SYSTEM_HPP__
