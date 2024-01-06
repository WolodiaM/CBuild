/**
 * @file generator.hpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Base class for generator of some output from command sequence
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
#ifndef __CBUILD_GENERATOR_HPP__
#define __CBUILD_GENERATOR_HPP__
// C++ libraries
#include "string"
// Project includes
#include "../CBuild.hpp"
#include "../map.hpp"
#include "../system.hpp"
// Code
namespace CBuild {
/**
 * @class generator_base
 * @brief Base for any generator
 */
class generator_base {
  protected:
    std::string OUT;

  public:
    /**
     * @brief Create and save output file name
     *
     * @param out => std::string -> Output file
     */
    generator_base(std::string out) {
        this->OUT = out;
    }
    /**
     * @brief Generate some output
     *
     * @param mode => CBuild::RType -> CBuild arg
     * @param args => lib::map<std::string, std::string> -> CBuild arg
     */
    virtual void generate(CBuild::RType mode, lib::map<std::string, std::string>* args) = 0;
    /**
     * @brief Some init
     *
     * @return bool -> Work as logger/capture execution thread (true/false)
     */
    virtual bool init() = 0;
};
} // namespace CBuild
#endif // __CBUILD_GENERATOR_HPP__
