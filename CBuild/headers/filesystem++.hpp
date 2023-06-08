/**
 * @file filesystem++.hpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief filesystem++ api
 * @version 1.0
 * @date 2023-01-19
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
#ifndef __CBUILD_FILESYSTEM_HPP__
#define __CBUILD_FILESYSTEM_HPP__
namespace CBuild {
namespace fs {
/**
 * @brief Type for element creating
 */
typedef enum {
  /**
   * @brief Standard file
   */
  FILE,
  /**
   * @brief Standard directory
   */
  DIRECTORY,
  /**
   * @brief Alias for DIRECTORY
   */
  DIR,
  /**
   * @brief Standard symlink (can be unsupported or supported partially)
   * (thought std::filesystem::create_symlink)
   */
  SYMLINK_FILE,
  /**
   * @brief Standard symlink (can be unsupported or supported partially)
   * (std::filesystem::create_directory_symlink)
   */
  SYMLINK_DIRECTORY,
  /**
   * @brief Alias for SYMLINK_DIRECTORY
   */
  SYMLINK_DIR,
  /**
   * @brief Standard hardlink (can be unsupported or supported partially)
   * (though std::filesystem::create_hard_link)
   */
  HARDLINK
} type;
/**
 * @brief Search files using provided regex
 *
 * @param path => std::string -> Starting path for search
 * @param search => std::string -> Search regex
 * @return std::vector<std::string> -> list of found files
 */
std::vector<std::string> dir(std::string path, std::string search);
/**
 * @brief Search files using standard regex (".*\.(cpp|cxx|cc|c)")
 *
 * @param path => std::string -> Starting path for search
 * @return std::vector<std::string> -> list of found files
 */
std::vector<std::string> dir(std::string path);
/**
 * @brief Recursively search files using provided regex
 *
 * @param path => std::string -> Starting path for search
 * @param search => std::string -> Search regex
 * @return std::vector<std::string> -> list of found files
 */
std::vector<std::string> dir_rec(std::string path, std::string search);
/**
 * @brief Recursively search files using standard regex (".*\.(cpp|cxx|cc|c)")
 *
 * @param path => std::string -> Starting path for search
 * @return std::vector<std::string> -> list of found files
 */
std::vector<std::string> dir_rec(std::string path);
/**
 * @brief Delete files or directories
 *
 * @param path => std::string -> path to file or directory
 * @param force => remove_all in place of remove
 * @return bool -> success or failure
 */
bool remove(std::string path, bool force = false);
/**
 * @brief Copy files or directories
 *
 * @param start => std::string -> path to file or directory
 * @param end => std::string -> path to file or directory after copy
 * @return bool -> success or failure
 */
bool copy(std::string start, std::string end);
/**
 * @brief Move files or directories, remove starting files or directories
 *
 * @param start => std::string -> path to file or directory
 * @param end => std::string -> path to file or directory after move
 * @return bool -> success or failure
 */
bool move(std::string start, std::string end);
/**
 * @brief Rename files or directories,
 *
 * @throws std::runtime_error with error message as string
 * @param start => std::string -> path to file or directory
 * @param end => std::string -> new name (without path)
 * @return bool -> success or failure
 */
bool rename(std::string start, std::string end);
/**
 * @brief Create element
 * @param paths => std::vector<std::string> -> Path to new element, ans some
 * other arguments (for symlinks)
 * @param what => CBuild::fs::type -> What create (custom enum type)
 * @return bool -> success or failure
 */
bool create(std::vector<std::string> paths, CBuild::fs::type what);
/**
 * @brief Check if file exists
 *
 * @param path => std::string -> path
 * @return bool -> does object exist
 */
bool exists(std::string path);
/**
 * @brief Get absolute path to file using relative path and base path
 *
 * @param path => std::string -> Relative path to file
 * @param base_path => std::string -> Base path of this reative path, "" to none
 * @return std::string -> Absolute path
 */
std::string normalize_path(std::string path, std::string base_path = "");
/**
 * @brief Get base file path (path dir in what file is)
 *
 * @param file => std::string -> Filepath
 */
std::string base(std::string file);
} // namespace fs
} // namespace CBuild
#endif // __CBUILD_FILESYSTEM_HPP__
