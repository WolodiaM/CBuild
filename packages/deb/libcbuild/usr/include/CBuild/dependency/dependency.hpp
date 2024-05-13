/**
 * @file dependency.hpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Some external dependency. Base class
 * @date 2024-05-12
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
// Code
#ifndef __DEPENDENCY_HPP__
#define __DEPENDENCY_HPP__
namespace CBuild {
class Dependency {
  public:
    virtual bool need_prepare() = 0;
    virtual void prepare() = 0;
    virtual std::string cargs() = 0;
    virtual std::string largs() = 0;
};
} // namespace CBuild
#endif // __DEPENDENCY_HPP__
