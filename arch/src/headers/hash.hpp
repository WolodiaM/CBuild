/**
 * @file hash.hpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Hashing files
 * @version 1.0
 * @date 2023-02-04
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
#include "string"
#include "vector"
// Code
#ifndef __CBUILD_HASH_HPP__
#define __CBUILD_HASH_HPP__
namespace CBuild
{
    /**
     * @brief Check if saved hash match new hash of file and update saved hash
     * 
     * @param hash_file => std::string -> path to hash file
     * @param code_file => std::string -> path to code file
     * @return true -> file hash equal to saved
     * @return false -> file hash different from saved
     */
    bool hash_match_store(std::string hash_file, std::string code_file);
    /**
     * @brief Load hash
     * 
     * @param hash_files => std::string -> hash files
     * @param code_files => std::string -> code files
     * @return true -> file hash equal to saved
     * @return false -> file hash different from saved
     */
    bool load_hash(std::string hash, std::string code);
    /**
     * @brief Store hashes
     * 
     * @param hash_files => std::vector<std::string> -> hash files
     * @param code_files => std::vector<std::string> -> code files
     */
    void save_hashes(std::vector<std::string> hash_files, std::vector<std::string> code_files);
} // namespace CBuild
#endif // __CBUILD_HASH_HPP__