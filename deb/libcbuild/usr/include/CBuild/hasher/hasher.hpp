/**
 * @file hasher.hpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @date 2023-12-04
 * @brief Hasher base class and some helper functions
 * This file implements some helper funtions, eg. storing and retrieving files from database and
 * also provides base class for hasher. Hasher need to implement dependency scanning and hashing
 * function
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
#ifndef __HASHER_HPP__
#define __HASHER_HPP__
// C++ libs
#include "string"
#include "vector"
// Project headers
#include "../CBuild_defs.hpp"
#include "../filesystem++.hpp"
#include "../map.hpp"
#include "stdint.h"
// Code
namespace CBuild {
/**
 * @brief djb2 hashing function for std::string
 *
 * @param str => std::string -> Input string
 * @return uint64_t -> Hash of data
 */
inline uint64_t hash_djb2(std::string str) {
    uint64_t hash = 5381;
    for (auto c : str) {
        hash = ((hash << 5) + hash) + (uint64_t)c;
    }
    return hash;
}
/**
 * @brief FNV-1a hashing function for std::string
 *
 * @param str => std::string -> Input string
 * @return uint64_t -> Hash of data
 */
inline uint64_t hash_fnv_1a(std::string str) {
    uint64_t hash = 14695981039346656037ULL;
    for (char c : str) {
        hash ^= ((uint64_t)(c));
        hash *= 1099511628211ULL;
    }
    return hash;
}
class Hash {
  protected:
    std::string target_id;

  public:
    Hash(std::string target_id) {
        this->target_id = target_id;
    }
    /**
     * @brief Pass a list of source files and coresponding object files and get a map of
     * source-object that need to be recompiled (check only hash and dependencie hash, not any arg
     * canges)
     *
     * @param file_list => std::vector<std::string> -> List of source files
     * @param objects_list => std::vector<std::string> -> List of object files
     * @return lib::map<std::string, std::string> -> Map of source->object, that need to be
     * recompiled
     */
    virtual lib::map<std::string, std::string>
    get_files_for_recompilation(std::vector<std::string> file_list,
                                std::vector<std::string> objects_list) = 0;
    /**
     * @brief Compare compile args and update metadata
     *
     * @param cargs => std::vector<std::string> -> Compile args
     * @return int -> 0 - all good, 1 - args didn`t match, -1 - File didn`t exist at all
     */
    virtual int compare_and_set_cargs(std::vector<std::string> cargs) = 0;
    /**
     * @brief Compare link args and update metadata
     *
     * @param cargs => std::vector<std::string> -> Compile args
     * @return int -> 0 - all good, 1 - args didn`t match, -1 - File didn`t exist at all
     */
    virtual int compare_and_set_largs(std::vector<std::string> largs) = 0;
    /**
     * @brief Compare shell commands used for compilation and update metadata
     *
     * @param compiler => std::string -> Command used for compiling source files
     * @param linker => std::string -> Command used for linking objects
     * @param packer => std::string -> Command used for packing static libs
     * @return int -> 0 - all good, 1 - args didn`t match, -1 - File didn`t exist at all
     */
    virtual int compare_and_set_commands(std::string compiler, std::string linker,
                                         std::string packer) = 0;
    /**
     * @brief Compare output file name and update metadata
     *
     * @param out => std::string -> Output name
     * @return int -> 0 - all good, 1 - args didn`t match, -1 - File didn`t exist at all
     */
    virtual int compare_and_set_output_file(std::string out) = 0;
    /**
     * @brief Set the target meta
     *
     * @param cargs => std::vector<std::string> -> Compile args
     * @param largs => std::vector<std::string> -> Link args
     * @param out => std::string -> Output binary name
     * @param compiler => std::string -> Command used for compiling source files
     * @param linker => std::string -> Command used for linking objects
     * @param packer => std::string -> Command used for packing static libs
     */
    virtual void set_target_meta(std::vector<std::string> cargs, std::vector<std::string> largs,
                                 std::string out, std::string compiler, std::string linker,
                                 std::string packer) = 0;
};
} // namespace CBuild
#endif // __HASHER_HPP__
