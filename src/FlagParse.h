/**
 * @file FlagParse.h
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Simple CLI flag parses
 * Parses logn and short option flags. Formats error messages. Support different
 * argument arrangemet for options. Suppoert non-flag options.
 *
 * @date 2025-06-25
 * @copyright (C) 2025 WolodiaM
 * @license MIT
 *
 * Copyright (C) 2025 WolodiaM
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
#ifndef __CBUILD_FLAGPARSE_H__
#define __CBUILD_FLAGPARSE_H__
// Project includes
#include "DynArray.h"
#include "common.h"
// Code
// data is a pointer borrowed into a argv
typedef struct cbuild_arglist_t {
	const char** data;
	size_t size;
	size_t capacity;
} cbuild_arglist_t;
/**
 * @brief Parse flag spec and register it
 * [See here for a full spec](../4.internals/2.flagparse.html)
 *
 * @param spec => char* -> Flag spec
 */
void cbuild_flag_new(const char* spec);
/**
 * @brief Parse flags
 *
 * @param argc => int -> Argument count
 * @param argv => char** -> Arguments, argv[0] is a binary name
 */
void cbuild_flag_parse(int argc, char** argv);
/**
 * @brief Prints help for all flags to a stdout
 */
void cbuild_flag_print_help();
/**
 * @brief Get positional arguments list
 *
 * @return cbuild_flag_arglist_t* -> List of positional arguments
 */
cbuild_arglist_t* cbuild_flag_get_pargs(void);
/**
 * @brief Get arguments of a specific flag
 *
 * @param opt => char* -> Flag long version
 * @return cbuild_flag_arglist_t* -> List of flag's arguments or NULL if flag
 * not found
 */
cbuild_arglist_t* cbuild_flag_get_flag(const char* opt);
/**
 * @brief Get app name as a C-string
 *
 * @return char* -> App name
 */
char* cbuild_flag_app_name(void);
/**
 * @brief Function prototype fo a help print function, stub provided, but should
 * be reimplemented for best user experience.
 */
extern void (*cbuild_flag_help)(const char* app_name);
/**
 * @brief Function prototype fo a version print function, stub provided, but
 * should be reimplemented for best user experience.
 */
extern void (*cbuild_flag_version)(const char* app_name);
#endif // __CBUILD_FLAGPARSE_H__
