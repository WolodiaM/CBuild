/**
 * @file environment.hpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Set environment variables
 * @date 2023-10-30
 *
 * @copyright (C) 2023 WolodiaM
 * @license GPL v3.0 or later
 *
 * Copyright (C) 2023 WolodiaM
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
#ifndef __CBUILD_ENVIRONMENT_HPP__
#define __CBUILD_ENVIRONMENT_HPP__
// C++ libraries
#include "string"
// Code
namespace CBuild {
/**
 * @brief Add environment variable to CBuild::system and CBuild::system_piped
 * calls
 *
 * @param var => std::string -> Var name
 * @param value => std::string -> Var value
 */
void add_env_var(std::string var, std::string value);
/**
 * @brief Remove registered environment var
 *
 * @param var => std::string -> Var name
 * @return std::string -> Var value
 */
std::string remove_env_var(std::string var);
/**
 * @brief Change value for environment var
 *
 * @param var => std::string -> Var name
 * @param new_value => std::string -> New var value
 */
void change_env_var(std::string var, std::string new_value);
} // namespace CBuild
#endif // __CBUILD_ENVIRONMENT_HPP__
