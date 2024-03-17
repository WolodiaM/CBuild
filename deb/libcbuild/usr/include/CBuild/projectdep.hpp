/**
 * @file projectdep.hpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief CBuild project dependency
 * @date 2024-03-16
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
#include "string"
#include "vector"
// Project includes
#include "build/Build.hpp"
#include "build_data.hpp"
#include "filesystem++.hpp"
// Code
#ifndef __PROJECTDEP_HPP__
#define __PROJECTDEP_HPP__
namespace CBuild {
template <class T>
concept ToolchainImpl = std::is_base_of<CBuild::Toolchain, T>::value;
template <ToolchainImpl compiler> class ProjectDependency {};
} // namespace CBuild
#endif // __PROJECTDEP_HPP__
