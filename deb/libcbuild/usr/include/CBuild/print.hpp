/**
 * @file print.hpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Custom print that support color codes
 * @version 1.0
 * @date 2023-02-14
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
#ifndef __CBUILD_PRINT_HPP__
#define __CBUILD_PRINT_HPP__
namespace CBuild {
/**
 * @brief Text color
 */
typedef enum {
	BLACK = 30,
	RED = 31,
	GREEN = 32,
	YELLOW = 33,
	BLUE = 34,
	MAGENTA = 35,
	CYAN = 36,
	WHITE = 37
} color;
/**
 * @brief Print colored text to STDOUT
 *
 * @param msg => std::string -> text to print
 * @param fg => CBuild::color -> color of text
 */
void print(std::string msg, color fg = CBuild::WHITE);
/**
 * @brief Print colored text to STDOUT if verbose flag is set
 *
 * @param msg => std::string -> text to print
 * @param fg => CBuild::color -> color of text
 */
void print_full(std::string msg, color fg = CBuild::WHITE);
/**
 * @brief Enable verbose flag
 */
void print_verbose();
}  // namespace CBuild
#endif	// __CBUILD_PRINT_HPP__
