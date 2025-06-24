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
/* CHANGELOG
 * --------------------------------------------
 * 15.12.2024  v1.0    Initial release
 *   Initial release [new]
 * --------------------------------------------
 * 21.12.2024  v1.1    Few small additions
 *   Proc.h [feature]
 *     - Added 'cbuild_proc_start' function
 * --------------------------------------------
 * 27.12.2024  v1.2    Bugfix release
 *   Compile.h [bugfix]
 *     - Fixed type ('CBUILD_CARG_WARN' -> 'CBUILD_CARGS_WARN')
 *	 Log.h [bugfix]
 *     - Logger always use one output stream for its messages now
 *   General [bugfix]
 *     - Added 'CBUILD_IMPL' macro to allow multiple includes in same
 *       translation unit
 * --------------------------------------------
 * 10.01.2025  v1.3    New API features and bugfixes
 *   common.h [bugfix]
 *     - Fixed bug in macro (wrong variable name used)
 *     - Switched 'CBuildFD' typedef from '__pid_t' to 'pid_t'
 *   common.h	[feature]
 *     - Added 'cbuild_shift_expect' (allows to have error message on top of
 *       'cbuild_shift')
 *     - Added 'cbuild_assert'
 *     - Added 'noreturn' attribute to an assert function.
 *   Command.h [bugfix]
 *     - Added 'fd' prefix to 'stdin', 'stdout' and 'stderr' for compatibility
 *       with libc that defines this names as a macro
 *     - Fixed typo in error message
 *	 Log.h [feature]
 *     - Logger now use 16-color ANSI mode
 *     - Added 'cbuild_vlog' that takes 'va_list' inserted of variadic arguments
 *	 General [bugfix]
 *     - Added few 'const' annotations to pointers
 *     - Changed all asserts to 'cbuild_assert'
 * --------------------------------------------
 * 18.02.2025  v1.4    Small bugfix
 *   common.h [bugfix]
 *     - Wrong argument passed to '__CBUILD_ERR_PRINTF' in 'CBuild_UNREACHAGLE'
 * --------------------------------------------
 * 18.02.2025  v1.5    Bugfix release
 *   impl.c [bugfix]
 *     - Fixed buffer overflow in 'cbuild_dir_copy'
 *     - Improved 'cbuild_shift'
 *   General [change]
 *     - Changed macro 'CBUILD_IMPL' to 'CBUILD_IMPLEMENTATION'
 * --------------------------------------------
 * 16.06.2025  v1.6    Full rewrite
 *   Term.h [new]
 *     - ANSI wrapper module
 *   Log.h [feature]
 *     - Rely on 'Term.h' for colored output
 *     - Runtime configuration for minimum log level
 *     - Logger now support customizable formatting callbacks
 *   DynArray.h [new]
 *     - Different implementation
 *	 common.h [feature]
 *     - Fixes for MacOS related to '__progname'
 *     - Better OS and API abstraction
 *	 StringBuilder.h [new]
 *     - Rely on new 'DynArray.h'
 *     - Few new operations
 *   Proc.h [feature]
 *     - Better naming scheme
 *     - Added ability to allocate inter-process shared memory
 *   Command.h [feature]
 *     - More consistent names
 *   Map.h [new]
 *     - Hash-map implementation
 *   Compile.h [change]
 *     - Added consistent prefixes
 *     - Added output arg	macro
 *   FS.h [change]
 *     - Changes becouse of rewrite of other modules
 *   StringView.h [new]
 *     - New implementation, better functionality
 */
// Code
#ifndef __CBUILD_COMMON_H__
#define __CBUILD_COMMON_H__
// Includes (all external included of CBuild. Other header could only have
// project-level includes)
#include "ctype.h"
#include "dirent.h"
#include "errno.h"
#include "fcntl.h"
#include "limits.h"
#include "stdarg.h"
#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/mman.h"
#include "sys/stat.h"
#include "sys/types.h"
#include "sys/wait.h"
#include "time.h"
#include "unistd.h"
// Constants that can be redefined
/**
 * @brief Init capacity of cbuild datastructures. `unsigned long`
 */
#ifndef CBUILD_INIT_CAPACITY
#	define CBUILD_INIT_CAPACITY 256ul
#endif // CBUILD_INIT_CAPACITY
/**
 * @brief Minimim log level. `CBuildLogLevel`
 */
#ifndef CBUILD_LOG_MIN_LEVEL
#	define CBUILD_LOG_MIN_LEVEL CBUILD_LOG_ERROR
#endif // CBUILD_LOG_MIN_LEVEL
/**
 * @brief	Size of sprintf buffer that will be used as a fast-path.
 * `unsigned long`
 */
#ifndef CBUILD_SB_QUICK_SPRINTF_SIZE
#	define CBUILD_SB_QUICK_SPRINTF_SIZE 512ul
#endif // CBUILD_SB_QUICK_SPRINTF_SIZE
/**
 * @brief Temporary buffer size for file copies
 */
#ifndef CBUILD_TMP_BUFF_SIZE
#	define CBUILD_TMP_BUFF_SIZE (32 * 1024 * 1024)
#endif // CBUILD_TMP_BUFF_SIZE
// OS-specific defines
#if defined(__linux__)
#	define CBUILD_OS_LINUX
#	define CBUILD_API_POSIX
#elif defined(__APPLE__) || defined(__MACH__)
#	define CBUILD_OS_MACOS
#	define CBUILD_API_POSIX
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) ||   \
		defined(__DragonFly__)
#	define CBUILD_OS_BSD
#	define CBUILD_API_POSIX
#elif defined(__unix__)
#	define CBUILD_OS_UNIX
#	define CBUILD_API_POSIX
#elif defined(__CYGWIN__)
#	define CBUILD_OS_WINDOWS_CYGWIN
#	define CBUILD_API_POSIX
#elif defined(__MINGW32__) || defined(__MINGW64__)
#	define CBUILD_OS_WINDOWS_MINGW
#	define CBUILD_API_WIN32
#	error "This library support only POSIX api and MSVC only supports WinAPI"
#elif defined(_MSC_VER)
#	define CBUILD_OS_WINDOWS_MSVC
#	define CBUILD_API_WIN32
#	error "This library support only POSIX api and MSVC only supports WinAPI"
#else
#	error                                                                        \
			"This OS is unsupported by CBuild. If it supports POSIX API then you can add new compile-time check for your current OS and define API macro and OS macro and add compiler macro check for your OS. If it don't support any of this APIs then you need to create your own API macro and change implementation-specifc parts of CBuild"
#endif // OS selector
// Different between different APIs
#if defined(CBUILD_API_POSIX)
// Platform includes
#	if defined(CBUILD_OS_MACOS)
#		include "crt_externs.h"
#	endif // CBUILD_OS_MACOS
// Print functions
#	define __CBUILD_PRINT(str)                printf((str))
#	define __CBUILD_PRINTF(fmt, ...)          printf((fmt), __VA_ARGS__)
#	define __CBUILD_VPRINTF(fmt, va_args)     vprintf((fmt), (va_args))
#	define __CBUILD_FLUSH()                   fflush(stdout)
// Maybe you want to redefine this two macro to work with stderr, but I prefer
// to have my errors in standard stdout
#	define __CBUILD_ERR_PRINT(str)            printf((str))
#	define __CBUILD_ERR_PRINTF(fmt, ...)      printf((fmt), __VA_ARGS__)
#	define __CBUILD_ERR_VPRINTF(fmt, va_args) vprintf((fmt), (va_args))
#	define __CBUILD_ERR_FLUSH()               fflush(stdout)
// Memory functions
#	define __CBUILD_MALLOC                    malloc
#	define __CBUILD_REALLOC                   realloc
#	define __CBUILD_MEMCPY                    memcpy
#	define __CBUILD_FREE                      free
// Process and file handles
typedef pid_t cbuild_proc_t;
#	define CBUILD_INVALID_PROC -1
typedef int cbuild_fd_t;
#	define CBUILD_INVALID_FD -1
// For pointer errors
#	define CBUILD_PTR_ERR    (void*)((intptr_t)-1)
#endif // CBUILD_API_POSIX
// Macro functionality
#define __CBUILD_STRINGIFY(var)  #var
#define __CBUILD_XSTRINGIFY(var) __CBUILD_STRINGIFY(var)
#define __CBUILD_CONCAT(a, b)    a##b
#define __CBUILD_XCONCAT(a, b)   __CBUILD_CONCAT(a, b)
// Command and process functions
// Some preprocessor trickery
/**
 * @brief Mark variable as usused
 *
 * @param val => any -> Any variable
 */
#define CBUILD_UNUSED(val)       (void)(val)
/**
 * @brief Mark some part of code as TODO, code will error-out if this code is
 * executed
 *
 * @param message => char* -> Additional message
 */
#define CBUILD_TODO(message)                                                   \
	do {                                                                         \
		__CBUILD_ERR_PRINTF("%s:%d: TODO: %s\n", __FILE__, __LINE__, (message));   \
		abort();                                                                   \
	} while (0)
/**
 * @brief Mark some part of code as UNREACHABLE, code will error-out if this
 * code is executed
 *
 * @param message => char* -> Additional message
 */
#define CBUILD_UNREACHABLE(message)                                            \
	do {                                                                         \
		__CBUILD_ERR_PRINTF("%s:%d: UNREACHABLE: %s\n", __FILE__, __LINE__,        \
		                    (message));                                            \
		abort();                                                                   \
	} while (0)
// More user-friendly array operations
/**
 * @brief get size of array
 *
 * @param array => any[] -> Array
 */
#define cbuild_arr_len(array) (sizeof(array) / sizeof((array)[0]))
/**
 * @brief Assert function that allows to print custom message
 *
 * @param expr -> Assert expression
 * @param ... -> Printf arguments, "" if no message is needed
 */
#define cbuild_assert(expr, ...)                                               \
	(expr) ? (void)(0)                                                           \
				 : __cbuild_assert(__FILE__, __LINE__, __func__, #expr, __VA_ARGS__)
/**
 * @brief Internal function that does assert
 *
 * @param file => const char* -> __FILE__
 * @param line => const char* -> __LINE__
 * @param func => const char* -> __func__
 * @param expr => const char* -> String form of an assert expression
 * @param ...[0] => const char* -> Format string for and printf
 * @param ... -> Printf args
 */
void __cbuild_assert(const char* file, unsigned int line, const char* func,
                     const char* expr, ...) __attribute__((__noreturn__));
/**
 * @brief Get element from array, errors-out at invalid index
 *
 * @param array => any[] -> Array
 * @param index => size_t -> Array index, should not have side effects
 */
#define cbuild_arr_get(array, index)                                           \
	(cbuild_assert((size_t)(index) < cbuild_arr_len(array),                      \
	               "Index %zu is out of bounds!\n", (size_t)index),              \
	 (array)[(size_t)(index)])
/**
 * @brief Shift args in and array with size (like argv and argc pair. Should
 * work similartly to bash `shift`
 *
 * @param argv => T[] -> Array
 * @paran argc => Integer -> Array size
 * */
#define cbuild_shift(argv, argc)                                               \
	(cbuild_assert((argc) > 0, "More arguments is required!\n"), (argc)--,       \
	 *(argv)++)
/**
 * @brief Shift args in and array with size (like argv and argc pair. Should
 * work similartly to bash `shift`. Takes addiitional message to print on error
 *
 * @param argv => T[] -> Array
 * @paran argc => Integer -> Array size
 * @param ... -> Printf arfs
 * */
#define cbuild_shift_expect(argv, argc, ...)                                   \
	(cbuild_assert((argc) > 0, __VA_ARGS__), (argc)--, *(argv)++)
// Version
#define CBUILD_VERSION        "v1.6"
#define CBUILD_VERSION_MAJOR  1
#define CBUILD_2VERSION_MINOR 6
#endif // __CBUILD_COMMON_H__
