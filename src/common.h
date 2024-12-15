/**
 * @file common.h
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Common CBuild code
 * Misceleanous code, defines to implement API abstraction, OS/API select
 * defines
 *
 * @date 2024-12-04
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
// Code
#ifndef __CBUILD_COMMON_H__
#define __CBUILD_COMMON_H__
// Includes
#include "assert.h"
#include "dirent.h"
#include "errno.h"
#include "fcntl.h"
#include "limits.h"
#include "stdarg.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/stat.h"
#include "sys/types.h"
#include "sys/wait.h"
#include "unistd.h"
// Constants that can be redefined
#ifndef CBUILD_DA_INIT_CAPACITY
#	define CBUILD_DA_INIT_CAPACITY 256ul
#endif // CBUILD_DA_INIT_CAPACITY
#ifndef CBUILD_LOG_MIN_LEVEL
#	define CBUILD_LOG_MIN_LEVEL CBUILD_LOG_ERROR
#endif // CBUILD_LOG_MIN_LEVEL
#ifndef CBUILD_TMP_BUFF_SIZE
#	define CBUILD_TMP_BUFF_SIZE (32 * 1024 * 1024)
#endif // CBUILD_TMP_BUFF_SIZE
// OS-specific defines
#if defined(__linux__)
#	define CBUILD_OS_LINUX
#	define CBUILD_API_POSIX
#elif defined(__MINGW32__)
#	define CBUILD_OS_WINDOWS_MINGW
#	define CBUILD_API_POSIX
#elif defined(__CYGWIN__)
#	define CBUILD_OS_WINDOWS_CYGWIN
#	define CBUILD_API_POSIX
#elif defined(__APPLE__) || defined(__MACH__)
#	define CBUILD_OS_MACOS
#	define CBUILD_API_POSIX
#elif defined(__unix__)
#	define CBUILD_OS_UNIX
#	define CBUILD_API_POSIX
#elif defined(_MSC_VER)
#	error "This library support only POSIX api and MSVC only supports WinAPI"
#else
#	error                                                                        \
			"This OS is unsupported by CBuild. If it supports POSIX API then you can add new compile-time check for your current OS and define API macro and OS macro and add compiler macro check for your OS. If it don't support any of this APIs then you need to create your own API macro and change implementation-specifc parts of CBuild"
#endif // OS selector
// Print functions
#define __CBUILD_PRINT(str)								 printf((str))
#define __CBUILD_PRINTF(fmt, ...)					 printf((fmt), __VA_ARGS__)
#define __CBUILD_VPRINTF(fmt, va_args)		 vprintf((fmt), (va_args))
// Maybe you want to redefine this two macro to work with stderr, but I prefer
// to have my errors in standard stdout
#define __CBUILD_ERR_PRINT(str)						 printf((str))
#define __CBUILD_ERR_PRINTF(fmt, ...)			 printf((str), __VA_ARGS__)
#define __CBUILD_ERR_VPRINTF(fmt, va_args) vprintf((fmt), (va_args))
// Macro functionality
#define __CBUILD_STRINGIFY(var)						 #var
#define __CBUILD_CONCAT(a, b)							 a##b
// Process and file handles
typedef __pid_t CBuildProc;
#define CBUILD_INVALID_PROC -1
typedef int CBuildFD;
#define CBUILD_INVALID_FD	 -1
// Command and process functions
// Some preprocessor trickery
/**
 * @brief Mark variable as usused
 *
 * @param val => any -> Any variable
 */
#define CBuild_UNUSED(val) (void)(val)
/**
 * @brief Mark some part of code as TODO, code will error-out if this code is
 * executed
 *
 * @param message => char* -> Additional message
 */
#define CBuild_TODO(message)                                                   \
	do {                                                                         \
		("%s:%d: TODO: %s\n", __FILE__, __LINE__, (message));                      \
		abort();                                                                   \
	} while (0)
/**
 * @brief Mark some part of code as UNREACHABLE, code will error-out if this
 * code is executed
 *
 * @param message => char* -> Additional message
 */
#define CBuild_UNREACHABLE(message)                                            \
	do {                                                                         \
		__CBUILD_ERR_PRINTF(stderr, "%s:%d: UNREACHABLE: %s\n", __FILE__,          \
												__LINE__, (message));                                  \
		abort();                                                                   \
	} while (0)
// More user-friendly array operations
/**
 * @brief get size of array
 *
 * @param => any[] -> Array
 */
#define cbuild_arr_len(array) (sizeof(array) / sizeof((array)[0]))
/**
 * @brief Get element from array, errors-out at invalid index
 *
 * @param array => any[] -> Array
 * @param index => size_t -> Array index
 */
#define cbuild_arr_get(array, index)                                           \
	(assert((size_t)(index) < cbuild_arr_len(array)), (array)[(size_t)(index)])
/**
 * @brief Shift args in and array with size (like argv and argc pair. Should
 * work similartly to bash `shift`
 *
 * @param argv => T[] -> Array
 * @paran argc => Integer -> Array size
 * */
#define cbuild_shift(argv, argc)                                               \
	*(argv);                                                                     \
	assert((argc) > 0);                                                          \
	(argc)--;                                                                    \
	(argv)++;
// Version
#define CBUILD_VERSION "v1.0"
#define VERSION_MINOR	 0
#define VERSION_MAJOR	 1
#endif // __CBUILD_COMMON_H__
