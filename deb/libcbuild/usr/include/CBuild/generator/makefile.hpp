/**
 * @file makefile.hpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Makefile generator
 * @date 2023-07-24
 *
 * @copyright (C) 2023 WolodiaM
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
#ifndef __CBUILD_MAKEFILE_HPP__
#define __CBUILD_MAKEFILE_HPP__
// C++ libraries
#include "string"
// Project includes
#include "../CBuild_defs.hpp"
#include "../map.hpp"
#include "generator.hpp"
// Code
namespace CBuild {
/**
 * @class makefile_out
 * @brief Generate Makefile from task/target, works as command logger
 *
 */
class makefile_out : public CBuild::generator_base {
  public:
    /**
     * @brief Create new Makefile generator
     */
    makefile_out() : CBuild::generator_base(CBUILD_MAKEFILE) {}
    /**
     * @brief Generate some output
     *
     * @param mode => CBuild::RType -> CBuild arg
     * @param args => lib::map<std::string, std::string> -> CBuild arg
     */
    virtual void generate(CBuild::RType mode, lib::map<std::string, std::string>* args);
    /**
     * @brief Some init
     *
     * @return bool -> Work as logger/capture execution thread (true/false)
     */
    virtual bool init();
};
} // namespace CBuild
#endif // __CBUILD_MAKEFILE_HPP__
