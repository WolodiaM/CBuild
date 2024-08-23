/**
 * @file build_data.hpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @date 2023-12-06
 * @brief Build metadata config file
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
#ifndef __BUILD_DATA_HPP__
#define __BUILD_DATA_HPP__
// C++ libs
#include "cstdint"
#include "string"
#include "vector"
// Code
namespace CBuild {
/**
 * @brief Metadata for source files (.cpp/.c/etc)
 * Structure of file:
 * - line 1: Source file name
 * - line 2: Object file name or "header" string if this is a header
 * - line 3: Always "DEPS"
 * - line 4-n: Dependencies paths (relative to this file)
 * - line n+1: Always "ENDDEPS"
 * - line n+2: hash as decimal number
 */
struct source_metadata_file {
    /**
     * @brief Source file path (relative to CBuild.run)
     */
    std::string source;
    /**
     * @brief Object file name
     */
    std::string object;
    /**
     * @brief On what files this file depends
     */
    std::vector<std::string> deps;
    /**
     * @brief Hash of this file
     */
    uint64_t hash;
};
/**
 * @brief Metadata for a full target
 * Structure of file:
 * - line 1: Output binary name
 * - line 2: Compiler command used
 * - line 3: Linker command used
 * - line 4: Command used to pack static libraries
 * - line 5: Always "CARGS"
 * - line 5-n: Compile arguments
 * - line n+1: Always "ENDCARGS"
 * - line n+2: Always "LARGS"
 * - line n+3-m: Linker arguments
 * - line m+1: Always "ENDLARGS"
 */
struct target_metadata_file {
    /**
     * @brief Path to output binary (relative to CBuild.run)
     */
    std::string out;
    /**
     * @brief Command used to compile this target (what shell command was used)
     */
    std::string compiler;
    /**
     * @brief Command used to link this target (what shell command was used)
     */
    std::string linker;
    /**
     * @brief Command used to pack this target to a static library (what shell command was used)
     */
    std::string packer;
    /**
     * @brief Compile args
     */
    std::vector<std::string> cargs;
    /**
     * @brief Link args
     */
    std::vector<std::string> largs;
};
/**
 * @page auxilary-docs
 * @anchor file_err_codes
 * @par File error codes
 *  - 0  -> All good @n
 *  - -1 -> File not found (only for reads) @n
 *  - -2 -> Underlying file operation failed @n
 *  - -3 -> File is illformatted, cannot parse (only for reads) @n
 */
/**
 * @brief Load metadata for source file
 *
 * @param target_id => std::string -> Id of target that has this file
 * @param src_file => std::string -> File name
 * @param metadata => CBuild::source_metadata_file* -> Pointer to file metadata struct
 * @return int -> @ref file_err_codes "Error code"
 */
int read_file_metadata(std::string target_id, std::string src_file,
                       CBuild::source_metadata_file* metadata);
/**
 * @brief Load metadata for source file
 *
 * @param target_id => std::string -> Id of target that has this filelibsfml-dev
 * @param file => std::string -> Metadata file name
 * @param metadata => CBuild::source_metadata_file* -> Pointer to file metadata struct
 * @return int -> @ref file_err_codes "Error code"
 */
int read_file_metadata_direct(std::string target_id, std::string file,
                              CBuild::source_metadata_file* metadata);
/**
 * @brief Load metadata for a full target
 *
 * @param target_id => std::string ->Id of target
 * @param metadata => CBuild::target_metadata_file* -> Pointer to a target metadata struct
 * @return int -> @ref file_err_codes "Error code"
 */
int read_target_metadata(std::string target_id, CBuild::target_metadata_file* metadata);
/**
 * @brief Load metadata for a full target using path to metdata file
 *
 * @param path => std::string -> File path
 * @param metadata => CBuild::target_metadata_file* -> Metadata
 * @return int -> @ref file_err_codes "Error code"
 */
int read_target_metadata_direct(std::string path, CBuild::target_metadata_file* metadata);
/**
 * @brief Get a file hash (internally uses read_file_metadata, so no performance benefits, but
 * easiier to use in hasher)
 *
 * @param target_id => std::string -> Id of target that has this file
 * @param file => std::string -> File name
 * @param hash => uint64_t -> Hash value
 * @return int -> @ref file_err_codes "Error code"
 */
int read_file_hash(std::string target_id, std::string file, uint64_t* hash);
/**
 * @brief Write a metadata for a file to a file
 *
 * @param target_id => std::string -> Id of a target that has this file
 * @param src_file => std::string -> File name
 * @param metadata => CBuild::source_metadata_file* -> Pointer to a file metadata struct
 * @return int -> @ref file_err_codes "Error code"
 */
int write_file_metadata(std::string target_id, std::string src_file,
                        CBuild::source_metadata_file* metadata);
/**
 * @brief Write a metadata for a full target
 *
 * @param target_id => std::string -> Id of a target
 * @param metadata => CBuild::target_metadata_file* -> Pointer to a target metadata struct
 * @return int -> @ref file_err_codes "Error code"
 */
int write_target_metadata(std::string target_id, CBuild::target_metadata_file* metadata);
/**
 * @@brief Write a file hash (internally uses write_file_metadata, so no performance benefits, but
 * easiier to use in hasher)
 *
 * @param target_id => std::string -> Id of target that has this file
 * @param file => std::string -> File name
 * @param hash => uint64_t -> Pointer to a hash value
 * @return int -> @ref file_err_codes "Error code"
 */
int write_file_hash(std::string target_id, std::string file, uint64_t* hash);
/**
 * @brief Get the path to a file metadata file
 *
 * @param target_id => std::string -> Id of target that has this file
 * @param file => std::string -> File name
 * @return std::string -> Path to a metadata file
 */
std::string get_file_metadata_path(std::string target_id, std::string file);
/**
 * @brief Get the path to a target metadata file
 *
 * @param target_id => std::string -> Id of a target
 * @return std::string -> Path to a metadata file
 */
std::string get_target_metadata_path(std::string target_id);
} // namespace CBuild
#endif // __BUILD_DATA_HPP__
