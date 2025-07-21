/**
 * @file FS.h
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Filesystem operations
 * Simple wrapper around POSIX API to perfrm some higher-level system operations
 *
 * @date 2024-12-07
 * @copyright (C) 2024 WolodiaM
 * @license MIT
 *
 * Copyright (C) 2024 WolodiaM
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef __CBUILD_FS_H__
#define __CBUILD_FS_H__
// Project includes
#include "StringBuilder.h"
#include "common.h"
// Code
typedef struct cbuild_pathlist_t {
	char** data;
	size_t size;
	size_t capacity;
} cbuild_pathlist_t;
/**
 * @enum cbuild_filetype_t
 * @brief Type of filesystem element
 */
typedef enum {
	CBUILD_FTYPE_REGULAR   = 0,
	CBUILD_FTYPE_DIRECTORY = 1,
	CBUILD_FTYPE_SYMLINK   = 2,
	CBUILD_FTYPE_OTHER     = 3,
} cbuild_filetype_t;
/**
 *  @brief Close file dscripto
 *
 *  @param fd => cbuild_fd_t -> File descriptor that should be close
 *  @return bool -> Success of failur
 */
bool cbuild_fd_close(cbuild_fd_t fd);
/**
 * @brief Open file descriptor for reading using file path
 *
 * @param path => const char* -> Path to file
 * @return cbuild_fd_t -> New file descriptor
 */
cbuild_fd_t cbuild_fd_open_read(const char* path);
/**
 * @brief Open file descriptor for writing using file path
 *
 * @param path => const char* -> Path to file
 * @return cbuild_fd_t -> New file descriptor
 */
cbuild_fd_t cbuild_fd_open_write(const char* path);
/**
 * @brief Create and open pipe
 *
 * @param read => cbuild_fd_t* -> Read file descriptor return
 * @param write => cbuild_fd_t* -> Write file descriptor return
 * @return bool -> Success or failure
 */
bool cbuild_fd_open_pipe(cbuild_fd_t* read, cbuild_fd_t* write);
/**
 * @brief Read full file into buffer
 *
 * @param path => const char* -> File path
 * @param data => cbuild_sb_t* -> String buffer
 * @return bool -> Success or failure
 */
bool cbuild_file_read(const char* path, cbuild_sb_t* data);
/**
 * @brief Write full file (overwrite)
 *
 * @param path => const char* -> Filepath
 * @param data => cbuild_sb_t * -> Buffer to write
 * @return bool -> Success or failure
 */
bool cbuild_file_write(const char* path, cbuild_sb_t* data);
/**
 * @brief Copy file
 *
 * @param src => const char* -> Source filepath
 * @param dst => const char* -> Destination filepath
 * @return bool -> Success or failure
 */
bool cbuild_file_copy(const char* src, const char* dst);
/**
 * @brief Move file
 *
 * @param src => const char* -> Source filepath
 * @param dst => const char* -> Destination filepath
 * @return bool -> Success or failure
 */
bool cbuild_file_move(const char* src, const char* dst);
/**
 * @brief Rename file
 *
 * @param src => const char* -> Source filepath
 * @param dst => const char* -> Destination filepath
 * @return bool -> Success or failure
 */
bool cbuild_file_rename(const char* src, const char* dst);
/**
 * @brief Check if file exist
 *
 * @param path => const char* -> Filepath
 * @return bool -> Result
 */
bool cbuild_file_check(const char* path);
/**
 * @brief Remove file
 *
 * @param path => const char* -> Filepath
 * @return bool -> Success or failure
 */
bool cbuild_file_remove(const char* path);
/**
 * @brief Copy directory
 *
 * @param src => const char* -> Source filepath
 * @param dst => const char* -> Destination filepath
 * @return bool -> Success or failure
 */
bool cbuild_dir_copy(const char* src, const char* dst);
/**
 * @brief Move directory
 *
 * @param src => const char* -> Source filepath
 * @param dst => const char* -> Destination filepath
 * @return bool -> Success or failure
 */
bool cbuild_dir_move(const char* src, const char* dst);
/**
 * @brief Rename directory
 *
 * @param src => const char* -> Source filepath
 * @param dst => const char* -> Destination filepath
 * @return bool -> Success or failure
 */
bool cbuild_dir_rename(const char* src, const char* dst);
/**
 * @brief Remove directory
 *
 * @param path => const char* -> Filepath
 * @return bool -> Success or failure
 */
bool cbuild_dir_remove(const char* path);
/**
 * @brief Check if directory exist
 *
 * @param path => const char* -> Filepath
 * @return bool -> Success or failure
 */
bool cbuild_dir_check(const char* path);
/**
 * @brief Get list of file in a directory
 *
 * @param path => const char* -> Filepath
 * @param elements => cbuild_pathlist_t* -> Return valud
 */
bool cbuild_dir_list(const char* path, cbuild_pathlist_t* elements);
/**
 * @brief Free path list structure
 *
 * @param list => cbuild_pathlist_t* -> Path list
 */
void cbuild_pathlist_clear(cbuild_pathlist_t* list);
/**
 * @brief Create directory
 *
 * @param path => const char* -> Filepath
 * @return bool -> Success or failure
 */
bool cbuild_dir_create(const char* path);
/**
 * @brief Get type of filesystem entry
 *
 * @param path => const char* -> Path
 * @return CBuildFiletype -> Type of filesystem element
 */
cbuild_filetype_t cbuild_path_filetype(const char* path);
/**
 * @brief Get file extension (after last '.')
 *
 * @param path => const char* -> Filepath
 * @return const char* -> String allocated on heap
 */
const char* cbuild_path_ext(const char* path);
/**
 * @brief Get file name (after last '/' and before last '.' or if path ends with
 * '/' then after previous '/' to last '/')
 *
 * @param path => const char* -> Filepath
 * @return const char* -> String allocated on heap
 */
const char* cbuild_path_name(const char* path);
/**
 * @brief Everything that function cbuild_path_name removes
 *
 * @param path => const char* -> Filepath
 * @return const char* -> String allocated on heap
 */
const char* cbuild_path_base(const char* path);
#endif // __CBUILD_FS_H__
