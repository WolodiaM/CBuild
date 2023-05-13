/**
 * @file print.cpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Custom print that support colors
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
#include "stdio.h"
// Project headers
#include "../../headers/print.hpp"
namespace CBuild {
bool verbose = false;
}  // namespace CBuild
/* print.hpp */
void CBuild::print(std::string msg, color fg) {
	// Create buffer
	std::string buff;
	// Load ANSI start code in buffer
	buff = "\u001b[";
	// Load color
	int f = fg;
	// Push collor to buffer
	buff += std::to_string(f);
	// Push ANSI end code to buffer
	buff += "m";
	// Push message to buffer
	buff += msg;
	// Push ANSI reset code to buffer
	buff += "\u001b[0m";
	// Print buffer with \n
	puts(buff.c_str());
	fflush(stdout);
}
void CBuild::print_full(std::string msg, color fg) {
	if (CBuild::verbose) CBuild::print(msg, fg);
}
void CBuild::print_verbose() { CBuild::verbose = true; }
