/**
 * @file system.cpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief custom system() wrapper
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
#include "stdlib.h"
#include "string"
#include "vector"
// Project headers
#include "../../headers/print.hpp"
#include "../../headers/system.hpp"
/* namespace CBuild */
namespace CBuild {
std::vector<std::string> log;
}  // namespace CBuild
/* system.hpp */
void CBuild::system(std::string cmd) {
	CBuild::log.push_back(cmd);
	CBuild::print(cmd, CBuild::color::BLUE);
	std::system(cmd.c_str());
}
std::vector<std::string>* CBuild::get_log() { return &CBuild::log; }
