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
 *     - Added 'cbuild_proc_start' function.
 * --------------------------------------------
 * 27.12.2024  v1.2    Bugfix release
 *   Compile.h [bugfix]
 *     - Fixed type ('CBUILD_CARG_WARN' -> 'CBUILD_CARGS_WARN').
 *	 Log.h [bugfix]
 *     - Logger always use one output stream for its messages now.
 *   General [bugfix]
 *     - Added 'CBUILD_IMPL' macro to allow multiple includes in same
 *       translation unit.
 * --------------------------------------------
 * 10.01.2025  v1.3    New API features and bugfixes
 *   common.h [bugfix]
 *     - Fixed bug in macro (wrong variable name used).
 *     - Switched 'CBuildFD' typedef from '__pid_t' to 'pid_t.
 *   common.h	[feature]
 *     - Added 'cbuild_shift_expect' (allows to have error message on top of
 *       'cbuild_shift').
 *     - Added 'cbuild_assert'.
 *     - Added 'noreturn' attribute to an assert function.
 *   Command.h [bugfix]
 *     - Added 'fd' prefix to 'stdin', 'stdout' and 'stderr' for compatibility
 *       with libc that defines this names as a macro.
 *     - Fixed typo in error message.
 *	 Log.h [feature]
 *     - Logger now use 16-color ANSI mode.
 *     - Added 'cbuild_vlog' that takes 'va_list' inserted of variadic arguments.
 *	 General [bugfix]
 *     - Added few 'const' annotations to pointers.
 *     - Changed all asserts to 'cbuild_assert'.
 * --------------------------------------------
 * 18.02.2025  v1.4    Small bugfix
 *   common.h [bugfix]
 *     - Wrong argument passed to '__CBUILD_ERR_PRINTF' in 'CBuild_UNREACHAGLE'.
 * --------------------------------------------
 * 18.02.2025  v1.5    Bugfix release
 *   impl.c [bugfix]
 *     - Fixed buffer overflow in 'cbuild_dir_copy'.
 *     - Improved 'cbuild_shift'.
 *   General [change]
 *     - Changed macro 'CBUILD_IMPL' to 'CBUILD_IMPLEMENTATION'.
 * --------------------------------------------
 * 16.06.2025  v1.6    Full rewrite
 *   Term.h [new]
 *     - ANSI wrapper module.
 *   Log.h [feature]
 *     - Rely on 'Term.h' for colored output.
 *     - Runtime configuration for minimum log level.
 *     - Logger now support customizable formatting callbacks.
 *   DynArray.h [new]
 *     - Different implementation.
 *	 common.h [feature]
 *     - Fixes for MacOS related to '__progname'.
 *     - Better OS and API abstraction.
 *     - Unified INIT_CAPACITY macro.
 *     - Better IO abstraction.
 *     - Assert now use proper IO abstractions.
 *     - Types for WinAPI.
 *	 StringBuilder.h [new]
 *     - Rely on new 'DynArray.h'.
 *     - Few new operations.
 *   Proc.h [feature]
 *     - Better naming scheme.
 *     - Added ability to allocate inter-process shared memory.
 *   Command.h [feature]
 *     - More consistent names.
 *   Map.h [new]
 *     - Hash-map implementation.
 *     - Hash functions for string added.
 *   Compile.h [change]
 *     - Added consistent prefixes.
 *     - Added output arg	macro.
 *   FS.h [change]
 *     - Changes becouse of rewrite of other modules.
 *   StringView.h [new]
 *     - New implementation, better functionality.
 *     - 'find', 'rfind' and 'contains' function.
 *     - 'memrchr' is used on MacOS, *BSD and Glibc (if detected).
 *   Compile.h [feature]
 *     - Better naming.
 *     - More macro.
 *   Span.h [new]
 *     - New module - array+size.
 *   Stack.h [new]
 *     - New module - basic re-sizable stack implementation.
 *   DLload.h [new]
 *     - Small wrapper for runtime loading of dynamic libraries, first code to
 *       support 'WinAPI'.
 *   FlagParse.h [new]
 *     - CLI flag parser library.
 *     - Supports long and short options.
 *     - Support different flag arguments (but not support their types).
 *     - Pre-parses positional arguments - collects them in separate buffer.
 *     - Has build-in help and version flags and build-in help and version subs.
 *     - Has function to print proper help for all defined flags.
 *     - Allow overrides to help and version stubs using 'weak' linking.
 *   General [feature]
 *     - Better error handling.
 *     - More integrated logging.
 *     - Slight formatting change.
 *     - Proper use of asserts in place of older 'exit(0);'.
 * --------------------------------------------
 * 2025-07-01  v1.7    Fix in build script and few code fixes
 *   General [bugfix]
 *     - Add 'FlagParse.h' into a output library ;).
 *   Compile.h [bugfix]
 *     - New file now explicitly marked as executable.
 *     - Allow adding compile argument for self-rebuild.
 *     - Changed API of 'cbuild_cmd_to_sb' to match other 'cbuild_*_to_*'
 *       functions.
 *   FlagParse.h [bugfix]
 *     - Fixed short flags for help and version.
 *     - Fixed 'cbuild_flag_get_flag' to actually return NULL if flag was not
 *       specified..
 *   FS.h [change]
 *     - Different implementation for 'cbuild_dir_list'.
 *     - '.' and '..' now excluded.
 *     - Entries now sorted alphabetically.
 *     - Disable file descriptor auto inheritance on exec by default.
 * --------------------------------------------
 * 2025-07-09  v1.8    Command updates
 *   Command.h [feature]
 *     - Allow to autokill child processes on exit on Linux.
 *   Command.h [change]
 *     - Changed cbuild_cmd_t format.
 * --------------------------------------------
 * 2025-07-21  v1.9    Back to beginning
 *   General
 *     - Revert basic data types to be implemented as a raw macro.
 *       Map is not rewritten because complexity is too high.
 *   Map.h [bugfix]
 *     - Fixed memory leak.
 *   Span.h [remove]
 *     - Removed because it is useless.
 * --------------------------------------------
 * 2025-07-24  v1.10    API cleanup
 *   Map.h [change]
 *     - Rewrite with a new API. Same basic design, but now use void* and
 *       elem_size to be type-agnostic. Should be much faster (less function
 *       pointers) and have API more tailored to read-modify-write patter.
 *       Map now access only a key (and only does 'read' access).
 *   Compile.h [change]
 *     - Now '-Wall', '-Wextra', '-Wno-comments', '-Wconversion' and
 *       '-Wcast-align' is default set of compile args on selfrebuild.
 *   FlagParse.h [change]
 *     - 'cbuild_flag_flg_help' renamed to 'cbuild_flag_print_help'.
 *   StringView.h [feature]
 *     - Added compile-time constructor for string literals.
 *   Stack.h [feature]
 *     - Added 'cbuild_stack_top' to get top element in a read mode.
 *   Proc.h [feature]
 *     - Added 'cbuild_proc_is_running' to check process without blocking.
 *   FS.h [feature]
 *     - Added 'cbuild_fd_read' and 'cbuild_fd_write' to abstract POSIX 'read'
 *       and 'write'.
 *   FlagParse.h [feature]
 *     - Added 'alias' command to DSL - allow to define long option aliases to
 *       another option, used in help message and while parsing, but cannot be
 *       targeted in 'cbuild_flag_get_flag'.
 *     - Added flag groups. Added metadata key 'group' that allow to append flag
 *       to a specific group. Added command 'group' that allow to add extended
 *       description to a group. Groups will only be used during help generation
 *       and flags from same group will be grouped under same heading.
 *   Arena.h [new]
 *     - Temporary allocated. Experimental module.
 *   General [new]
 *     - New allocator hook system.
 *   DynArray.h [bugfix]
 *     - If 'cbuild_da_resize' shrinks array and size becomes invalid it is
 *       decrement.
 *   StringView.h [bugfix]
 *     - Function that can return '-1' now returns 'ssize_t' not 'size_t'.
 * --------------------------------------------
 * 2025-08-05  v1.11		Cleanup and utf8
 *   FS.h [feature]
 *     - 'cbuild_fs_move' and 'cbuild_fs_remove' now try 'rename' before copying.
 *   Log.h [change]
 *     - Renamed types.
 *   Compile.h [bugfix]
 *     - Now default warn flags include VLA warnings.
 *     - Now if compilation failed in selfrebuild, old version will be run.
 *   FlagParse.h [change]
 *     - Short flag for version option is more more conventional 'V'.
 *   StringView.h [feature]
 *     - Optional utf8 supports.
 *   StringBuilder.h [feature]
 *     - Optional utf8 support.
 *   General [bugfix]
 *     - Now functions that takes no arguments properly have 'void' as arguments.
 * --------------------------------------------
 * 2025-08-18  v0.12    Switch to zero-ver
 *   Compile.h [feature]
 *     - New 'cbuild_selfrebuild_ex' allow to check more files before rebuild.
 *   Command.h [change]
 *     - Added new API, 'cbuild_cmd_run'.
 *   Command.h [feature]
 *     - Improved output of 'cbuild_cmd_to_sb'.
 *   Proc.h [feature]
 *     - Added ability to wait for multiple processes.
 *   DynArray.h [feature]
 *     - Added 'cbuild_da_remove_unordered'.
 *   Compile.h [bugfix]
 *     - Fixed 'CBUILD_CARGS_INCLUDE'.
 *   FS.h [feature]
 *     - Now 'cbuild_dir_create' will create all needed dirs recursively.
 *     - Added 'cbuild_symlink', which forces symlink at destination.
 *     - Added 'cbuild_path_normalize' to normalize paths without FS lookup.
 *       Strips "<dir>/../: pairs, removes ".", remove trailing slashes
 *       and collapse slash sequences.
 *   FS.h [bugfix]
 *     - 'cbuild_path_filetype' now use 'lstat'.
 *   General [new]
 *     - Now test on 'musl' libc too.
 *     - Now test on strict POSIX.1-2001.
 *     - Now I use zero-ver. This will fully break all version checks.
 *       The only change is that leading '1' will be '0'.
 *       "Real" version number will be monotonically incremented as always.
 *   General [bugfix]
 *     - Better OS-detect logic.
 *     - First deprecated APIs.
 *     - Added CBDEF for all functions (user-overridable).
 *     - Fixed macro to document when they evaluate args multiple times.
 */
// Code
// NOTE: CBuild should be a first header to be included in translation unit, or
// you need to define '_GNU_SOURCE' yourself
#ifndef __CBUILD_COMMON_H__
#define __CBUILD_COMMON_H__
// OS-specific defines
#if defined(__linux__)
	// Use GNU extentions if possible
	#if !defined(_GNU_SOURCE) && !defined(STRICT_POSIX)
		#define _GNU_SOURCE
	#endif // _GNU_SOURCE
	#include <features.h>
	// Needed by CBuild
	#define CBUILD_OS_LINUX
	#define CBUILD_API_POSIX
	// Zoo of linux libc's
	#if defined(__GLIBC__)
		#define CBUILD_OS_LINUX_GLIBC
	#elif defined(__UCLIBC__)
		#define CBUILD_OS_LINUX_UCLIBC
	#else // Assume musl
		#define CBUILD_OS_LINUX_MUSL
	#endif // Libc selector
#elif defined(__APPLE__) || defined(__MACH__)
	#define CBUILD_OS_MACOS
	#define CBUILD_API_POSIX
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) ||   \
	defined(__DragonFly__)
	#define CBUILD_OS_BSD
	#define CBUILD_API_POSIX
#elif defined(__unix__)
	#define CBUILD_OS_UNIX
	#define CBUILD_API_POSIX
#elif defined(__CYGWIN__)
	#define CBUILD_OS_WINDOWS_CYGWIN
	#define CBUILD_API_POSIX
#elif defined(__MINGW32__) || defined(__MINGW64__)
	#define CBUILD_OS_WINDOWS_MINGW
	#define CBUILD_API_WIN32
	#error "This library support only POSIX api for now and MinGW only supports WinAPI"
#elif defined(_MSC_VER)
	#define CBUILD_OS_WINDOWS_MSVC
	#define CBUILD_API_WIN32
	#error "MSVC is fully unsupported as a compiler. Please use gcc/clang-compatible compiler! Compiler should support 'gnu99' standard!"
#else
	#error                                                                        \
	"This OS is unsupported by CBuild. If it supports POSIX API then you can add new compile-time check for your current OS and define API macro and OS macro and add compiler macro check for your OS. If it don't support any of this APIs then you need to create your own API macro and change implementation-specifc parts of CBuild"
#endif // OS selector
// Includes (all external included of CBuild. Other header could only have
// project-level includes)
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
// Constants that can be redefined
/**
 * @brief Init capacity of cbuild datastructures. `unsigned long`
 */
#ifndef CBUILD_INIT_CAPACITY
	#define CBUILD_INIT_CAPACITY 256ul
#endif // CBUILD_INIT_CAPACITY
/**
 * @brief Minimim log level. `CBuildLogLevel`
 */
#ifndef CBUILD_LOG_MIN_LEVEL
	#define CBUILD_LOG_MIN_LEVEL CBUILD_LOG_ERROR
#endif // CBUILD_LOG_MIN_LEVEL
/**
 * @brief	Size of sprintf buffer that will be used as a fast-path.
 * `unsigned long`
 */
#ifndef CBUILD_SB_QUICK_SPRINTF_SIZE
	#define CBUILD_SB_QUICK_SPRINTF_SIZE 512ul
#endif // CBUILD_SB_QUICK_SPRINTF_SIZE
/**
 * @brief Temporary buffer size for file copies
 */
#ifndef CBUILD_TMP_BUFF_SIZE
	#define CBUILD_TMP_BUFF_SIZE (32ul * 1024ul * 1024ul)
#endif // CBUILD_TMP_BUFF_SIZE
/**
 * @brief Default hash function for a hash map.
 * Function signature is this: size_t func(const void* key, size_t len)
 */
#ifndef CBUILD_MAP_DEFAULT_HASH_FUNC
	#define CBUILD_MAP_DEFAULT_HASH_FUNC __cbuild_int_map_hash_func
#endif // CBUILD_MAP_DEFAULT_HASH_FUNC
/**
 * @brief Size of CBuild temporary allocator arena
 */
#ifndef CBUILD_TEMP_ARENA_SIZE
	#define CBUILD_TEMP_ARENA_SIZE (8ul * 1024ul * 1024ul)
#endif // CBUILD_TEMP_ARENA_SIZE
/**
* @brief Default self-rebuild arguments
*/
#ifndef CBUILD_SELFREBUILD_ARGS
	#define CBUILD_SELFREBUILD_ARGS CBUILD_CARGS_WARN
#endif // CBUILD_SEFLREBUILD_ARGS
// Print abstraction	// Print functions
#define __CBUILD_PRINT(str)                printf((str))
#define __CBUILD_PRINTF(fmt, ...)          printf((fmt), __VA_ARGS__)
#define __CBUILD_VPRINTF(fmt, va_args)     vprintf((fmt), (va_args))
#define __CBUILD_FLUSH()                   fflush(stdout)
// Maybe you want to redefine this two macro to work with stderr, but I prefer
// to have my errors in standard stdout
#define __CBUILD_ERR_PRINT(str)            printf((str))
#define __CBUILD_ERR_PRINTF(fmt, ...)      printf((fmt), __VA_ARGS__)
#define __CBUILD_ERR_VPRINTF(fmt, va_args) vprintf((fmt), (va_args))
#define __CBUILD_ERR_FLUSH()               fflush(stdout)
// Different between different APIs
#if defined(CBUILD_API_POSIX)
	// Platform includes
	#if defined(CBUILD_OS_MACOS)
		#include "crt_externs.h"
	#endif // CBUILD_OS_MACOS
	#if defined(CBUILD_OS_LINUX)
		#include "sys/prctl.h"
	#endif // CBUILD_OS_LINUX
	#include "dlfcn.h"
	// Process and file handles
	typedef pid_t cbuild_proc_t;
	#define CBUILD_INVALID_PROC -1
	typedef int cbuild_fd_t;
	#define CBUILD_INVALID_FD -1
	// For pointer errors
	#define CBUILD_PTR_ERR    (void*)((intptr_t)-1)
#elif defined(CBUILD_API_WIN32)
	// Platform includes
	#include <Windows.h>
	// Process and file handles
	typedef HANDLE cbuild_proc_t;
	#define CBUILD_INVALID_PROC ((HANDLE)(intptr_t)-1)
	typedef HANDLE cbuild_fd_t;
	#define CBUILD_INVALID_FD ((HANDLE)(intptr_t)-1)
	// For pointer errors
	#define CBUILD_PTR_ERR (void*)((intptr_t)-1)
#endif // CBUILD_API_*
// Macro functionality
#define __CBUILD_STRINGIFY(var)  #var
#define __CBUILD_XSTRINGIFY(var) __CBUILD_STRINGIFY(var)
#define __CBUILD_CONCAT(a, b)    a##b
#define __CBUILD_XCONCAT(a, b)   __CBUILD_CONCAT(a, b)
// Attributes
/**
 * @brief Allow user to add more attributes to a functions
 */
#ifndef CBDEF
	#define CBDEF
#endif // CBDEF
#if defined(__has_c_attribute)
	#if __has_c_attribute(deprecated)
		#define CBUILD_DEPRECATED(msg, decl) [[deprecated(msg)]] decl
	#else
		#define CBUILD_DEPRECATED(msg, decl) decl __attribute__((deprecated(msg)))
	#endif
#else
	#define CBUILD_DEPRECATED(msg, decl) decl __attribute__((deprecated(msg)))
#endif
// Global allocator
extern void* (*cbuild_malloc)(size_t size);
extern void* (*cbuild_realloc)(void* ptr, size_t size);
extern void (*cbuild_free)(void* ptr);
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
		  (message));                                                              \
		abort();                                                                   \
	} while (0)
// More user-friendly array operations
/**
 * @brief Get size of array
 *
 * @param array => any[] -> Array. Evaluated twice by macro.
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
CBDEF void __cbuild_assert(const char* file, unsigned int line, const char* func,
  const char* expr, ...) __attribute__((__noreturn__));
/**
 * @brief Get element from array, errors-out at invalid index
 *
 * @param array => any[] -> Array
 * @param index => size_t -> Array index, should not have side effects
 */
#define cbuild_arr_get(array, index)                                           \
	(cbuild_assert((size_t)(index) < cbuild_arr_len(array),                      \
	  "Index %zu is out of bounds!\n", (size_t)index),                           \
	  (array)[(size_t)(index)])
/**
 * @brief Shift args from an array with size (like argv and argc pair). Should
 * work similartly to bash `shift`
 *
 * @param argv => T[] -> Array
 * @paran argc => Integer -> Array size. Evaluated twice by macro.
 * */
#define cbuild_shift(argv, argc)                                               \
	(cbuild_assert((argc) > 0, "More arguments is required!\n"), (argc)--,       \
	  *(argv)++)
/**
 * @brief Shift args in and array with size (like argv and argc pair. Should
 * work similartly to bash `shift`. Takes addiitional message to print on error
 *
 * @param argv => T[] -> Array
 * @paran argc => Integer -> Array size. Evaluated twice by macro.
 * @param ... -> Printf arfs
 * */
#define cbuild_shift_expect(argv, argc, ...)                                   \
	(cbuild_assert((argc) > 0, __VA_ARGS__), (argc)--, *(argv)++)
// Version
#define CBUILD_VERSION "v0.12"
#define CBUILD_VERSION_MAJOR 0
#define CBUILD_VERSION_MINOR 12
#endif // __CBUILD_COMMON_H__
