/**
 * @file pkgconfig.hpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @date 2023-12-01
 * @brief pkg-config interface header
 * Provides interface to pkg-config cli tool, including some parts of error handling
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
// C++ libs
#include "string"
// Code
#ifndef __PKGCONFIG_HPP__
#define __PKGCONFIG_HPP__
namespace CBuild {
/**
 * @brief Package info struct for data for pkg-config
 */
typedef struct package_info {
    /**
     * @brief Package name
     */
    std::string name;
    /**
     * @brief Package link args
     */
    std::string largs;
    /**
     * @brief Package compile args
     */
    std::string cargs;
} package_info;
/**
 * @brief Get the package info from pkg-config database
 *
 * @param package => package_info* -> Input and output from function
 * @return true -> All good
 * @return false -> Eroor, package not found
 */
bool get_pkg_info(CBuild::package_info* package);
} // namespace CBuild
#endif // __PKGCONFIG_HPP__