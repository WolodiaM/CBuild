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
#include "common.h"
#include "DynArray.h"
// Code
cbuild_da_t(char*, CBuildFlagArgList);
typedef cbuild_da_CBuildFlagArgList_t cbuild_flag_arglist_t;
cbuild_da_t_ext_impl(CBuildFlagArgList);
/**
 * @brief Parse flag spec and register it
 * [See here for a full spec](../internals/flagparse)
 *
 * @param spec => char* -> Flag spec
 */
void                   cbuild_flag_new(const char* spec);
/**
 * @brief Parse flags
 *
 * @param argc => int -> Argument count
 * @param argv => char** -> Arguments, argv[0] is a binary name
 */
void                   cbuild_flag_parse(int argc, char** argv);
/**
 * @brief Prints help for all flags to a stdout
 */
void                   cbuild_flag_flg_help();
/**
 * @brief Get positional arguments list
 *
 * @return cbuild_flag_arglist_t* -> List of positional arguments
 */
cbuild_flag_arglist_t* cbuild_flag_get_pargs(void);
/**
 * @brief Get positional arguments list
 *
 * @param opt => char* -> Flag long version
 * @return cbuild_flag_arglist_t* -> List of flag's arguments
 */
cbuild_flag_arglist_t* cbuild_flag_get_flag(const char* opt);
/**
 * @brief Get app name as a C-string
 *
 * @return char* -> App name
 */
char*                  cbuild_flag_app_name(void);
/**
 * @brief Function prototype fo a help print function, stub provided, but should
 * be reimplemented for best user experience.
 *
 * @param name => char* -> App name
 */
void                   cbuild_flag_help(const char* name);
/**
 * @brief Function prototype fo a version print function, stub provided, but
 * should be reimplemented for best user experience.
 *
 * @param name => char* -> App name
 */
void                   cbuild_flag_version(const char* name);
#endif // __CBUILD_FLAGPARSE_H__
