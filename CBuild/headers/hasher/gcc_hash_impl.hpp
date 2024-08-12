/**
 * @file gcc_hash_impl.hpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief GCC hasher low-level implementation
 * @date 2024-01-08
 *
 * @copyright (C) 2024 WolodiaM
 * @license GPL v3.0 or later
 *
 * Copyright (C) 2024 WolodiaM
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
#include "../map.hpp"
#include "string"
#include "vector"
// Code
#ifndef __GCC_HASH_IMPL_HPP__
#define __GCC_HASH_IMPL_HPP__
namespace CBuild {
/**
 * @brief Implementation of GCC hasher
 *
 * @param file_list => std::vector<std::string> -> List of source files
 * @param objects_list => std::vector<std::string> -> List of coresponding object files
 * @param target_id => std::string -> ID of target that called hasher
 */
lib::map<std::string, std::string> gcc_hash_impl(std::vector<std::string> file_list,
                                                 std::vector<std::string> objects_list,
                                                 std::string target_id);

} // namespace CBuild
#endif // __GCC_HASH_IMPL_HPP__
