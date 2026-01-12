/**
 * @file cbuild.h
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief C standard library extender and simple build system library.
 * Library consists of several modules. Some of them require other modules.
 * Some of the modules can work almost independantly. FOr more information,
 * please visit [CBuild wiki](https://WolodiaM.github.io/CBuild/).
 *
 * @date 2025-09-07
 * @copyright (C) 2025 WolodiaM
 * @licenseGPL-3.0-or-later
 *
 * Copyright (C) 2025 WolodiaM
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
 *     - Fixed typo ('CBUILD_CARG_WARN' -> 'CBUILD_CARGS_WARN').
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
 *     - Added 'cbuild_vlog' that takes 'va_list' inserted of variadic
 *       arguments.
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
 *     - 'cbuild_fs_move' and 'cbuild_fs_remove' now try 'rename' before
 *       copying.
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
 *     - Now functions that takes no arguments properly have 'void' as
 *       arguments.
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
 * --------------------------------------------
 * 2025-09-02  v0.13
 *   common.h [change]
 *     - Better platform detection logic.
 *   common.h [feature]
 *     - New timing function.
 *     - CBUILD_MAX and CBUILD_MIN macro.
 *   Commang.h [feature]
 *     - Added 'file_std*' as optional args that can be used to redirect
 *       directly.
 *       to a file.
 *     - Added ability to control how many async workers can be spawned.
 *   Command.h [remove]
 *     - Few small changes to an API. Removed few useless args.
 *   FS.h [feature]
 *     - Added 'cbuild_dir_current' and 'cbuild_dir_set_current'.
 *     - Now cbuild_path_normalize respect POSIX network paths stating from '//'
 *     - Added 'CBUILD_FTYPE_MISSING'
 *   FS.h [change]
 *     - Now 'cbuild_path_filetype' return 'CBUILD_FTYPE_MISSING' on invalid
 *       files.
 *   Proc.h [feature]
 *     - Ability to get amount of CPU cores.
 *     - Ability to wait for any process from a list.
 *   Log.h [feature]
 *     - Added shortcuts for built-in log levels.
 *     - Made CBUILD_LOG_PRINT print always (except if
 *       CBUILD_LOG_NO_LOGS is enabled).
 *     - Made enum for print levels not sparse. Extension of log level was not really 
 *       a useful feature.
 *     - Removed cbuild_log_fmt.
 *     - Exported few internal function to allow easier extension of logger
 *   Arena.h [feature]
 *     - Added ability to do checkpoint and rewind allocator to that checkpoint.
 *       To reset allocator you should use '0' in place of a checkpoint.
 *   StringView.h [bugfix]
 *     - Fixed bug in rfind for strict POSIX
 *     - Added early return for both find and rfind
 *   StringView.h [change]
 *     - Deprecated cbuild_sv_utf8cmp. cbuild_sv_cmp is enough.
 *   StringBuilder.h [change]
 *     - Deprecated cbuild_sb_utf8cmp. cbuild_sb_cmp is enough.
 *   Compile.h [feature]
 *     - Added 'CBUILD_CARGS_DEFINE_TEMP' and 'CBUILD_CARGS_DEFINE_VAL_TEMP'
 *      that use 'cbuild_temp_sprintf' to support runtime values or names
 *      for defines.
 *     - Added 'CBUILD_CARGS_DEBUG_GDB' that expands to '-ggdb'. It will be fine
 *       if you use gdb debugger or your debugger support all of gdb extensions.
 *     - Added __cbuild_selfrebuild_ex_argv to allow having self-rebuilding 
 *       binaries in PATH. For normal binaries can be 'argv[0]'.
 *   General [change]
 *     - Changed CBDEF to CBUILDDEF.
 *     - Remove build stage for a header.
 * --------------------------------------------
 * 2025-01-12  v0.14
 *   DynArray.h [feature]
 *     - Add macro to allow easy creation of required structures.
 *   General [bugfix]
 *     - Added some 'const' annotations
 *     - Moved few functions to take parameters by value and not by pointer
 */
// NOTE: CBuild should be a first header to be included in translation unit, or
// you need to define '_GNU_SOURCE' yourself if you use default CBuild API for
// POSIX (CBUILD_API_POSIX).
// CBuild load a lot of C and platform specific headers, so you are not required
// to load them manually (but no one forbids you to do that). Also, while most
// of basic ones are loaded, only headers required for CBuild are present.
#ifndef __CBUILD_H__
#define __CBUILD_H__
/* common.h */
// OS-specific defines
// Valid compilers:
//   - CBUILD_CC_CLANG - clang or its derivative
//   - CBUILD_CC_GCC   - gcc or its derivative
// Several compiler extensions are required from supported compiler:
//   - Binary literals support (0b) either trough c standard (c23+) or as extension
//   - Attributes trough __attribute__((...)) syntax, some are supported trough [[...]]
//     syntax if support is detected 
//       * deprecated
//       * format
//       * noreturn
//       * falltrough
//   - Unnamed structs/enums/unions
//   - typeof
//   - __VA_OPT__
//   - GNU statement expressions '({...})'. This requirement may be removed in the future.
// Explicetely unsupported compilers:
//   - TCC  - no statement expression (at least).
//   - MSVC - no extensions at all.
#if !defined(CBUILD_CC_DEFINED)
	#if defined(__clang__)
		#define CBUILD_CC_CLANG
	#elif defined(__TINYC__)
		#error "You are trying to compile CBuild with TinyC compiler. This compile is not supported. It should support 'gnu99' extensions, including binary literals; attributes; unnamed structs, enums, unions; typeof; __VA_OPT__; statement expressions."
	#elif defined(__GNUC__)
		#define CBUILD_CC_GCC
	#elif defined(_MSC_VER)
		#error "You are trying to compile CBuild with MSVC compiler. It does not support 'gnu99' extensions, including binary literals; attributes; unnamed structs, enums, unions; typeof; __VA_OPT__; statement expressions."
	#else
		#error "This compile is unsupported. If it supports 'gnu99' extensions, including binary literals; attributes; unnamed structs, enums, unions; typeof; __VA_OPT__; statement expressions; then you can simply add a check to this block for it."
	#endif // Compiler check
	#define CBUILD_CC_DEFINED
#endif // !CBUILD_CC_DEFINNED
// Valid OSes:
//   - CBUILD_OS_LINUX   - any desktop Linux (excluding Android or ChromeOS for example)
//   - CBUILD_OS_MACOS   - MacOS
//   - CBUILD_OS_BSD     - Any desktop BSD
//   - CBUILD_OS_UNIX    - Generic Unix, should support POSIX.1-2001
//   - CBUILD_OS_WINDOWS - Windows, should support WinAPI and NT native API
// Also, there are some auxiliary OS checks, valid values are:
//   - CBUILD_OS_WINDOWS:
//       * CBUILD_OS_WINDOWS_CYGWIN - windows under cygwin
//   - CBUILD_OS_LINUX:
//       * CBUILD_OS_LINUX_GLIBC  - Linux with glibc
//       * CBUILD_OS_LINUX_UCLIBC - Linux with uclibc
//       * CBUILD_OS_LINUX_MUSL   - Linux with musl (fallback, because musl provides no way of detection)
#if !defined(CBUILD_OS_DEFINED)
	#if defined(__linux__) || defined(linux) || defined(__linux)
		#define CBUILD_OS_LINUX
	#elif defined(__APPLE__) || defined(__MACH__)
		#define CBUILD_OS_MACOS
	#elif defined(__FreeBSD__) || \
		defined(__NetBSD__) || defined(__NETBSD__) || \
		defined(__OpenBSD__) || \
		defined(__DragonFly__)
		#define CBUILD_OS_BSD
	#elif defined(__unix__) || defined(unix) || defined(__unix)
		#define CBUILD_OS_UNIX
	#elif defined(__CYGWIN__)
		#define CBUILD_OS_WINDOWS
		#define CBUILD_OS_WINDOWS_CYGWIN
	#elif defined(_WIN32) || defined(_WIN64) || defined(__MINGW32__) || defined(__MINGW64__)
		#define CBUILD_OS_WINDOWS
	#else
		#error "This OS is unsupported by CBuild. If it supports POSIX.1-2001 you can simply add check for it to a list of checks here. If not you should add a check for OS and a macro for its API and then implement OS/API specifc parts of CBuild."
	#endif // OS detect
	// TODO: Android and IOS
#endif // !CBUILD_OS_DEFINED
// Valid APIs:
//   - CBUILD_API_POSIX        - POSIX.1-2001 (_POSIX_C_SOURCE == 200112L) with some GNU extensions, supports Linux, MacOS and BSD.
//   - CBUILD_API_STRICT_POSIX - POSIX.1-2001 (_POSIX_C_SOURCE == 200112L). Support any Unix that has this POSIX version supported. Only extension used:
//       * MAP_ANON/MAP_ANONYMOUS. If none is available require 'shm_opem' to be provided by either libc, 'librt.so' or 'librt.so.1', also required dlopen+dlsym to be provided by libc in this case.
//       * sysconf(_SC_NPROCESSORS_ONLN). Fallback is 1 because no way exist to query this information.
//     It can use extension if they are from later POSIX version and newer POSIX is detected.
//   - CBUILD_API_WINAPI        - WinAPI+NT native API. Requires codepage to be set to 65001 if utf8 functions are needed. Tries to use WinAPi when possible but relies on NT native API for more low-level tasks.
#if !defined(CBUILD_API_DEFINED)
	#if defined(CBUILD_OS_LINUX) || defined(CBUILD_OS_MACOS) || defined(CBUILD_OS_BSD)
		#define CBUILD_API_POSIX
	#elif defined(CBUILD_OS_UNIX)
		#define CBUILD_API_STRICT_POSIX
	#elif defined(CBUILD_OS_WINDOWS_CYGWIN)
		#define CBUILD_API_POSIX
	#elif defined(CBUILD_OS_WINDOWS)
		#define CBUILD_API_WINAPI
		#error "WinAPI is unsupported!"
	#else
		// TODO: Android/IOS/ChromeOS/IPadOS
		#error "This OS is not known by CBuild. If it supports POSIX.1-2001 you can simply add check for it's OS to a list of OS checks and define appropriate API macro there. If not you should add a check for OS and a macro for its API and then implement OS/API specifc parts of CBuild."
	#endif // API select
#endif // !CBUILD_API_DEFINED
// Different between different APIs
#if defined(CBUILD_API_POSIX)
	// Use GNU extentions if possible
	#if !defined(_GNU_SOURCE)
		#define _GNU_SOURCE
	#endif // _GNU_SOURCE
	// Platform include
	#include <dirent.h>
	#include <dlfcn.h>
	#include <errno.h>
	#include <fcntl.h>
	#include <signal.h>
	#include <sys/mman.h>
	#include <sys/stat.h>
	#include <sys/time.h>
	#include <sys/types.h>
	#include <sys/wait.h>
	#include <unistd.h>
	#if defined(CBUILD_OS_MACOS)
		#include <crt_externs.h>
	#endif // CBUILD_OS_MACOS
	#if defined(CBUILD_OS_LINUX)
		#if defined(__GLIBC__)
			#define CBUILD_OS_LINUX_GLIBC
		#elif defined(__UCLIBC__)
			#define CBUILD_OS_LINUX_UCLIBC
		#else // Assume musl
			#define CBUILD_OS_LINUX_MUSL
		#endif // Libc select
		#include <sys/prctl.h>
	#endif // CBUILD_OS_LINUX
	// Process and file handles
	typedef pid_t cbuild_proc_t;
	#define CBUILD_INVALID_PROC -1
	typedef int cbuild_fd_t;
	#define CBUILD_INVALID_FD -1
	// For pointer errors
	#define CBUILD_PTR_ERR (void*)((intptr_t)-1)
#elif defined(CBUILD_API_STRICT_POSIX)
	#if !defined(_POSIX_C_SOURCE)
		#define _POSIX_C_SOURCE 200112L
	#endif
	// Platform includes
	#include <dirent.h>
	#include <dlfcn.h>
	#include <errno.h>
	#include <fcntl.h>
	#include <signal.h>
	#include <sys/mman.h>
	#include <sys/stat.h>
	#include <sys/time.h>
	#include <sys/types.h>
	#include <sys/wait.h>
	#include <unistd.h>
	// Process and file handles
	typedef pid_t cbuild_proc_t;
	#define CBUILD_INVALID_PROC -1
	typedef int cbuild_fd_t;
	#define CBUILD_INVALID_FD -1
	// For pointer errors
	#define CBUILD_PTR_ERR (void*)((intptr_t)-1)
#elif defined(CBUILD_API_WINAPI)
	// Platform includes
	#include <windows.h>
	#include <io.h>
	// Process and file handles
	typedef HANDLE cbuild_proc_t;
	#define CBUILD_INVALID_PROC ((HANDLE)(intptr_t)-1)
	typedef HANDLE cbuild_fd_t;
	#define CBUILD_INVALID_FD ((HANDLE)(intptr_t)-1)
	// For pointer errors
	#define CBUILD_PTR_ERR (void*)((intptr_t)-1)
#endif // CBUILD_API_*
// Includes (all external included of CBuild. Other header could only have
// project-level includes)
#include <ctype.h>
#include <inttypes.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// Constants that can be redefined
/**
 * @brief Init capacity of cbuild datastructures. `size_t`
 */
#ifndef CBUILD_INIT_CAPACITY
	#define CBUILD_INIT_CAPACITY (size_t)256
#endif // CBUILD_INIT_CAPACITY
/**
 * @brief Minimim log level. `CBuildLogLevel`
 */
#ifndef CBUILD_LOG_MIN_LEVEL
	#define CBUILD_LOG_MIN_LEVEL CBUILD_LOG_ERROR
#endif // CBUILD_LOG_MIN_LEVEL
/**
 * @brief	Size of sprintf buffer that will be used as a fast-path. `size_t`
 * `unsigned long`
 */
#ifndef CBUILD_SB_QUICK_SPRINTF_SIZE
	#define CBUILD_SB_QUICK_SPRINTF_SIZE (size_t)512
#endif // CBUILD_SB_QUICK_SPRINTF_SIZE
/**
 * @brief Temporary buffer size for file copies. `size_t`
 */
#ifndef CBUILD_TMP_BUFF_SIZE
	#define CBUILD_TMP_BUFF_SIZE ((size_t)32 * (size_t)1024 * (size_t)1024)
#endif // CBUILD_TMP_BUFF_SIZE
/**
 * @brief Default hash function for a hash map.
 * Function signature is this: size_t func(const void* key, size_t len)
 */
#ifndef CBUILD_MAP_DEFAULT_HASH_FUNC
	#define CBUILD_MAP_DEFAULT_HASH_FUNC __cbuild_int_map_hash_func
#endif // CBUILD_MAP_DEFAULT_HASH_FUNC
/**
 * @brief Size of CBuild temporary allocator arena. `size_t`
 */
#ifndef CBUILD_TEMP_ARENA_SIZE
	#define CBUILD_TEMP_ARENA_SIZE ((size_t)8 * (size_t)1024 * (size_t)1024)
#endif // CBUILD_TEMP_ARENA_SIZE
/**
 * @brief Default self-rebuild arguments
 */
#ifndef CBUILD_SELFREBUILD_ARGS
	#define CBUILD_SELFREBUILD_ARGS CBUILD_CARGS_WARN
#endif // CBUILD_SEFLREBUILD_ARGS
// Print abstraction	// Print functions
#define __CBUILD_PRINT(str)                printf("%s", (str))
#define __CBUILD_PRINTF(fmt, ...)          printf((fmt), __VA_ARGS__)
#define __CBUILD_VPRINTF(fmt, va_args)     vprintf((fmt), (va_args))
#define __CBUILD_FLUSH()                   fflush(stdout)
// Maybe you want to redefine this two macro to work with stderr, but I prefer
// to have my errors in standard stdout
#define __CBUILD_ERR_PRINT(str)            printf("%s", (str))
#define __CBUILD_ERR_PRINTF(fmt, ...)      printf((fmt), __VA_ARGS__)
#define __CBUILD_ERR_VPRINTF(fmt, va_args) vprintf((fmt), (va_args))
#define __CBUILD_ERR_FLUSH()               fflush(stdout)
// Macro functionality
#define __CBUILD_STRINGIFY(var)  #var
#define __CBUILD_XSTRINGIFY(var) __CBUILD_STRINGIFY(var)
#define __CBUILD_CONCAT(a, b)    a ## b
#define __CBUILD_XCONCAT(a, b)   __CBUILD_CONCAT(a, b)
// Attributes
/**
 * @brief Allow user to add more attributes to a functions
 */
#ifndef CBUILDDEF
#define CBUILDDEF
#endif // CBUILDDEF
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
extern void*  (*cbuild_malloc)(size_t size);
extern void*  (*cbuild_realloc)(void* ptr, size_t size);
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
CBUILDDEF void __cbuild_assert(const char* file, unsigned int line,
	const char* func, const char* expr, ...) __attribute__((__noreturn__));
/**
 * @brief Get element from array, errors-out at invalid index
 *
 * @param array => any[] -> Array
 * @param index => size_t -> Array index, should not have side effects
 */
#define cbuild_arr_get(array, index)                                           \
	(cbuild_assert((size_t)(index) < cbuild_arr_len(array),                      \
			"Index %zu is out of bounds!\n", (size_t)index),                         \
		(array)[(size_t)(index)])
/**
 * @brief Shift args from an array with size (like argv and argc pair). Should
 * work similartly to bash `shift`
 *
 * @param argv => T[] -> Array
 * @paran argc => Integer -> Array size. Evaluated twice by macro.
 */
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
 */
#define cbuild_shift_expect(argv, argc, ...)                                   \
	(cbuild_assert((argc) > 0, __VA_ARGS__), (argc)--, *(argv)++)
/**
 * @brief Provides nanoseconds since some unspecifd time
 *
 * @return uint64_t -> Time in nanoseconds
 */
CBUILDDEF uint64_t cbuild_time_nanos(void);
#define CBUILD_NANOS_PER_SEC (1000*1000*1000)
/**
 * @brief Return maximum from 2 numbers.
 *
 * @param	a	=> number -> First number. Evaluated twice by macro.
 * @param b => number -> Second number. Evaluated twice by macro.
 */
#define CBUILD_MAX(a, b) a > b ? a : b
/**
 * @brief Return minimum from 2 numbers.
 *
 * @param	a	=> number -> First number. Evaluated twice by macro.
 * @param b => number -> Second number. Evaluated twice by macro.
 */
#define CBUILD_MIN(a, b) a < b ? a : b
// Version
#define CBUILD_VERSION "v0.14"
#define CBUILD_VERSION_MAJOR 0
#define CBUILD_VERSION_MINOR 14
/* Term.h */
// Escape
#define CBUILD_TERM_ESCAPE "\033["
#define CBUILD_TERM_CSI    CBUILD_TERM_ESCAPE // CSI - Control Sequence Introducer
#define CBUILD_TERM_SGR(n)                                                     \
	CBUILD_TERM_ESCAPE n "m"                    // SGR - Select Graphic Rendition
// Cursor
#define CBUILD_TERM_CURSOR_SET(x, y)                                           \
	CBUILD_TERM_CSI __CBUILD_STRINGIFY(x) ";" __CBUILD_STRINGIFY(y) "H"
#define CBUILD_TERM_CURSOR_SETH(x) CBUILD_TERM_CSI __CBUILD_STRINGIFY(x) "G"
#define CBUILD_TERM_CURSOR_MOVE(direction, num_steps)                          \
	CBUILD_TERM_CSI __CBUILD_STRINGIFY(num_steps) direction
#define CBUILD_TERM_UP              "A"
#define CBUILD_TERM_DOWN            "B"
#define CBUILD_TERM_RIGHT           "C"
#define CBUILD_TERM_LEFT            "D"
#define CBUILD_TERM_NEXT_LINE       "E"
#define CBUILD_TERM_PREV_LINE       "F"
#define CBUILD_TERM_CURSOR_GET      CBUILD_TERM_CSI "6n"
// Clean
#define CBUILD_TERM_ERASE_SCR(mode) CBUILD_TERM_CSI __CBUILD_STRINGIFY(mode) "J"
#define CBUILD_TERM_ERASE_LINE(mode)                                           \
	CBUILD_TERM_CSI __CBUILD_STRINGIFY(mode) "K"
#define CBUILD_TERM_CURSOR_TO_END    0
#define CBUILD_TERM_CURSOR_TO_BEGIN  1
#define CBUILD_TERM_FULL             2 // Resets cursor in screen mode
#define CBUILD_TERM_FULL_SCROLLBACK  3 // Dont work in line mode
// Style
#define CBUILD_TERM_RESET            CBUILD_TERM_SGR("0")
#define CBUILD_TERM_BOLD             CBUILD_TERM_SGR("1")
#define CBUILD_TERM_FAINT            CBUILD_TERM_SGR("2")
#define CBUILD_TERM_ITALIC           CBUILD_TERM_SGR("3")
#define CBUILD_TERM_NITALIC          CBUILD_TERM_SGR("23")
#define CBUILD_TERM_UNDERLINE        CBUILD_TERM_SGR("4")
#define CBUILD_TERM_NUNDERLINE       CBUILD_TERM_SGR("24")
#define CBUILD_TERM_BLINK            CBUILD_TERM_SGR("5")
#define CBUILD_TERM_NBLINK           CBUILD_TERM_SGR("25")
#define CBUILD_TERM_REVERSE          CBUILD_TERM_SGR("7")
#define CBUILD_TERM_NREVERSE         CBUILD_TERM_SGR("27")
#define CBUILD_TERM_CROSS_OUT        CBUILD_TERM_SGR("9")
#define CBUILD_TERM_NCROSS_OUT       CBUILD_TERM_SGR("29")
#define CBUILD_TERM_DOUBLE_UNDERLINE CBUILD_TERM_SGR("21")
#define CBUILD_TERM_OVERLINE         CBUILD_TERM_SGR("53")
#define CBUILD_TERM_NOVERLINE        CBUILD_TERM_SGR("55")
// Color - 4 bit
#define CBUILD_TERM_FG(color)        CBUILD_TERM_SGR(__CBUILD_STRINGIFY(color))
#define CBUILD_TERM_BG(color)                                                  \
	CBUILD_TERM_SGR(__CBUILD_XSTRINGIFY(color##addten))
#define CBUILD_TERM_BLACK           30
#define CBUILD_TERM_RED             31
#define CBUILD_TERM_GREEN           32
#define CBUILD_TERM_YELLOW          33
#define CBUILD_TERM_BLUE            34
#define CBUILD_TERM_MAGENTA         35
#define CBUILD_TERM_CYAN            36
#define CBUILD_TERM_WHITE           37
#define CBUILD_TERM_BRBLACK         90
#define CBUILD_TERM_BRRED           91
#define CBUILD_TERM_BRGREEN         92
#define CBUILD_TERM_BRYELLOW        93
#define CBUILD_TERM_BRBLUE          94
#define CBUILD_TERM_BRMAGENTA       95
#define CBUILD_TERM_BRCYAN          96
#define CBUILD_TERM_BRWHITE         97
#define CBUILD_TERM_BLACKaddten     40
#define CBUILD_TERM_REDaddten       41
#define CBUILD_TERM_GREENaddten     42
#define CBUILD_TERM_YELLOWaddten    43
#define CBUILD_TERM_BLUEaddten      44
#define CBUILD_TERM_MAGENTAaddten   45
#define CBUILD_TERM_CYANaddten      46
#define CBUILD_TERM_WHITEaddten     47
#define CBUILD_TERM_BRBLACKaddten   100
#define CBUILD_TERM_BRREDaddten     101
#define CBUILD_TERM_BRGREENaddten   102
#define CBUILD_TERM_BRYELLOWaddten  103
#define CBUILD_TERM_BRBLUEaddten    104
#define CBUILD_TERM_BRMAGENTAaddten 105
#define CBUILD_TERM_BRCYANaddten    106
#define CBUILD_TERM_BRWHITEaddten   107
// Color - 8 bit
#define CBUILD_TERM_FG8(color)                                                 \
	CBUILD_TERM_SGR("38;5;" __CBUILD_STRINGIFY(color))
#define CBUILD_TERM_BG8(color)                                                 \
	CBUILD_TERM_SGR("48;5;" __CBUILD_STRINGIFY(color))
#define CBUILD_TERM_UNDERLINE8(color)                                          \
	CBUILD_TERM_SGR("58;5;" __CBUILD_STRINGIFY(color))
// Color - 24 bit
#define CBUILD_TERM_FG24(r, g, b)                                              \
	CBUILD_TERM_SGR("38;2;" __CBUILD_STRINGIFY(r) ";" __CBUILD_STRINGIFY(g) ";"  \
		__CBUILD_STRINGIFY(b))
#define CBUILD_TERM_BG24(r, g, b)                                              \
	CBUILD_TERM_SGR("48;2;" __CBUILD_STRINGIFY(r) ";" __CBUILD_STRINGIFY(g) ";"  \
		__CBUILD_STRINGIFY(b))
#define CBUILD_TERM_UNDERLINE24(r, g, b)                                       \
	CBUILD_TERM_SGR("58;2;" __CBUILD_STRINGIFY(r) ";" __CBUILD_STRINGIFY(g) ";"  \
		__CBUILD_STRINGIFY(b))
/* Log.h */
typedef enum {
	CBUILD_LOG_NO_LOGS = -1,
	CBUILD_LOG_ERROR   = 1,
	CBUILD_LOG_WARN    = 2,
	CBUILD_LOG_INFO    = 3,
	CBUILD_LOG_TRACE   = 4,
	CBUILD_LOG_PRINT   = 5,
} cbuild_log_level_t;
/**
 * @brief Print logs
 *
 * @param level => cbuild_log_level_t -> Type of log
 * @param fmt => const char* -> Format string
 * @param ... => varargs -> Values for format string
 */
CBUILDDEF void cbuild_log(cbuild_log_level_t level, const char* fmt, ...)
__attribute__((format(printf, 2, 3)));
// Shortcuts
#define cbuild_log_error(msg, ...)                                             \
	cbuild_log(CBUILD_LOG_ERROR, msg __VA_OPT__(,) __VA_ARGS__)
#define cbuild_log_warn(msg, ...)                                              \
	cbuild_log(CBUILD_LOG_WARN, msg __VA_OPT__(,) __VA_ARGS__)
#define cbuild_log_info(msg, ...)                                              \
	cbuild_log(CBUILD_LOG_INFO, msg __VA_OPT__(,) __VA_ARGS__)
#define cbuild_log_trace(msg, ...)                                             \
	cbuild_log(CBUILD_LOG_TRACE, msg __VA_OPT__(,) __VA_ARGS__)
#define cbuild_log_print(msg, ...)                                             \
	cbuild_log(CBUILD_LOG_PRINT, msg __VA_OPT__(,) __VA_ARGS__)
/**
 * @brief Print logs but takes va list
 *
 * @param level => cbuild_log_level_t -> Type of log
 * @param fmt => const char* -> Format string
 * @param args => va_list -> Variadic arguments
 */
CBUILDDEF void cbuild_vlog(cbuild_log_level_t level, const char* fmt,
	va_list args);
/**
 * @brief Set minimum log level
 *
 * @param level => cbuild_log_level_t -> Log level
 */
CBUILDDEF void cbuild_log_set_min_level(cbuild_log_level_t level);
/**
 * @brief Get minimum log level
 *
 * @return cbuild_log_level_t -> Current log level
 */
CBUILDDEF cbuild_log_level_t cbuild_log_get_min_level(void);
// This two functions are semi-internal. They should not be used in normal code.
// But they are useful for for extending logger with small wrappers, like this:
// #define my_custom_log(msg, ...) \
//   __cbuild_int_log("[My custom level] ", msg __VA_OPT__(,) __VA_ARGS__)
// You can also use Term.h to provide additional styling for log level.
// Timestamp will be inserted automatically by logger in this case
// Second way to extend logger is to redefine cbuild_log_level_t and
// change __cbuild_int_log_level_names to provide your custom names and then
// just create a macro similar to default cbuild_log_* group (this will respect
// CBUILD_LOG_MIN_LEVEL but will require more work)
CBUILDDEF void __cbuild_int_vlog(const char* level, const char* fmt,
	va_list args);
CBUILDDEF void __cbuild_int_log(const char* level, const char* fmt, ...);
/* Arena.h */
/**
 * @brief Create new temp allocation
 *
 * @param size => size_t -> Number of bytes requested
 * @return void* -> Allocated pointer
 */
CBUILDDEF void* cbuild_temp_alloc(size_t size);
/**
 * @brief sprintf using temp allocation
 *
 * @param fmt => char* -> Format string
 * @param ... => variadics -> rguments
 * @return char* -> New string
 */
CBUILDDEF char* cbuild_temp_sprintf(const char* fmt, ...)
__attribute__((format(printf, 1, 2)));
/**
 * @brief vsprintf using temp allocation
 *
 * @param fmt => char* -> Format string
 * @param ap. => va_list -> rguments
 * @return char* -> New string
 */
CBUILDDEF char* cbuild_temp_vsprintf(const char* fmt, va_list ap);
/**
 * @brief strdup using temp allocation
 *
 * @param str => char* -> Original string
 * @return char* -> New dupped string
 */
CBUILDDEF char* cbuild_temp_strdup(const char* str);
/**
 * @brief Dupes memory using temporary allocation
 *
 * @param mem => void* -> Original pointer
 * @param size => size_t -> Number of bytes to dup
 * @return char* -> New dupped memory block
 */
CBUILDDEF void* cbuild_temp_memdup(const void* mem, size_t size);
/**
 * @brief Deallocate all temporary allocations
 *
 * @param checkpoint => size_t  -> Either a temp allocator checkpoint
 * or 0 to fully reset
 */
CBUILDDEF void cbuild_temp_reset(size_t checkpoint);
/**
 * @brief Create checkpoint for temporary allocator.
 * It can be restored to this state.
 *
 * @return size_t -> Temp allocator checkpoint
 */
CBUILDDEF size_t cbuild_temp_checkpoint(void);
/* DynArray.h */
#define cbuild_da_new(type) \
	struct {                  \
		type* data;             \
		size_t size;            \
		size_t capacity;        \
	}
/**
 *  @brief Append element to a dynamic array
 *
 *  @param da => CBUILD_DA* -> Dynamic array. Will be evaluated multiple times.
 *  @param elem => VAL -> Element
 */
#define cbuild_da_append(da, elem)                                             \
	do {                                                                         \
		if (((da)->size + 1) > (da)->capacity) {                                   \
			cbuild_da_resize((da), 0);                                               \
		}                                                                          \
		(da)->data[(da)->size++] = elem;                                           \
	} while(0)
/**
 * @brief Append an array to a da
 *
 * @param da => CBUILD_DA* -> Dynamic array. Will be evaluated multiple times.
 * @param arr => VAL* -> Array pointer
 * @param arr_size => size_t -> Number of new elements
 */
#define cbuild_da_append_arr(da, arr, arr_size)                                \
	do {                                                                         \
		if (((da)->size + (arr_size)) > (da)->capacity) {                          \
			cbuild_da_resize((da), (da)->capacity + (arr_size));                     \
		}                                                                          \
		memcpy((da)->data + (da)->size, (arr),                                     \
			(arr_size) * sizeof(typeof(*(da)->data)));                               \
		(da)->size += (arr_size);                                                  \
	} while(0)
/**
 * @brief Append multiple elements to a da
 *
 * @param da => CBUILD_DA* -> Dynamic array. Will be evaluated multiple times.
 * @param ... => VAL -> New elements
 */
#define cbuild_da_append_many(da, ...)                                         \
	do {                                                                         \
		typeof(*((da)->data))   __cbuild_da_tmp_arr[] = { __VA_ARGS__ };           \
		size_t __cbuild_da_tmp_count =                                             \
			sizeof(__cbuild_da_tmp_arr) / sizeof(__cbuild_da_tmp_arr[0]);            \
		cbuild_da_append_arr((da), __cbuild_da_tmp_arr, __cbuild_da_tmp_count);    \
	} while (0)
/**
 * @brief Set da element using its index
 *
 * @param da => CBUILD_DA* -> Dynamic array. Will be evaluated multiple times.
 * @param idx => size_t -> Index
 * @param elem => VAL -> Element
 * @return bool -> Success or failure on overflow
 */
#define cbuild_da_set(da, idx, elem)                                           \
	({                                                                           \
		bool __cbuild__ret = false;                                                \
		if ((idx) >= (da)->size) {                                                  \
			cbuild_log(CBUILD_LOG_ERROR, "(LIB_CBUILD_DA) Index overflow in set.");  \
			__cbuild__ret = false;                                                   \
		} else {                                                                   \
			(da)->data[(idx)] = elem;                                                 \
			__cbuild__ret = true;                                                    \
		}                                                                          \
		__cbuild__ret;                                                             \
	})
/**
 * @brief Get an element from a da using index
 *
 * @param da => CBUILD_DA -> Dynamic array. Will be evaluated multiple times.
 * @param idx => size_t -> Element index
 * @return VAL* -> Element or NULL on overflow
 */
#define cbuild_da_get(da, idx)                                                 \
	({                                                                           \
		typeof(*(da).data)* __cbuild__ret = NULL;                                  \
		if ((idx) >= (da).size) {                                                  \
			cbuild_log(CBUILD_LOG_ERROR, "(LIB_CBUILD_DA) Index overflow in get.");  \
			__cbuild__ret = NULL;                                                    \
		} else {                                                                   \
			__cbuild__ret = &((da).data[(idx)]);                                     \
		}                                                                          \
		__cbuild__ret;                                                             \
	})
/**
 * @brief Remove an element from a da using index
 *
 * @param da => CBUILD_DA* -> Dynamic array. Will be evaluated multiple times.
 * @param idx => size_t -> Element index
 * @return bool -> Success or failure on overflow
 */
#define cbuild_da_remove(da, idx)                                              \
	({                                                                           \
		bool __cbuild__ret = false;                                                \
		if ((idx) >= (da)->size) {                                                 \
			cbuild_log(CBUILD_LOG_ERROR,                                             \
				"(LIB_CBUILD_DA) Index overflow in remove.");                          \
			__cbuild__ret = false;                                                   \
		} else {                                                                   \
			memmove((da)->data + (idx), ((da)->data + ((idx) + 1)),                  \
				((da)->size - ((idx) + 1)) * sizeof(typeof(*(da)->data)));             \
			(da)->size--;                                                            \
			__cbuild__ret = true;                                                    \
		}                                                                          \
		__cbuild__ret;                                                             \
	})
/**
 * @brief Remove an element from a da using index
 * O(1) but order is changed
 * @param da => CBUILD_DA* -> Dynamic array. Will be evaluated multiple times.
 * @param idx => size_t -> Element index
 * @return bool -> Success or failure on overflow
 */
#define cbuild_da_remove_unordered(da, idx)                                    \
	({                                                                           \
		bool __cbuild__ret = false;                                                \
		if ((idx) >= (da)->size) {                                                 \
			cbuild_log(CBUILD_LOG_ERROR,                                             \
				"(LIB_CBUILD_DA) Index overflow in remove.");                          \
			__cbuild__ret = false;                                                   \
		} else {                                                                   \
			(da)->data[(idx)] = (da)->data[--(da)->size];                            \
			__cbuild__ret = true;                                                    \
		}                                                                          \
		__cbuild__ret;                                                             \
	})
/**
 * @brief Resize da (done automatically most of the time ;) )
 *
 * @param da => CBUILD_DA* -> Dynamic array. Will be evaluated multiple times.
 * @param new_size => size_t -> New element count. Array will be truncated if it
 * will be lover than da->size. If zero then default growth behavior is used
 */
#define cbuild_da_resize(da, new_size)                                         \
	do {                                                                         \
		if ((da)->data == NULL) {                                                  \
			if ((new_size) == 0) {                                                   \
				(da)->capacity = CBUILD_INIT_CAPACITY;                                 \
			} else {                                                                 \
				(da)->capacity = (new_size);                                           \
			}                                                                        \
			(da)->data = cbuild_malloc(                                              \
				(da)->capacity * sizeof(typeof(*(da)->data)));                         \
		} else {                                                                   \
			if ((new_size) == 0) {                                                   \
				(da)->capacity *= 2;                                                   \
			} else {                                                                 \
				(da)->capacity = (new_size);                                           \
			}                                                                        \
			(da)->data = cbuild_realloc((da)->data,                                  \
				(da)->capacity * sizeof(typeof(*(da)->data)));                         \
		}                                                                          \
		cbuild_assert((da)->data != NULL, "(LIB_CBUILD_DA) Allocation failed.\n"); \
		((da)->size > (da)->capacity) ? ((da)->size = (da)->capacity) : 0;         \
	} while(0)
/**
 * @brief Free da
 *
 * @param da => CBUILD_DA* -> Dynamic array. Will be evaluated multiple times.
 */
#define cbuild_da_clear(da)                                                    \
	do {                                                                         \
		cbuild_free((da)->data);                                                   \
		(da)->data     = NULL;                                                     \
		(da)->size     = 0;                                                        \
		(da)->capacity = 0;                                                        \
	} while(0)
/**
 * @brief Foreach loop
 *
 * @param da => CBUILD_DA -> Dynamic array. Will be evaluated multiple times.
 * @param iter => NAME -> Iteration value name
 */
#define cbuild_da_foreach(da, iter)                                            \
	for (typeof(*((da).data))* iter = (da).data;                                 \
		iter < ((da).data + (da).size); iter++)
/* StringView.h */
typedef struct cbuild_sv_t {
	char*  data;
	size_t size;
} cbuild_sv_t;
#define CBuildSVFmt     "%.*s"
#define CBuildSVArg(sv) (int)(sv).size, (sv).data
/**
 * @brief Create string view
 *
 * @param data => const char* -> Char array
 * @param size => size_t -> Length of a view
 * @return cbuild_sv_t -> New string view
 */
#define cbuild_sv_from_parts(_data, _size)                                     \
((cbuild_sv_t){ .data = (char*)(_data), .size = (_size) })
/**
 * @brief Create string view from c string
 *
 * @param cstr => const char* -> C-string. Will be evaluated twice.
 * @return cbuild_sv_t -> New string view
 */
#define cbuild_sv_from_cstr(cstr)                                              \
((cbuild_sv_t){ .data = (char*)(cstr), .size = strlen((cstr)) })
/**
 * @brief Create string view from c string (compile-time literal)
 *
 * @param lit => const char* -> C-string (compile-time literal).
 * Will be evaluated twice.
 * @return cbuild_sv_t -> New string view
 */
#define cbuild_sv_from_lit(lit)                                                \
((cbuild_sv_t){ .data = (char*)(lit), .size = sizeof(lit) - 1 })
/**
 * @brief Trim whitespaces from a string view starting from the left
 *
 * @param sv => cbuild_sv_t* -> String view to work with
 * @return size_t -> Number of characters removed
 */
CBUILDDEF size_t cbuild_sv_trim_left(cbuild_sv_t* sv);
/**
 * @brief Trim whitespaces from a string view starting from the right
 *
 * @param sv => cbuild_sv_t* -> String view to work with
 * @return size_t -> Number of characters removed
 */
CBUILDDEF size_t cbuild_sv_trim_right(cbuild_sv_t* sv);
/**
 * @brief Trim whitespaces from a string view starting from both sides
 *
 * @param sv => cbuild_sv_t* -> String view to work with
 * @return size_t -> Number of characters removed
 */
CBUILDDEF size_t cbuild_sv_trim(cbuild_sv_t* sv);
/**
 * @brief Chop characters from one string view into another
 * Note: Original string view will be truncated
 * @param sv => cbuild_sv_t* -> String view to work with
 * @param size => size_t -> Number of characters to chop
 * @return cbuild_sv_t -> New string view
 */
CBUILDDEF cbuild_sv_t cbuild_sv_chop(cbuild_sv_t* sv, size_t size);
/**
 * @brief Chop characters from one string view into another
 * Note: Original string view will be truncated. delim will be removed from both
 * string views
 * @param sv => cbuild_sv_t* -> String view to work with
 * @param delim => char -> Character on which function stops.
 * @return cbuild_sv_t -> New string view
 */
CBUILDDEF cbuild_sv_t cbuild_sv_chop_by_delim(cbuild_sv_t* sv, char delim);
/**
 * @brief Chop characters from one string view into another
 * Note: Original string view will be truncated. delim will be removed from both
 * string views
 * @param sv => cbuild_sv_t* -> String view to work with
 * @param delim => cbuild_sv_t -> Character on which function stops.
 * @return cbuild_sv_t -> New string view
 */
CBUILDDEF cbuild_sv_t cbuild_sv_chop_by_sv(cbuild_sv_t* sv, cbuild_sv_t delim);
/**
 * @brief Delimiter func for sv_chop_by_func
 *
 * @param sv => const cbuild_sv_t* -> String view currently being chopped
 * @param idx => size_t -> Current scan index
 * @param args => void* -> Some user arguments
 * @return bool -> True if this character should be marked as a delimiter
 */
typedef bool (*cbuild_sv_delim_func)(const cbuild_sv_t* sv, size_t idx,
	void* args);
/**
 * @brief Chop characters from one string view into another
 * Note: Original string view will be truncated. Index which function marked as
 * an end will not be in any string view string views.
 * @param sv => cbuild_sv_t* -> String view to work with
 * @param delim => cbuild_sv_delim_func -> Delimiter scanner
 * @param args => void* -> Will be passed into delim function
 * @return cbuild_sv_t -> New string view
 */
CBUILDDEF cbuild_sv_t cbuild_sv_chop_by_func(cbuild_sv_t* sv,
	cbuild_sv_delim_func delim, void* args);
/**
 * @brief strcmp for string view
 *
 * @param a => cbuild_sv_t -> First string view
 * @param b => cbuild_sv_t -> Second string view
 * @return -2 -> If size of first string view is smaller
 * @return -1 -> If first different character in first string view is smaller
 * @return 0  -> If two string views are equal
 * @return 1  -> If first different character in first string view is larger
 * @return 2  -> If size of first string view is larger
 */
CBUILDDEF int cbuild_sv_cmp(cbuild_sv_t a, cbuild_sv_t b);
/**
 * @brief compare two string view ignoring case of a latin letters for string
 * view
 *
 * @param a => cbuild_sv_t -> First string view
 * @param b => cbuild_sv_t -> Second string view
 * @return -2 -> If size of first string view is smaller
 * @return -1 -> If first different character in first string view is smaller
 * @return 0  -> If two string views are equal
 * @return 1  -> If first different character in first string view is larger
 * @return 2  -> If size of first string view is larger
 */
CBUILDDEF int cbuild_sv_cmp_icase(cbuild_sv_t a, cbuild_sv_t b);
/**
 * @brief Check if string view has specific prefix
 *
 * @param sv => cbuild_sv_t -> String view to work with
 * @param prefix => cbuild_sv_t -> Prefix
 * @return bool -> True if sv contains prefix
 */
CBUILDDEF bool cbuild_sv_prefix(cbuild_sv_t sv, cbuild_sv_t prefix);
/**
 * @brief Check if string string view has specific suffix
 *
 * @param sv => cbuild_sv_t -> String view to work with
 * @param suffix => cbuild_sv_t -> Prefix
 * @return bool -> True if cbuild_sv contains suffix
 */
CBUILDDEF bool cbuild_sv_suffix(cbuild_sv_t sv, cbuild_sv_t suffix);
/**
 * @brief Find characte in a string view
 *
 * @param sv => cbuild_sv_t -> String view to work with
 * @param c => char -> Character to look for
 * @return ssize_t -> Index of a character or -1
 */
CBUILDDEF ssize_t cbuild_sv_find(cbuild_sv_t sv, char c);
/**
 * @brief Find string view in another string view
 *
 * @param sv => cbuild_sv_t -> String view to work with
 * @param needle => cbuild_sv_t -> String view to look for
 * @return ssize_t -> Index of a character or -1
 */
CBUILDDEF ssize_t cbuild_sv_find_sv(cbuild_sv_t sv, cbuild_sv_t needle);
/**
 * @brief Find char in a string view starting from the back
 *
 * @param sv => cbuild_sv_t -> String view to work with
 * @param c => char -> Character to look for
 * @return ssize_t -> Index of a character or -1
 */
CBUILDDEF ssize_t cbuild_sv_rfind(cbuild_sv_t sv, char c);
/**
 * @brief Check if string view contains the cracter
 *
 * @param sv => cbuild_sv_t -> String view to work with
 * @param c => char -> Character to look for
 * @return bool -> Contains or not
 */
CBUILDDEF bool cbuild_sv_contains(cbuild_sv_t sv, char c);
/**
 * @brief Check if string view contains another string view
 *
 * @param sv => cbuild_sv_t -> String view to work with
 * @param needle => cbuild_sv_t -> String view to look for
 * @return bool -> Contains or not
 */
CBUILDDEF bool cbuild_sv_contains_sv(cbuild_sv_t sv, cbuild_sv_t needle);
/**
 * @brief	Get length of first utf8 codepoint in strting view
 *
 * @param sv => cbuild_sv_t -> String view to work with
 * @return Size of codepoint in bytes. 0 if size of string view is 0.
 * 1 If codepoint is invalid.
 */
CBUILDDEF int cbuild_sv_utf8cp_len(cbuild_sv_t sv);
/**
 * @brief strchr for string view
 *
 * @param sv => cbuild_sv_t -> Strinv view to work with
 * @param c => char -> Char to find
 * @return char* -> Pointer to a char inside of a string view or NULL if not
 * found
 */
CBUILDDEF char* cbuild_sv_strchr(cbuild_sv_t sv, char c);
/**
 * @brief strchr for string view with utf8 support
 *
 * @param sv => cbuild_sv_t -> Strinv view to work with
 * @param c => char -> Char to find
 * @return char* -> Pointer to a char inside of a string view or NULL if not
 * found
 */
CBUILDDEF char* cbuild_sv_utf8chr(cbuild_sv_t sv, uint32_t c);
/**
 * @brief Chop utf8 codepoint from a string view
 * Note: Original string view will be truncated
 * @param sv => scbuild_sv_t* -> String view to work with
 * @return uint32_t -> Unicode codepoint, UINT32_MAX means error
 */
CBUILDDEF uint32_t cbuild_sv_chop_utf8(cbuild_sv_t* sv);
/**
 * @brief Chop string view using unicode codepoint as delimeter
 * Note: Original string view will be truncated
 * @param sv => cbuild_sv_t* ->	String view to work with
 * @param delim => uint32_t -> Delimeter codepoint
 * @return cbuild_sv_t => New string view
 */
CBUILDDEF cbuild_sv_t cbuild_sv_chop_by_utf8(cbuild_sv_t* sv, uint32_t delim);
/**
 * @brief Delimiter func for sv_chop_by_func_utf8
 *
 * @param sv => const cbuild_sv_t* -> String view currently being chopped,
 * already prechopped
 * @param args => void* -> Some user arguments
 * @return bool -> True if this character should be marked as a delimiter
 */
typedef bool (*cbuild_sv_utf8delim_func)(const cbuild_sv_t* sv, void* args);
/**
 * @brief Chop characters from one string view into another, for utf8 content
 * Note: Original string view will be truncated. Index which function marked as
 * an end will not be in any string view string views.
 * @param sv => cbuild_sv_t* -> String view to work with
 * @param delim => cbuild_sv_delim_func -> Delimiter scanner
 * @param args => void* -> Will be passed into delim function
 * @return cbuild_sv_t -> New string view
 */
CBUILDDEF cbuild_sv_t cbuild_sv_chop_by_func_utf8(cbuild_sv_t* sv,
	cbuild_sv_utf8delim_func delim, void* args);
/**
 * @brief strcmp for string view with utf8 content
 *
 * @param a => cbuild_sv_t -> First string view
 * @param b => cbuild_sv_t -> Second string view
 * @return -2 -> If size of first string view is smaller
 * @return -1 -> If first different character in first string view is smaller
 * @return 0  -> If two string views are equal
 * @return 1  -> If first different character in first string view is larger
 * @return 2  -> If size of first string view is larger
 */
CBUILD_DEPRECATED("Please use cbuild_sv_cmp instead!",
	CBUILDDEF int cbuild_sv_utf8cmp(cbuild_sv_t a, cbuild_sv_t b));
/**
 * @brief Get lengths of a string view with utf8 content
 *
 * @param sv => cbuild_sv_t -> String view to work with
 * @return size_t -> Number of encoded utf8 codepoints
 */
CBUILDDEF size_t cbuild_sv_utf8len(cbuild_sv_t sv);
/**
 * @brief Validate string as utf8-based
 *
 * @param idx => size_t* -> If non-NULL here will be stored	index of first
 * invalid codepoint, if validation failed.
 * @return bool -> true if valid, false otherwise
 */
CBUILDDEF bool cbuild_sv_utf8valid(cbuild_sv_t sv, size_t* idx);
/**
 * @brief Convert string view to a c-style string
 *
 * @param sv => cbuild_sv_t -> String view to convert
 * @return char* -> C-style string. Allocated with malloc
 */
CBUILDDEF char* cbuild_sv_to_cstr(cbuild_sv_t sv);
/**
 * @brief Convert string view to a c-style string
 *
 * @param sv => cbuild_sv_t -> String view to convert
 * @return char* -> C-style string. Allocated with malloc
 */
#define cbuild_cstr_from_sv(sv) cbuild_sv_to_cstr(sv)
/**
 * @brief Convert string view to a c-style string
 *
 * @param sv => cbuild_sv_t -> String view to convert
 * @return char* -> C-style string. Allocated with cbuild_temp_alloc
 */
CBUILDDEF char* cbuild_temp_sv_to_cstr(cbuild_sv_t sv);
/**
 * @brief Convert string view to a c-style string
 *
 * @param sv => cbuild_sv_t -> String view to convert
 * @return char* -> C-style string. Allocated with cbuild_temp_alloc
 */
#define cbuild_temp_cstr_from_sv(sv) cbuild_temp_sv_to_cstr(sv)
/* StringBuilder.h */
#define CBuildSBFmt     "%.*s"
#define CBuildSBArg(sb) (int)(sb).size, (sb).data
typedef struct cbuild_sb_t {
	char* data;
	size_t size;
	size_t capacity;
} cbuild_sb_t;
/**
 *  @brief Append character to a string builder
 *
 *  @param sb => cbuild_sb_t* -> String builder. Will be evaluated multiple times.
 *  @param elem => char -> Character
 */
#define cbuild_sb_append(sb, elem) cbuild_da_append((sb), (elem))
/**
 * @brief Append an array to a sb
 *
 * @param sb => cbuild_sb_t* -> String builder. Will be evaluated multiple times.
 * @param arr => char* -> Array pointer
 * @param size => size_t -> Number of new elements
 */
#define cbuild_sb_append_arr(sb, arr, size)                                    \
	cbuild_da_append_arr((sb), (arr), (size))
/**
 * @brief Append list of chars to a string builder
 *
 * @param sb => cbuild_sb_t* -> String builder. Will be evaluated multiple times. to operate on
 * @param ... => char ... -> Chars that need to be inserted
 */
#define cbuild_sb_append_many(sb, ...)                                         \
	cbuild_da_append_many((sb), __VA_ARGS__)
/**
 * @brief Append a C-string to a sb
 *
 * @param sb => cbuild_sb_t* -> String builder. Will be evaluated multiple times.
 * @param cstr => char* -> C-string
 */
#define cbuild_sb_append_cstr(sb, cstr)                                        \
	cbuild_da_append_arr((sb), (cstr), strlen(cstr))
/**
 * @brief Append NULL-terminator to a string builder
 *
 * @param sb => cbuild_sb_t* -> String builder. Will be evaluated multiple times.
 */
#define cbuild_sb_append_null(sb) cbuild_da_append((sb), '\0')
/**
 * @brief Set a character in a sb using index
 *
 * @param sb => cbuild_sb_t -> String builder. Will be evaluated multiple times.
 * @param idx => size_t -> Character index
 * @param elem => char -> New character
 */
#define cbuild_sb_set(sb, idx, elem) cbuild_da_set((sb), (idx), (elem))
/**
 * @brief Get a character from a sb using index
 *
 * @param sb => cbuild_sb_t -> String builder. Will be evaluated multiple times.
 * @param idx => size_t -> Character index
 * @return VAL* -> Element
 */
#define cbuild_sb_get(sb, idx) cbuild_da_get((sb), (idx))
/**
 * @brief Remove an element from a sb using index
 *
 * @param sb => cbuild_sb_t* -> Dynamic array
 * @param idx => size_t -> Element index
 */
#define cbuild_sb_remove(sb, idx) cbuild_da_remove((sb), (idx))
/**
 * @brief Resize sb (Done automatically most of the times ;) )
 *
 * @param sb => cbuild_sb_t* -> String builder. Will be evaluated multiple times.
 * @param size => size_t -> New element count. String will be truncated if it
 * will be lover than sb->size. If it is zero then default behavior is used.
 */
#define cbuild_sb_resize(sb, new_capacity)                                     \
	cbuild_da_resize((sb), new_capacity)
/**
 * @brief Free sb
 *
 * @param sb => cbuild_sb_t* -> String builder. Will be evaluated multiple times.
 */
#define cbuild_sb_clear(sb) cbuild_da_clear(sb)
/**
 * @brief strcmp for string builder
 *
 * @param a => cbuild_sb_t* -> First string buffer
 * @param b => cbuild_sb_t* -> Second String buffer
 * @return -2 -> If size of first string builder is smaller
 * @return -1 -> If first different character in first string builder is smaller
 * @return 0  -> If two string builders are equal
 * @return 1  -> If first different character in first string builder is larger
 * @return 2  -> If size of first string builder is larger
 */
CBUILDDEF int cbuild_sb_cmp(cbuild_sb_t a, cbuild_sb_t b);
/**
 * @brief Compare two string builders ignoring case of an ASCII letter (Latin
 * only)
 *
 * @param a => cbuild_sb_t -> First string builder
 * @param b => cbuild_sb_t -> Second string builder
 * @return -2 -> If size of first string builder is smaller
 * @return -1 -> If first different character in first StringBuiler is smaller
 * @return 0  -> If two string builders are equal
 * @return 1  -> If first different character in first string builder is larger
 * @return 2  -> If size of first string builder is larger
 */
CBUILDDEF int cbuild_sb_cmp_icase(cbuild_sb_t a, cbuild_sb_t b);
/**
 * @brief Convert string builder to string view
 *
 * @param sb => cbuild_sb_t -> String builder
 * @return cbuild_sv_t -> New string view
 */
CBUILDDEF cbuild_sv_t cbuild_sb_to_sv(cbuild_sb_t sb);
/**
 * @brief Convert string builder to string view
 *
 * @param sb => cbuild_sb_t -> String builder
 * @return cbuild_sv_t -> New string view
 */
#define	cbuild_sv_from_sb(sb) cbuild_sb_to_sv(sb)
/**
 * @brief Convert string view to a string builder. Does a copy.
 *
 * @param sv => cbuild_sv_t -> String view
 * @return cbuild_sb_t -> New string builder
 */
CBUILDDEF cbuild_sb_t cbuild_sv_to_sb(cbuild_sv_t sv);
/**
 * @brief Convert string view to a string builder. Does a copy.
 *
 * @param sv => cbuild_sv_t -> String view
 * @return cbuild_sb_t -> New string builder
 */
#define cbuild_sb_from_sv(sv) cbuild_sv_to_sb(sv)
/**
 * @brief Append string view to a string builder
 *
 * @param sb => CBUILD_SB* -> String builder. Will be evaluated multiple times.
 * @param sv => cbuild_sv_t -> String view. Will be evaluated twice
 */
#define cbuild_sb_append_sv(sb, sv)                                            \
	cbuild_sb_append_arr((sb), (sv).data, (sv).size)
/**
 * @brief vsprintf for a string builder
 *
 * @param sb => cbuild_sb_t* -> String builder
 * @param fmt => const char* -> Format string
 * @param args => va_list -> Variadic arguments
 * @return int -> Number of bytes printed or -1 on error
 */
CBUILDDEF int cbuild_sb_vappendf(cbuild_sb_t* sb, const char* fmt, va_list args);
/**
 * @brief sprintf for a string builder
 *
 * @param sb => cbuild_sb_t* -> String builder
 * @param fmt => const char* -> Format string
 * @param ... => ... -> Variadic arguments
 * @return int -> Number of bytes printed or -1 on error
 */
CBUILDDEF int cbuild_sb_appendf(cbuild_sb_t* sb, const char* fmt, ...)
__attribute__((format(printf, 2, 3)));
/**
 * @brief Foreach loop
 *
 * @param sb => cbuild_sb_t -> String builder. Will be evaluated multiple times.
 * @param iter => NAME -> Iteration value name
 */
#define cbuild_sb_foreach(sb, iter) cbuild_da_foreach(sb, iter)
/**
 * @brief Append unicode codepoint to string builder. Encoding will be utf8
 *
 * @param sb => cbuild_sb_t* -> String builder
 * @param cp => uint32_t -> Codepoint
 */
CBUILDDEF void cbuild_sb_append_utf8(cbuild_sb_t* sb, uint32_t cp);
/**
 * @brief strcmp for string builder encoded as utf8.
 * Will use cbuild_sb_utf8cmp under the hood
 * @param a => cbuild_sb_t -> First string buffer
 * @param b => cbuild_sb_t -> Second String buffer
 * @return -2 -> If size of first string builder is smaller
 * @return -1 -> If first different character in first string builder is smaller
 * @return 0  -> If two string builders are equal
 * @return 1  -> If first different character in first string builder is larger
 * @return 2  -> If size of first string builder is larger
 */
CBUILD_DEPRECATED("Please use cbuild_sb_cmp instead!",
	CBUILDDEF int cbuild_sb_utf8cmp(cbuild_sb_t a, cbuild_sb_t b));
/**
 * @brief Get lengths of a string builder with utf8 content
 *
 * @param sv => cbuild_sb_t -> String builder
 * @return size_t -> Number of encoded utf8 codepoints
 */
CBUILDDEF size_t cbuild_sb_utf8len(cbuild_sb_t sb);
/* Map.h */
typedef struct cbuild_map_bucket_t {
	void* vals;
	size_t nvals;
} cbuild_map_bucket_t;
typedef size_t (*cbuild_map_hash_t)(const void* map, const void* key);
typedef bool (*cbuild_map_keycmp_t)(const void* map, const void* k1,
	const void* k2);
typedef void (*cbuild_map_elem_clear_t)(const void* map, void* elem);
/**
 * @brief Main map datatype
 * User shoudl set elem_size or key_size. Elem size is a size of a full elem
 * struct - key + value. Key size is a size of a key only. If 0 key is assumed
 * to be a 'char*' pointer to a c-string.
 * Hash and key compare function can be overwrittn. Default support any inline
 * keys (number, array, etc. uses memcmp and hash on a raw block of bytes) and
 * c-strings.
 */
typedef struct cbuild_map_t {
	cbuild_map_bucket_t* buckets;
	size_t nbuckets;
	cbuild_map_hash_t hash_func;
	cbuild_map_keycmp_t keycmp_func;
	size_t key_size;
	size_t elem_size;
	size_t iter_buckets;
	size_t iter_vals;
} cbuild_map_t;
/**
 * @brief Init a map to specifc capacity. Do not rehash.
 *
 * @param map => cbuild_map_t* -> Map object
 * @param nbuckets => size_t -> New bucket count
 */
CBUILDDEF void cbuild_map_init(cbuild_map_t* map, size_t nbuckets);
/**
 * @brief Get element from a map
 *
 * @param map => cbuild_map_t* -> Map object
 * @param key => void* -> Requested key
 * @return void* -> Element of NULL if not found
 */
CBUILDDEF void* cbuild_map_get_raw(const cbuild_map_t* map, const void* key);
/**
 * @brief Get element from a map
 *
 * @param map => cbuild_map_t* -> Map object. Will be evaluated multiple times.
 * @param key => T -> Requested key
 * @return void* -> Element of NULL if not found
 */
#define cbuild_map_get(map, key)                                               \
	({                                                                           \
		__auto_type __cbuild_tmp_key = key;                                        \
		cbuild_assert(sizeof(__cbuild_tmp_key) == (map)->key_size ||               \
			((map)->key_size == 0 && sizeof(__cbuild_tmp_key) == sizeof(char*)),     \
			"(LIB_CBUILD_MAP) Key type mismatch!\n");                                \
		cbuild_map_get_raw((map), &__cbuild_tmp_key);                              \
	})
/**
 * @brief Get element from a map
 *
 * @param map => cbuild_map_t* -> Map object. Will be evaluated multiple times.
 * @param key => T* -> Requested key
 * @return void* -> Element of NULL if not found
 */
#define cbuild_map_get_ptr(map, key)                                           \
	({                                                                           \
		__auto_type __cbuild_tmp_key = key;                                        \
		cbuild_assert(sizeof(*__cbuild_tmp_key) == (map)->key_size ||              \
			((map)->key_size == 0 && sizeof(*__cbuild_tmp_key) == sizeof(char*)),    \
			"(LIB_CBUILD_MAP) Key type mismatch!\n");                                \
		cbuild_map_get_raw((map), __cbuild_tmp_key);                               \
	})
/**
 * @brief Get element from a map or allocate new
 *
 * @param map => cbuild_map_t* -> Map object
 * @param key => void* -> Requested key
 * @return void* -> Element
 */
CBUILDDEF void* cbuild_map_get_or_alloc_raw(cbuild_map_t* map, const void* key);
/**
 * @brief Get element from a map or allocate new
 *
 * @param map => cbuild_map_t* -> Map object. Will be evaluated multiple times.
 * @param key => T -> Requested key
 * @return void* -> Element
 */
#define cbuild_map_get_or_alloc(map, key)                                      \
	({                                                                           \
		__auto_type __cbuild_tmp_key = key;                                        \
		cbuild_assert(sizeof(__cbuild_tmp_key) == (map)->key_size ||               \
			((map)->key_size == 0 && sizeof(__cbuild_tmp_key) == sizeof(char*)),     \
			"(LIB_CBUILD_MAP) Key type mismatch!\n");                                \
		cbuild_map_get_or_alloc_raw((map), &__cbuild_tmp_key);                     \
	})
/**
 * @brief Get element from a map or allocate new
 *
 * @param map => cbuild_map_t* -> Map object. Will be evaluated multiple times.
 * @param key => T* -> Requested key
 * @return void* -> Element
 */
#define cbuild_map_get_or_alloc_ptr(map, key)                                  \
	({                                                                           \
		__auto_type __cbuild_tmp_key = key;                                        \
		cbuild_assert(sizeof(*__cbuild_tmp_key) == (map)->key_size ||              \
			((map)->key_size == 0 && sizeof(*__cbuild_tmp_key) == sizeof(char*)),    \
			"(LIB_CBUILD_MAP) Key type mismatch!\n");                                \
		cbuild_map_get_or_alloc_raw((map), __cbuild_tmp_key);                      \
	})
/**
 * @brief Remove element from a map. Userdata should be memcpy-safe.
 *
 * @param map => cbuild_map_t* -> Map object
 * @param key => void* -> Requested key
 * @param elem_clear_func => cbuild_map_elem_clear_t -> Function that will clear
 * element. Can be NULL.
 * @return bool -> False if key not found
 */
CBUILDDEF bool cbuild_map_remove_ex_raw(cbuild_map_t* map, const void* key,
	cbuild_map_elem_clear_t elem_clear_func);
/**
 * @brief Remove element from a map. Userdata should be memcpy-safe.
 *
 * @param map => cbuild_map_t* -> Map object. Will be evaluated multiple times.
 * @param key => T -> Requested key
 * @param elem_clear_func => cbuild_map_elem_clear_t -> Function that will clear
 * element. Can be NULL.
 * @return bool -> False if key not found
 */
#define cbuild_map_remove_ex(map, key, elem_clear_func)                        \
	({                                                                           \
		__auto_type __cbuild_tmp_key = key;                                        \
		cbuild_assert(sizeof(__cbuild_tmp_key) == (map)->key_size ||               \
			((map)->key_size == 0 && sizeof(__cbuild_tmp_key) == sizeof(char*)),     \
			"(LIB_CBUILD_MAP) Key type mismatch!\n");                                \
		cbuild_map_remove_ex_raw((map), &__cbuild_tmp_key, elem_clear_func);       \
	})
/**
 * @brief Remove element from a map. Userdata should be memcpy-safe.
 *
 * @param map => cbuild_map_t* -> Map object. Will be evaluated multiple times.
 * @param key => T* -> Requested key
 * @param elem_clear_func => cbuild_map_elem_clear_t -> Function that will clear
 * element. Can be NULL.
 * @return bool -> False if key not found
 */
#define cbuild_map_remove_ex_ptr(map, key, elem_clear_func)                    \
	({                                                                           \
		__auto_type __cbuild_tmp_key = key;                                        \
		cbuild_assert(sizeof(*__cbuild_tmp_key) == (map)->key_size ||              \
			((map)->key_size == 0 && sizeof(*__cbuild_tmp_key) == sizeof(char*)),    \
			"(LIB_CBUILD_MAP) Key type mismatch!\n");                                \
		cbuild_map_remove_ex_raw((map), __cbuild_tmp_key, elem_clear_func);        \
	})
/**
 * @brief Remove element from a map. Userdata should be memcpy-safe.
 *
 * @param map => cbuild_map_t* -> Map object
 * @param key => void* -> Requested key
 * @return bool -> False if key not found
 */
#define cbuild_map_remove_raw(map, key) cbuild_map_remove_ex_raw(map, key, NULL)
/**
 * @brief Remove element from a map. Userdata should be memcpy-safe.
 *
 * @param map => cbuild_map_t* -> Map object. Will be evaluated multiple times.
 * @param key => T -> Requested key
 * @return bool -> False if key not found
 */
#define cbuild_map_remove(map, key) cbuild_map_remove_ex(map, key, NULL)
/**
 * @brief Remove element from a map. Userdata should be memcpy-safe.
 *
 * @param map => cbuild_map_t* -> Map object. Will be evaluated multiple times.
 * @param key => T* -> Requested key
 * @return bool -> False if key not found
 */
#define cbuild_map_remove_ptr(map, key) cbuild_map_remove_ex_ptr(map, key, NULL)
/**
 * @brief Fully clear and deallocate map
 *
 * @param map => cbuild_map_t* -> Map object
 * @param elem_cear_func => cbuild_map_elem_clear_t -> Function that will clear
 * element. Can be called on all allocated elements. Can be NULL.
 */
CBUILDDEF void cbuild_map_clear_ex(cbuild_map_t* map,
	cbuild_map_elem_clear_t elem_clear_func);
/**
 * @brief Fully clear and deallocate map
 *
 * @param map => cbuild_map_t* -> Map object
 */
#define cbuild_map_clear(map) cbuild_map_clear_ex(map, NULL)
/**
 * @brief Reset map iterator
 *
 * @param map => cbuild_map_t -> Map object
 */
CBUILDDEF void cbuild_map_iter_reset(cbuild_map_t* map);
/**
 * @brief Get next map element
 * Note: Removing invalidates iterator (undefined behaviour if iterator is used
 * after remove without explicit reset)
 * Note: Map element allocations do not invalidate iterator, but it is undefined
 * behaviour if new element will be iterated or not (it may be iterated, or may
 * be not iterated)
 * @param map => cbuild_map_t -> Map object
 * @return void* -> Element pointer or NULL if end reached
 */
CBUILDDEF void* cbuild_map_iter_next(cbuild_map_t* map);
/**
 * @brief For-each loop over a map
 *
 * @param map => cbuild_map_t* -> Map object. Will be evaluated multiple times.
 * @param iter => NAME -> Name of the iteration variable (void*)
 */
#define	cbuild_map_foreach_raw(map, iter)                                      \
	cbuild_map_iter_reset(map);                                                  \
	for (void* iter = NULL; ((iter) = cbuild_map_iter_next(map));)
/**
 * @brief For-each loop over a map
 *
 * @param map => cbuild_map_t* -> Map object. Will be evaluated multiple times.
 * @param T => TYPE -> Type map userdata
 * @param iter => NAME -> Name of the iteration variable (T*)
 */
#define cbuild_map_foreach(map, T, iter)                                       \
	cbuild_map_iter_reset(map);                                                  \
	for (T* iter = NULL;((iter) = (T*)cbuild_map_iter_next(map));)
/* Stack.h */
/**
 * @brief Push element to a stack
 *
 * @param stack => CBUILD_STACK* -> Stack. Will be evaluated twice.
 * @param elem => VAL* -> Element
 */
#define cbuild_stack_push(stack, elem)                                         \
	do {                                                                         \
		if ((stack)->ptr >= (stack)->capacity) {                                   \
			cbuild_stack_resize((stack), 0);                                         \
		}                                                                          \
		(stack)->data[(stack)->ptr++] = elem;                                      \
	} while(0)
/**
 * @brief Pop element from a stack
 *
 * @param stack => CBUILD_STACK* -> Stack. Will be evaluated multiple times.
 * @return VAL -> Element
 */
#define cbuild_stack_pop(stack)                                                \
	({                                                                           \
		cbuild_assert((stack)->ptr != 0, "(LIB_CBUILD_STACK) Stack underfloor.\n");\
		(stack)->data[--(stack)->ptr];                                             \
	})
/**
 * @brief Get top element of the stack (same as for pop, but read-only)
 *
 * @param stack => CBUILD_STACK* -> Stack. Will be evaluated multiple times.
 * @return VAL -> Element
 */
#define cbuild_stack_top(stack)                                                \
	({                                                                           \
		cbuild_assert((stack)->ptr != 0, "(LIB_CBUILD_STACK) Stack underfloor.\n");\
		(stack)->data[(stack)->ptr - 1];                                           \
	})
/**
 * @brief Resize stack (done automatically most of the time ;) )
 *
 * @param stack => CBUILD_STACK* -> Stack. Will be evaluated multiple times.
 * @param size => size_t -> New element count. Stack will be truncated if it
 * will be lover than stack->ptr. If it is zero then default behaviour is used
 */
#define cbuild_stack_resize(stack, size)                                       \
	do {                                                                         \
		if ((stack)->data == NULL) {                                               \
			if (size == 0) {                                                         \
				(stack)->capacity = CBUILD_INIT_CAPACITY;                              \
			} else {                                                                 \
				(stack)->capacity = size;                                              \
			}                                                                        \
			(stack)->data = cbuild_malloc(                                           \
				(stack)->capacity * sizeof(typeof(*(stack)->data)));                   \
		} else {                                                                   \
			if (size == 0) {                                                         \
				(stack)->capacity *= 2;                                                \
			} else {                                                                 \
				(stack)->capacity = size;                                              \
			}                                                                        \
			(stack)->data = cbuild_realloc((stack)->data,                            \
				(stack)->capacity * sizeof(typeof(*(stack)->data)));                   \
		}                                                                          \
		cbuild_assert((stack)->data != NULL,                                       \
			"(LIB_CBUILD_STACK) Allocation failed.\n");                              \
	} while (0)
/**
 * @brief Free stack
 *
 * @param stack => CBUILD_STACK* -> Stack. Will be evaluated multiple times.
 */
#define cbuild_stack_clear(stack)                                              \
	do {                                                                         \
		cbuild_free((stack)->data);                                                \
		(stack)->data     = NULL;                                                  \
		(stack)->ptr      = 0;                                                     \
		(stack)->capacity = 0;                                                     \
	} while(0)
/* Proc.h */
typedef struct cbuild_proclist_t {
	cbuild_proc_t* data;
	size_t size;
	size_t capacity;
} cbuild_proclist_t;
/**
 * @brief Free proclist
 *
 * @param da => cbuild_proclist_t* -> Proclist. Will be evaluated multiple times.
 */
#define cbuild_proclist_clear(proclist) cbuild_da_clear(proclist)
/**
 * @brief Wait until process finishes execution
 *
 * @param proc => cbuild_proc_t -> Process ID
 * @return true -> Process finished without errors
 * @return false -> Process finished with an error
 */
CBUILDDEF bool cbuild_proc_wait(cbuild_proc_t proc);
/**
 * @brief Wait until all process finish execution
 *
 * @param procs => cbuild_proclist_t -> Process IDs
 * @return true -> All proceses finished without errors
 * @return false -> At least one process finished with an error
 */
CBUILDDEF bool cbuild_procs_wait(cbuild_proclist_t procs);
/**
 * @brief Wait until process finishes execution
 *
 * @param proc => cbuild_proc_t -> Process ID
 * @return int -> Process exit code. INT_MIN on invalid proc.
 * Negative signal value if signalled.
 */
CBUILDDEF int cbuild_proc_wait_code(cbuild_proc_t proc);
/**
 * @brief Wait until any process from list finishes execution
 *
 * @param procs => cbuild_proclist_t -> Process ID list
 * @param code => int* -> Process exit code. Could be set to NULL if caller
 * don't care about this value.
 * @return ssize_t -> Index of finished process in array
 */
CBUILDDEF ssize_t cbuild_procs_wait_any(cbuild_proclist_t procs, int* code);
/**
 * @brief Check if process is running
 *
 * @param proc => cbuild_proc_t -> Process
 * @return bool => true if running, false otherwise
 */
CBUILDDEF bool cbuild_proc_is_running(cbuild_proc_t proc);
/**
 * @brief Start another process with generic function
 *
 * @param callback => int (*)(void*) -> Callback for created process, return
 * value will be process exit status and context will be passed from parent
 * process
 * @param context => Will be passed to a callback
 * @return CBuildProc -> Process ID
 */
CBUILDDEF cbuild_proc_t cbuild_proc_start(int (*callback)(void* context),
	void* context);
/**
 * @struct cbuild_proc_ptr
 * @brief "Pointer" for cbuild_proc_malloc and cbuild_proc_free. Struct
 * because of munmap limitations.
 */
typedef struct cbuild_proc_ptr_t {
	void*  ptr;
	size_t size;
} cbuild_proc_ptr_t;
/**
 * @brief Allocates memory that can be shared between fork'ed processes
 *
 * @param n => size_t -> Number of bytes
 * @return cbuild_proc_ptr_t -> Pointer to a memory
 */
CBUILDDEF cbuild_proc_ptr_t cbuild_proc_malloc(size_t n);
/**
 * @brief free for cbuild_proc_malloc
 *
 * @param ptr => cbuild_proc_ptr_t -> Pointer (from cbuild_proc_malloc)
 */
CBUILDDEF void cbuild_proc_free(cbuild_proc_ptr_t ptr);
/**
 * @brief Get amount of CPU cores
 *
 * @return int -> Number of logical cores
 */
CBUILDDEF int cbuild_nproc(void);
/* Command.h */
// Command
typedef struct cbuild_cmd_t {
	const char** data;
	size_t size;
	size_t capacity;
} cbuild_cmd_t;
// IO overrides
typedef struct {
	cbuild_fd_t fdstdin;
	cbuild_fd_t fdstdout;
	cbuild_fd_t fdstderr;
} cbuild_cmd_fd_t;
/**
 * @brief Append new arg to cmd
 *
 * @param cmd => cbuild_cmd_t* -> Command bufer to work with
 * @param val => char* -> New token
 */
#define	cbuild_cmd_append(cmd, val) cbuild_da_append(cmd, val)
/**
 * @brief Append new args to cmd
 *
 * @param cmd => cbuild_cmd_t* -> Command bufer to work with
 * @param vals => char** -> New tokens
 * @param vals_cnt => size_t -> Count of new tokens
 */
#define	cbuild_cmd_append_arr(cmd, vals, vals_cnt)                             \
	cbuild_da_append_arr(cmd, vals, vals_cnt)
/**
 * @brief Append new args to cmd
 *
 * @param cmd => cbuild_cmd_t* -> Command bufer to work with
 * @param ... => char* -> New tokens
 */
#define	cbuild_cmd_append_many(cmd, ...)                                       \
	cbuild_da_append_many(cmd, __VA_ARGS__)
/**
 * @brief  Clear command buffer
 *
 * @param cmd => cbuild_cmd_t* -> Command bufer to work with
 */
#define	cbuild_cmd_clear(cmd) cbuild_da_clear(cmd)
/**
 * @brief Conver cbuild_cmd_t to cbuild_sb_t
 *
 * @param cmd => cbuild_cmd_t -> Command
 * @return CBuildStrBuff -> String buffer to work with
 */
CBUILDDEF cbuild_sb_t cbuild_cmd_to_sb(cbuild_cmd_t cmd);
/**
 * @brief Conver cbuild_cmd_t to cbuild_sb_t
 *
 * @param cmd => cbuild_cmd_t -> Command
 * @return CBuildStrBuff -> String buffer to work with
 */
#define	cbuild_sb_from_cmd(cmd) cbuild_cmd_to_sb(cmd)
/**
 * @brief Call async command without io redirecting
 *
 * @param cmd => cbuild_cmd_t -> Command buffer
 * @return cbuild_proc_t -> Process associated with called command
 */
CBUILD_DEPRECATED("Please use cbuild_cmd_run instead!",
	CBUILDDEF cbuild_proc_t cbuild_cmd_async(cbuild_cmd_t cmd));
/**
 * @brief Call async command with io rediecting
 *
 * @param cmd => cbuild_cmd_t -> Command buffer
 * @param fd => cbuild_cmd_fd_t -> IO redicrecting table
 * @return cbuild_proc_t -> Process associated with called command
 */
CBUILD_DEPRECATED("Please use cbuild_cmd_run instead!",
	CBUILDDEF cbuild_proc_t cbuild_cmd_async_redirect(cbuild_cmd_t cmd,
		cbuild_cmd_fd_t fd));
/**
 * @brief Call sync command without io redirecting
 *
 * @param cmd => cbuild_cmd_t -> Command buffer
 * @return true -> Command succeed
 * @return false -> Command failed
 */
CBUILD_DEPRECATED("Please use cbuild_cmd_run instead!",
	CBUILDDEF bool cbuild_cmd_sync(cbuild_cmd_t cmd));
/**
 * @brief Call sync command with io rediecting
 *
 * @param cmd => cbuild_cmd_t -> Command buffer
 * @param df => cbuild_cmd_fd_t -> IO redicrecting table
 * @return true -> Command succeed
 * @return false -> Command failed
 */
CBUILD_DEPRECATED("Please use cbuild_cmd_run instead!",
	CBUILDDEF bool cbuild_cmd_sync_redirect(cbuild_cmd_t cmd, cbuild_cmd_fd_t fd));
typedef struct cbuild_cmd_opt_t {
	// Async
	cbuild_proclist_t* procs; // Non-null implies async
	cbuild_proc_t* proc;      // Non-null implies async
	// Redirect 'fdstd* ' and 'file_std*' is mutually exclusive.
	const cbuild_fd_t* fdstdin;
	const char* file_stdin;
	const cbuild_fd_t* fdstdout;
	const char* file_stdout;
	const cbuild_fd_t* fdstderr;
	const char* file_stderr;
	// Async
	int async_threads; // 0 means implementation-defined. -1 means unlimited. Expects 'procs' to be a valid empty array.
	// Flags
	union {
		uint32_t flags;
		struct {
			uint32_t no_reset     : 1;
			uint32_t autokill     : 1;
			uint32_t no_print_cmd : 1;
			uint32_t              : 29;
		};
	};
} cbuild_cmd_opt_t;
/**
 * @brief Run commannd
 *
 * @brief cmd => cbuild_cmd_t* -> Command to execute
 * @brief opts => cbuild_cmd_opt_t -> Command options
 */
CBUILDDEF bool cbuild_cmd_run_opt(cbuild_cmd_t* cmd, cbuild_cmd_opt_t opts);
/**
 * @brief Run commannd
 *
 * @brief cmd => cbuild_cmd_t* -> Command to execute
 * @brief ... => cbuild_cmd_opt_t... -> Command options,
 * will be inserted in struct intiliazier.
 */
#define cbuild_cmd_run(cmd, ...)                                               \
cbuild_cmd_run_opt(cmd, (cbuild_cmd_opt_t){ __VA_ARGS__ })
/* FS.h */
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
	CBUILD_FTYPE_MISSING   = -1,
	CBUILD_FTYPE_REGULAR   = 0,
	CBUILD_FTYPE_DIRECTORY = 1,
	CBUILD_FTYPE_SYMLINK   = 2,
	CBUILD_FTYPE_OTHER     = 3,
} cbuild_filetype_t;
/**
 *  @brief Close file dscriptor
 *
 *  @param fd => cbuild_fd_t -> File descriptor that should be close
 *  @return bool -> true on success, false otherwise
 */
CBUILDDEF bool cbuild_fd_close(cbuild_fd_t fd);
/**
 * @brief Open file descriptor for reading using file path
 *
 * @param path => const char* -> Path to file
 * @return cbuild_fd_t -> New file descriptor
 */
CBUILDDEF cbuild_fd_t cbuild_fd_open_read(const char* path);
/**
 * @brief Open file descriptor for writing using file path
 *
 * @param path => const char* -> Path to file
 * @return cbuild_fd_t -> New file descriptor
 */
CBUILDDEF cbuild_fd_t cbuild_fd_open_write(const char* path);
/**
 * @brief Create and open pipe
 *
 * @param read => cbuild_fd_t* -> Read file descriptor return
 * @param write => cbuild_fd_t* -> Write file descriptor return
 * @return bool -> true on success, false otherwise
 */
CBUILDDEF bool cbuild_fd_open_pipe(cbuild_fd_t* read, cbuild_fd_t* write);
/**
 * @bytes Read data from specifc file descriptor. POSIX 'read' syscall
 *
 * @param fd => cbuild_fd_t -> File descriptor
 * @param buf => void* -> Data buffer
 * @param nbytes => size_t -> Len of buffer in bytes
 * @return ssize_t -> Number of bytes read or -1 on error
 */
CBUILDDEF ssize_t cbuild_fd_read(cbuild_fd_t fd, void* buf, size_t nbytes);
/**
 * @bytes Read data from specifc file descriptor. POSIX 'read' syscall
 *
 * @param fd => cbuild_fd_t -> File descriptor
 * @param buf => void* -> Data buffer
 * @param nbytes => size_t -> Len of buffer in bytes
 * @param path => const char* -> File path for error reporting
 * @return ssize_t -> Number of bytes read or -1 on error
 */
CBUILDDEF ssize_t cbuild_fd_read_file(cbuild_fd_t fd, void* buf, size_t nbytes,
	const char* path);
/**
 * @brief Write data to specifc file descritptor. POSIX 'write' syscall
 *
 * @param fd => cbuild_fd_t -> File descriptor
 * @param buf => void* -> Data buffer
 * @param nbytes => size_t -> Len of buffer in bytes
 * @return ssize_t -> Number of bytes written or -1 on error
 */
CBUILDDEF ssize_t cbuild_fd_write(cbuild_fd_t fd, const void* buf, size_t nbytes);
/**
 * @brief Write data to specifc file descritptor. POSIX 'write' syscall
 *
 * @param fd => cbuild_fd_t -> File descriptor
 * @param buf => void* -> Data buffer
 * @param nbytes => size_t -> Len of buffer in bytes
 * @param path => const char* -> Path to file for error reporting
 * @return ssize_t -> Number of bytes written or -1 on error
 */
CBUILDDEF ssize_t cbuild_fd_write_file(cbuild_fd_t fd, const void* buf,
	size_t nbytes, const char* path);
/**
 * @brief Get size of files in bytes
 *
 * @param path => consh char* -> File path
 * @return ssize_t -> Size of file in bytes
 */
CBUILDDEF ssize_t cbuild_file_len(const char* path);
/**
 * @brief Read full file into buffer
 *
 * @param path => const char* -> File path
 * @param data => cbuild_sb_t* -> String buffer
 * @return bool -> true on success, false otherwise
 */
CBUILDDEF bool cbuild_file_read(const char* path, cbuild_sb_t* data);
/**
 * @brief Write full file (overwrite)
 *
 * @param path => const char* -> Filepath
 * @param data => cbuild_sb_t * -> Buffer to write
 * @return bool -> true on success, false otherwise
 */
CBUILDDEF bool cbuild_file_write(const char* path, cbuild_sb_t* data);
/**
 * @brief Copy file
 *
 * @param src => const char* -> Source filepath
 * @param dst => const char* -> Destination filepath
 * @return bool -> true on success, false otherwise
 */
CBUILDDEF bool cbuild_file_copy(const char* src, const char* dst);
/**
 * @brief Move file
 *
 * @param src => const char* -> Source filepath
 * @param dst => const char* -> Destination filepath
 * @return bool -> true on success, false otherwise
 */
CBUILDDEF bool cbuild_file_move(const char* src, const char* dst);
/**
 * @brief Rename file
 *
 * @param src => const char* -> Source filepath
 * @param dst => const char* -> Destination filepath
 * @return bool -> true on success, false otherwise
 */
CBUILDDEF bool cbuild_file_rename(const char* src, const char* dst);
/**
 * @brief Check if file exist
 *
 * @param path => const char* -> Filepath
 * @return bool -> Result
 */
CBUILDDEF bool cbuild_file_check(const char* path);
/**
 * @brief Remove file
 *
 * @param path => const char* -> Filepath
 * @return bool -> true on success, false otherwise
 */
CBUILDDEF bool cbuild_file_remove(const char* path);
/**
 * @brief Create symbolic link. Will overwrite file if dst exists.
 *
 * @param src => consh char* -> Source string
 * @param dst => const char* -> Destination file
 * @return bool -> true on success, false on failure
 */
CBUILDDEF bool cbuild_symlink(const char* src, const char* dst);
/**
 * @brief Copy directory.
 *
 * @param src => const char* -> Source filepath
 * @param dst => const char* -> Destination filepath
 * @return bool -> true on success, false otherwise
 */
CBUILDDEF bool cbuild_dir_copy(const char* src, const char* dst);
/**
 * @brief Move directory.
 *
 * @param src => const char* -> Source filepath
 * @param dst => const char* -> Destination filepath
 * @return bool -> true on success, false otherwise
 */
CBUILDDEF bool cbuild_dir_move(const char* src, const char* dst);
/**
 * @brief Rename directory.
 *
 * @param src => const char* -> Source filepath
 * @param dst => const char* -> Destination filepath
 * @return bool -> true on success, false otherwise
 */
CBUILDDEF bool cbuild_dir_rename(const char* src, const char* dst);
/**
 * @brief Remove directory.
 *
 * @param path => const char* -> Filepath
 * @return bool -> true on success, false otherwise
 */
CBUILDDEF bool cbuild_dir_remove(const char* path);
/**
 * @brief Check if directory exist.
 *
 * @param path => const char* -> Filepath
 * @return bool -> true on success, false otherwise
 */
CBUILDDEF bool cbuild_dir_check(const char* path);
/**
 * @brief Get list of file in a directory.
 *
 * @param path => const char* -> Filepath
 * @param elements => cbuild_pathlist_t* -> Return valud
 */
CBUILDDEF bool cbuild_dir_list(const char* path, cbuild_pathlist_t* elements);
/**
 * @brief Get current working directory
 *
 * @return char* -> Current working directory. Malloced.
 */
CBUILDDEF char* cbuild_dir_current(void);
/**
 * @brief Set current working directory. Can work with relative path.
 *
 * @param path => char* -> Path to set as current working directory
 * @return bool -> True on success, false otherwise
 */
CBUILDDEF bool cbuild_dir_set_current(const char* path);
/**
 * @brief Free path list structure
 *
 * @param list => cbuild_pathlist_t* -> Path list
 */
CBUILDDEF void cbuild_pathlist_clear(cbuild_pathlist_t* list);
/**
 * @brief Create directory.
 *
 * @param path => const char* -> Filepath
 * @return bool -> true on success, false otherwise
 */
CBUILDDEF bool cbuild_dir_create(const char* path);
/**
 * @brief Get type of filesystem entry.
 *
 * @param path => const char* -> Path
 * @return CBuildFiletype -> Type of filesystem element
 */
CBUILDDEF cbuild_filetype_t cbuild_path_filetype(const char* path);
/**
 * @brief Get file extension (after last '.')
 *
 * @param path => const char* -> Filepath
 * @return char* -> String allocated on heap
 */
CBUILDDEF char* cbuild_path_ext(const char* path);
/**
 * @brief Get file name (after last '/' and before last '.' or if path ends with
 * '/' then after previous '/' to last '/')
 *
 * @param path => const char* -> Filepath
 * @return char* -> String allocated on heap
 */
CBUILDDEF char* cbuild_path_name(const char* path);
/**
 * @brief Everything that function cbuild_path_name removes
 *
 * @param path => const char* -> Filepath
 * @return char* -> String allocated on heap
 */
CBUILDDEF char* cbuild_path_base(const char* path);
/**
 * @brief Normalize path
 *
 * @param path => const char* -> Filepath
 * @return char* -> String allocate on heap
 */
CBUILDDEF char* cbuild_path_normalize(const char* path);
/* Compile.h */
#if !defined(CBUILD_CC)
	#if defined(CBUILD_CC_CLANG)
		#define CBUILD_CC "clang"
	#elif defined(CBUILD_CC_GCC)
		#define CBUILD_CC "gcc"
	#else
		#define CBUILD_CC "cc"
	#endif // Compiler select
#endif   // CBUILD_CC
#if !defined(CBUILD_LD)
	#if defined(CBUILD_CC_CLANG)
		#define CBUILD_LD "clang"
	#elif defined(CBUILD_CC_GCC)
		#define CBUILD_LD "gcc"
	#else
		#define CBUILD_LD "cc"
	#endif // Compiler select
#endif   // CBUILD_LD
#if !defined(CBUILD_CXX)
	#if defined(CBUILD_CC_CLANG)
		#define CBUILD_CXX "clang++"
	#elif defined(CBUILD_CC_GCC)
		#define CBUILD_CXX "g++"
	#else
		#define CBUILD_CXX "c++"
	#endif // Compiler select
#endif   // CBUILD_CXX
#if !defined(CBUILD_AR)
	#if defined(CBUILD_CC_CLANG)
		#define CBUILD_AR "llvm-ar"
	#elif defined(CBUILD_CC_GCC)
		#define CBUILD_AR "ar"
	#else
		#define CBUILD_AR "ar"
	#endif // Compiler select
#endif   // CBUILD_AR
#if !defined(CBUILD_AS)
	#if defined(CBUILD_CC_CLANG)
		#define CBUILD_AS "llvm-as"
	#elif defined(CBUILD_CC_GCC)
		#define CBUILD_AS "as"
	#else
		#define CBUILD_AS "as"
	#endif // Compiler select
#endif   // CBUILD_AS
#if !defined(CBUILD_CPP)
	#define CBUILD_CPP CBUILD_CC "-E"
#endif   // CPP
#if !defined(CBUILD_CC_OUT)
	#if defined(CBUILD_CC_CLANG)
		#define CBUILD_CC_OUT "-o"
	#elif defined(CBUILD_CC_GCC)
		#define CBUILD_CC_OUT "-o"
	#else
		#define CBUILD_CC_OUT "-o"
	#endif // Compiler select
#endif   // CBUILD_CC_OUT
// Predefined compiler arguments
#define CBUILD_CARGS_WARN                     "-Wall", "-Wextra", "-Wno-comments", "-Wconversion", "-Wcast-align", "-Wvla", "-Wno-override-init", "-Wshadow"
#define CBUILD_CARGS_WERROR                   "-Werror"
#if defined(CBUILD_CC_CLANG)
	#define CBUILD_CARGS_STATIC_ANALYZER        "--analyze", "-Xanalyzer", "-analyzer-output=text"
#elif defined(CBUILD_CC_GCC)
	#define CBUILD_CARGS_STATIC_ANALYZER        "-fanalyzer", "-Wanalyzer-too-complex"
#else
	#warn "Unknown compiler. Analyzer flags are empty"
	#define CBUILD_CARGS_STATIC_ANALYZER
#endif // Compiler select (clang/gcc)
#define CBUILD_CARGS_PROFILE                       "-pg"
#define CBUILD_CARGS_DEBUG                         "-g"
#define CBUILD_CARGS_DEBUG_GDB                     "-ggdb"
#define CBUILD_CARGS_MT                            "-pthread"
#define CBUILD_CARGS_DEFINE(defname)               "-D" defname
#define CBUILD_CARGS_DEFINE_TEMP(defname)          cbuild_temp_sprintf("-D%s", defname)
#define CBUILD_CARGS_DEFINE_VAL(defname, val)      "-D" defname "=" val
#define CBUILD_CARGS_DEFINE_VAL_TEMP(defname, val) cbuild_temp_sprintf("-D%s=%s", defname, val)
#define CBUILD_CARGS_UNDEF(defname)                "-U" defname
#define CBUILD_CARGS_INCLUDE(file)                 "--include", file
#define CBUILD_CARGS_LIBINCLUDE(lib)               "-l" lib
#define CBUILD_CARGS_LIBDIR(src, obj)              "-I" src, "-L" obj
#define CBUILD_CARGS_STD(std)                      "-std=" std
// Self-rebuild wrapper macro
#define cbuild_selfrebuild(argc, argv)                                         \
	__cbuild_selfrebuild(argc, argv, 1, __FILE__)
// Self-rebuild wrapper macro that support extra dependencies
#define cbuild_selfrebuild_ex(argc, argv, ...)                                 \
	do {                                                                         \
		const char* __cbuild_tmp_files[] = { __VA_ARGS__ };                        \
		size_t __cbuild_tmp_files_len = cbuild_arr_len(__cbuild_tmp_files);        \
		__cbuild_selfrebuild(argc, argv, __cbuild_tmp_files_len + 1,               \
			__FILE__, __VA_ARGS__);                                                  \
	} while(0)
/**
 * @brief Enables self-rebuild functionality
 *
 * @param argc => int -> Number of cli arguments
 * @param argv => char** -> Params passed to an app. Needed to get binary path.
 * @param num_file => size_t -> Number of dependency files in variadics.
 * @param ... => const char* -> Dependency files.
 * First will be used in compilation.
 * Other will just be checked to determine if rebuild is needed.
 */
CBUILDDEF void __cbuild_selfrebuild(int argc, char** argv, size_t num_files, ...);
/**
 * @brief Enables self-rebuild functionality
 *
 * @param argc => int -> Number of cli arguments
 * @param argv => char** -> Params passed to an app. Needed to get binary path.
 * @param filess => cbuild_cmd_t -> Dependency files.
 * First will be used in compilation.
 * Other will just be checked to determine if rebuild is needed.
 */
CBUILDDEF void __cbuild_selfrebuild_ex(int argc, char** argv, cbuild_cmd_t files);
/**
 * @brief Enables self-rebuild functionality
 *
 * @param argc => int -> Number of cli arguments
 * @param argv => char** -> Params passed to an app. Needed to get binary path.
 * @param argv0_path => Full path to argv[0]. Required mostly for self-rebuilding 
 * binaries which may be in PATH and called directly using PATH. For binaries 
 * which are not in PATH can just be 'argv[0].
 * @param filess => cbuild_cmd_t -> Dependency files.
 * First will be used in compilation.
 * Other will just be checked to determine if rebuild is needed.
 */
CBUILDDEF void __cbuild_selfrebuild_ex_argv(int argc, char** argv, 
	const char* argv0_path, cbuild_cmd_t files);
/**
 * @brief Allow to add additional arguments for self-rebuild.
 * Called with compiler command already in place.
 */
extern void (*cbuild_selfrebuild_hook)(cbuild_cmd_t* cmd);
/**
 * @brief Compare mtime of two files and report if input is newer than output
 *
 * @param output => char* -> Output file's filepath
 * @param input => char* -> Input file's filepath
 * @return int=0 -> Output is newer than input
 * @return int<0 -> Error
 * @return int>0 -> Output is older than input
 */
CBUILDDEF int cbuild_compare_mtime(const char* output, const char* input);
/**
 * @brief Compare mtime of one output file and many input files and report if
 * any input is newer than output
 *
 * @param output => char* -> Output file's filepath
 * @param inputs => char** -> Input file's filepaths
 * @param num_inputs => size_t -> Number of inout files
 * @return int=0 -> Output is newer than input
 * @return int<0 -> Error
 * @return int>0 -> Output is older than input, indicates number of newer inputs
 */
CBUILDDEF int cbuild_compare_mtime_many(const char* output, const char** inputs,
	size_t num_inputs);
/* DLLoad.h */
#if defined(CBUILD_API_POSIX)
	#define	cbuild_dlib_t                              void*
	#define	cbuild_dlib_load(filename)                 dlopen(filename, RTLD_LAZY)
	#define	cbuild_dlib_load_force(filename)           dlopen(filename, RTLD_NOW)
	#define	cbuild_dlib_get_function(handle, funcName) dlsym(handle, funcName)
	#define	cbuild_dlib_close(handle)                  dlclose(handle)
#endif // CBUILD_API_*
/* FlagParse.h */
typedef struct cbuild_arglist_t {
	char** data;
	size_t size;
	size_t capacity;
} cbuild_arglist_t;
/**
 * @brief Parse flag spec and register it
 * [See here for a full spec](../4.internals/2.flagparse.html)
 *
 * @param spec => char* -> Flag spec
 */
CBUILDDEF void cbuild_flag_new(const char* spec);
/**
 * @brief Parse flags
 *
 * @param argc => int -> Argument count
 * @param argv => char** -> Arguments, argv[0] is a binary name
 */
CBUILDDEF void cbuild_flag_parse(int argc, char** argv);
/**
 * @brief Prints help for all flags to a stdout
 */
CBUILDDEF void cbuild_flag_print_help(void);
/**
 * @brief Get positional arguments list
 *
 * @return cbuild_flag_arglist_t* -> List of positional arguments
 */
CBUILDDEF cbuild_arglist_t* cbuild_flag_get_pargs(void);
/**
 * @brief Get arguments of a specific flag
 *
 * @param opt => char* -> Flag long version
 * @return cbuild_flag_arglist_t* -> List of flag's arguments or NULL if flag
 * not found
 */
CBUILDDEF cbuild_arglist_t* cbuild_flag_get_flag(const char* opt);
/**
 * @brief Get app name as a C-string
 *
 * @return char* -> App name
 */
CBUILDDEF const char* cbuild_flag_app_name(void);
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
#endif // __CBUILD__H__
#ifdef CBUILD_IMPLEMENTATION
	/* misc code */
	#if defined(CBUILD_API_STRICT_POSIX) || (defined(CBUILD_API_POSIX) && ( \
				(defined(CBUILD_OS_LINUX) && \
					!(defined(CBUILD_OS_LINUX_GLIBC) || defined(CBUILD_OS_LINUX_MUSL))) || \
				defined(CBUILD_OS_MACOS) || \
				defined(CBUILD_OS_BSD) || \
				defined(CBUILD_OS_WINDOWS_CYGWIN)))
		extern const char* __progname;
	#endif // CBUILD_API_POSIX + ext check || CBUILD_API_STRICT_POSIX
	const char* __cbuild_progname(void) {
		#if defined(CBUILD_API_POSIX)
			#if defined(CBUILD_OS_LINUX_GLIBC) || defined(CBUILD_OS_LINUX_MUSL) || \
				defined(CBUILD_OS_WINDOWS_CYGWIN)
				return program_invocation_short_name;
			#elif defined(CBUILD_OS_BSD) || defined(CBUILD_OS_MACOS)
				return getprogname();
			#else
				return __progname;
			#endif // CBUILD_OS_*
		#elif defined(CBUILD_API_STRICT_POSIX)
			return __progname;
		#endif // CBUILD_API_*
	}
	void* (*cbuild_malloc)(size_t size) = malloc;
	void* (*cbuild_realloc)(void* ptr, size_t size) = realloc;
	void (*cbuild_free)(void* ptr) = free;
	/* common.h impl */
	CBUILDDEF void __cbuild_assert(const char* file, unsigned int line, const char* func,
		const char* expr, ...) {
		__CBUILD_ERR_PRINTF("%s: %s:%u: %s: Assertion \"%s\" failed with message:\n",
			__cbuild_progname(), file, line, func, expr);
		va_list args;
		va_start(args, expr);
		const char* fmt = va_arg(args, char*);
		__CBUILD_ERR_VPRINTF(fmt, args);
		va_end(args);
		__CBUILD_ERR_FLUSH();
		abort();
	}
	#if defined(CBUILD_API_POSIX) || defined(CBUILD_API_STRICT_POSIX)
		CBUILDDEF uint64_t cbuild_time_nanos(void) {
			struct timespec t;
			clock_gettime(CLOCK_MONOTONIC, &t);
			return (uint64_t)(CBUILD_NANOS_PER_SEC * t.tv_sec + t.tv_nsec);
		}
	#endif // CBUILD_API_*
	char* cbuild_sv_to_cstr(cbuild_sv_t sv) {
		char* ret = malloc(sv.size + 1);
		memcpy(ret, sv.data, sv.size);
		ret[sv.size] = '\0';
		return ret;
	}
	char* cbuild_temp_sv_to_cstr(cbuild_sv_t sv) {
		char* ret = cbuild_temp_alloc(sv.size + 1);
		memcpy(ret, sv.data, sv.size);
		ret[sv.size] = '\0';
		return ret;
	}
	/* StringBuilder.h impl */
	CBUILDDEF int cbuild_sb_cmp(cbuild_sb_t a, cbuild_sb_t b) {
		if(a.size < b.size) {
			return -2;
		}
		if(a.size > b.size) {
			return 2;
		}
		int ret = memcmp(a.data, b.data, a.size);
		if(ret == 0) {
			return 0;
		} else if(ret < 0) {
			return -1;
		} else if(ret > 0) {
			return 1;
		}
		CBUILD_UNREACHABLE("cbuild_sb_cmp fallthrough");
	}
	CBUILDDEF int cbuild_sb_cmp_icase(cbuild_sb_t a, cbuild_sb_t b) {
		if(a.size < b.size) {
			return -2;
		}
		if(a.size > b.size) {
			return 2;
		}
		for(size_t i = 0; i < a.size; i++) {
			char ac =
				'A' <= a.data[i] && a.data[i] <= 'Z' ? a.data[i] + 32 : a.data[i];
			char bc =
				'A' <= b.data[i] && b.data[i] <= 'Z' ? b.data[i] + 32 : b.data[i];
			int diff = ac - bc;
			if(diff < 0) {
				return -1;
			} else if(diff > 0) {
				return 1;
			}
		}
		return 0;
	}
	CBUILDDEF cbuild_sb_t cbuild_sv_to_sb(cbuild_sv_t sv) {
		cbuild_sb_t ret = {0};
		cbuild_sb_append_arr(&ret, sv.data, sv.size);
		return ret;
	}
	CBUILDDEF cbuild_sv_t cbuild_sb_to_sv(cbuild_sb_t sb) {
		return (cbuild_sv_t) {
			.data = sb.data, .size = sb.size
		};
	}
	CBUILDDEF int cbuild_sb_vappendf(cbuild_sb_t* sb, const char* fmt, va_list args) {
		va_list args_copy;
		va_copy(args_copy, args);
		char buff[CBUILD_SB_QUICK_SPRINTF_SIZE];
		int ret = vsnprintf(buff, CBUILD_SB_QUICK_SPRINTF_SIZE, fmt, args);
		if(ret < 0) {
			va_end(args_copy);
			return ret;
		}
		if((size_t)ret >= CBUILD_SB_QUICK_SPRINTF_SIZE) {
			char* buff1 = cbuild_malloc((size_t)ret + 1);
			cbuild_assert(buff1 != NULL, "(LIB_CBUILD_SB) Allocation failed.\n");
			vsnprintf(buff1, (size_t)ret + 1, fmt, args_copy);
			cbuild_sb_append_cstr(sb, buff1);
			cbuild_free(buff1);
		} else {
			cbuild_sb_append_cstr(sb, buff);
		}
		va_end(args_copy);
		return ret;
	}
	CBUILDDEF int cbuild_sb_appendf(cbuild_sb_t* sb, const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
		int ret = cbuild_sb_vappendf(sb, fmt, args);
		va_end(args);
		return ret;
	}
	CBUILDDEF void cbuild_sb_append_utf8(cbuild_sb_t* sb, uint32_t codepoint) {
		unsigned char buffer[4] = {0};
		size_t len = 0;
		if(codepoint <= 0x7F) {
			buffer[0] = (unsigned char)codepoint;
			len = 1;
		} else if(codepoint <= 0x7FF) {
			buffer[0] = 0xC0 | ((codepoint >> 6) & 0x1F);
			buffer[1] = 0x80 | (codepoint & 0x3F);
			len = 2;
		} else if(codepoint <= 0xFFFF) {
			buffer[0] = 0xE0 | ((codepoint >> 12) & 0x0F);
			buffer[1] = 0x80 | ((codepoint >> 6) & 0x3F);
			buffer[2] = 0x80 | (codepoint & 0x3F);
			len = 3;
		} else if(codepoint <= 0x10FFFF) {
			buffer[0] = 0xF0 | ((codepoint >> 18) & 0x07);
			buffer[1] = 0x80 | ((codepoint >> 12) & 0x3F);
			buffer[2] = 0x80 | ((codepoint >> 6) & 0x3F);
			buffer[3] = 0x80 | (codepoint & 0x3F);
			len = 4;
		} else {
			CBUILD_UNREACHABLE("(LIB_CBUILD_SB) Invalid Unicode codepoint!\n");
		}
		cbuild_sb_append_arr(sb, buffer, len);
	}
	CBUILDDEF int cbuild_sb_utf8cmp(cbuild_sb_t a, cbuild_sb_t b) {
		return cbuild_sv_cmp(cbuild_sv_from_sb(a), cbuild_sv_from_sb(b));
	}
	CBUILDDEF size_t cbuild_sb_utf8len(cbuild_sb_t sb) {
		return cbuild_sv_utf8len(cbuild_sv_from_sb(sb));
	}
	/* StringView.h impl */
	CBUILDDEF size_t cbuild_sv_trim_left(cbuild_sv_t* sv) {
		size_t i = 0;
		while(i < sv->size && isspace(sv->data[i])) {
			i++;
		}
		sv->data += i;
		sv->size -= i;
		return i;
	}
	CBUILDDEF size_t cbuild_sv_trim_right(cbuild_sv_t* sv) {
		size_t i = sv->size;
		while(i > 0 && isspace(sv->data[i - 1])) {
			i--;
		}
		size_t tmp = sv->size - i;
		sv->size  -= tmp;
		return tmp;
	}
	CBUILDDEF size_t cbuild_sv_trim(cbuild_sv_t* sv) {
		size_t ret = cbuild_sv_trim_left(sv);
		ret       += cbuild_sv_trim_right(sv);
		return ret;
	}
	CBUILDDEF cbuild_sv_t cbuild_sv_chop(cbuild_sv_t* sv, size_t size) {
		if(size > sv->size) {
			size = sv->size;
		}
		char* tmp = sv->data;
		sv->data += size;
		sv->size -= size;
		return cbuild_sv_from_parts(tmp, size);
	}
	CBUILDDEF cbuild_sv_t cbuild_sv_chop_by_delim(cbuild_sv_t* sv, char delim) {
		char* chrptr = memchr(sv->data, delim, sv->size);
		if(chrptr != NULL) {
			size_t i = (size_t)(chrptr - sv->data);
			cbuild_sv_t ret = cbuild_sv_from_parts(sv->data, i);
			sv->data += i + 1;
			sv->size -= i + 1;
			return ret;
		}
		return cbuild_sv_chop(sv, sv->size);
	}
	CBUILDDEF cbuild_sv_t cbuild_sv_chop_by_sv(cbuild_sv_t* sv, cbuild_sv_t delim) {
		if(delim.size == 0 || delim.size > sv->size) {
			return cbuild_sv_from_parts(sv->data, 0);
		}
		char* chrptr = sv->data;
		size_t i = 0;
		do {
			chrptr = memchr(chrptr + 1, delim.data[0], sv->size);
			if(chrptr != NULL && sv->size - i >= delim.size &&
				memcmp(chrptr, delim.data, delim.size) == 0) {
				i = (size_t)(chrptr - sv->data);
				cbuild_sv_t ret = cbuild_sv_from_parts(sv->data, i);
				sv->data += delim.size + i;
				sv->size -= delim.size + i;
				return ret;
			}
		} while(chrptr != NULL);
		return cbuild_sv_chop(sv, sv->size);
	}
	CBUILDDEF cbuild_sv_t cbuild_sv_chop_by_func(cbuild_sv_t* sv, cbuild_sv_delim_func delim,
		void* args) {
		size_t i = 0;
		while(i <= sv->size && !delim(sv, i, args)) {
			i++;
		}
		if(i >= sv->size) {
			return cbuild_sv_chop(sv, i);
		}
		char* tmp = sv->data;
		sv->data += i + 1;
		sv->size -= i + 1;
		return cbuild_sv_from_parts(tmp, i);
	}
	CBUILDDEF int cbuild_sv_cmp(cbuild_sv_t a, cbuild_sv_t b) {
		if(a.size < b.size) {
			return -2;
		}
		if(a.size > b.size) {
			return 2;
		}
		int ret = memcmp(a.data, b.data, a.size);
		if(ret == 0) {
			return 0;
		} else if(ret < 0) {
			return -1;
		} else if(ret > 0) {
			return 1;
		}
		return 0;
	}
	CBUILDDEF int cbuild_sv_cmp_icase(cbuild_sv_t a, cbuild_sv_t b) {
		if(a.size < b.size) {
			return -2;
		}
		if(a.size > b.size) {
			return 2;
		}
		for(size_t i = 0; i < a.size; i++) {
			char ac = 'A' <= a.data[i] && a.data[i] <= 'Z' ? a.data[i] + 32 : a.data[i];
			char bc = 'A' <= b.data[i] && b.data[i] <= 'Z' ? b.data[i] + 32 : b.data[i];
			int diff = ac - bc;
			if(diff < 0) {
				return -1;
			} else if(diff > 0) {
				return 1;
			}
		}
		return 0;
	}
	CBUILDDEF bool cbuild_sv_prefix(cbuild_sv_t sv, cbuild_sv_t prefix) {
		if(sv.size < prefix.size) {
			return false;
		}
		return cbuild_sv_cmp(cbuild_sv_from_parts(sv.data, prefix.size), prefix) == 0;
	}
	CBUILDDEF bool cbuild_sv_suffix(cbuild_sv_t sv, cbuild_sv_t suffix) {
		if(sv.size < suffix.size) {
			return false;
		}
		return cbuild_sv_cmp(
			cbuild_sv_from_parts(sv.data + sv.size - suffix.size, suffix.size),
			suffix) == 0;
	}
	CBUILDDEF ssize_t cbuild_sv_find(cbuild_sv_t sv, char c) {
		if(sv.size == 0) return -1;
		char* chrptr = memchr(sv.data, c, sv.size);
		if(chrptr == NULL) {
			return -1;
		}
		return chrptr - sv.data;
	}
	CBUILDDEF ssize_t cbuild_sv_rfind(cbuild_sv_t sv, char c) {
		if(sv.size == 0) return -1;
		char* chrptr = sv.data;
		#if defined(CBUILD_API_POSIX) && ( \
				defined(CBUILD_OS_LINUX_GLIBC) || defined(CBUILD_OS_LINUX_MUSL) || defined(CBUILD_OS_LINUX_UCLIBC) || \
				defined(CBUILD_OS_BSD) || \
				defined(CBUILD_OS_MACOS))
			chrptr = memrchr(sv.data, c, sv.size);
		#else
			chrptr += sv.size;
			do {
				chrptr--;
				if(*chrptr == c) {
					goto loop_end;
				}
			} while(chrptr > sv.data);
			chrptr = NULL;
		loop_end:
		#endif // Extension check
		if(chrptr == NULL) {
			return -1;
		}
		return chrptr - sv.data;
	}
	CBUILDDEF ssize_t cbuild_sv_find_sv(cbuild_sv_t sv, cbuild_sv_t needle) {
		#if defined(CBUILD_API_POSIX) && ( \
				defined(CBUILD_OS_LINUX_GLIBC) || defined(CBUILD_OS_LINUX_MUSL) || \
				defined(CBUILD_OS_BSD) || \
				defined(CBUILD_OS_MACOS))
			char* chrptr = memmem(sv.data, sv.size, needle.data, needle.size);
			if(chrptr == NULL) {
				return -1;
			}
			return chrptr - sv.data;
		#else
			if(sv.size < needle.size) return -1;
			const char* p = sv.data;
			size_t rem = sv.size;
			while((p = memchr(p, *needle.data, rem)) != NULL) {
				size_t offset = (size_t)(p - sv.data);
				rem = sv.size - offset;
				if(rem < needle.size) return -1;
				if(cbuild_sv_cmp(needle, cbuild_sv_from_parts(p, needle.size)) == 0) {
					return (ssize_t)offset;
				}
				p++;
				rem--;
			}
			return -1;
		#endif // Extension check
	}
	CBUILDDEF bool cbuild_sv_contains(cbuild_sv_t sv, char c) {
		return cbuild_sv_find(sv, c) != -1;
	}
	CBUILDDEF bool cbuild_sv_contains_sv(cbuild_sv_t sv, cbuild_sv_t needle) {
		return cbuild_sv_find_sv(sv, needle) != -1;
	}
	CBUILDDEF int cbuild_sv_utf8cp_len(cbuild_sv_t sv) {
		if(sv.size == 0) return 0;
		if((*(unsigned char*)sv.data) < 0x80) return 1;
		if((*sv.data & 0xE0) == 0xC0 && sv.size >= 2) return 2;
		if((*sv.data & 0xF0) == 0xE0 && sv.size >= 3) return 3;
		if((*sv.data & 0xF8) == 0xF0 && sv.size >= 4) return 4;
		return 1;
	}
	CBUILDDEF char* cbuild_sv_strchr(cbuild_sv_t sv, char c) {
		return memchr(sv.data, c, sv.size);
	}
	CBUILDDEF char* cbuild_sv_utf8chr(cbuild_sv_t sv, uint32_t c) {
		while(sv.size > 0) {
			char* curr = sv.data;
			uint32_t codepoint = cbuild_sv_chop_utf8(&sv);
			if(codepoint == c) return curr;
		}
		return NULL;
	}
	CBUILDDEF uint32_t cbuild_sv_chop_utf8(cbuild_sv_t* sv) {
		if(sv->size == 0) return UINT32_MAX;
		unsigned char* s = (unsigned char*)sv->data;
		if(*s < 0x80) {
			sv->data++; s++;
			sv->size--;
			unsigned char byte1 = *(s - 1);
			uint32_t codepoint = byte1;
			return codepoint;
		}
		if((*s & 0xE0) == 0xC0 && sv->size >= 2) {
			sv->data += 2; s += 2;
			sv->size -= 2;
			unsigned char byte1 = *(s - 2),
			byte2 = *(s - 1);
			uint32_t codepoint = ((byte1 & 0x1Fu) << 6) |
				(byte2 & 0x3Fu);
			return codepoint;
		}
		if((*s & 0xF0) == 0xE0 && sv->size >= 3) {
			sv->data += 3; s += 3;
			sv->size -= 3;
			unsigned char byte1 = *(s - 3),
			byte2 = *(s - 2),
			byte3 = *(s - 1);
			uint32_t codepoint = ((byte1 & 0x0Fu) << 12) |
				((byte2 & 0x3Fu) << 6) |
				(byte3 & 0x3Fu);
			return codepoint;
		}
		if((*s & 0xF8) == 0xF0 && sv->size >= 4) {
			sv->data += 4; s += 4;
			sv->size -= 4;
			unsigned char byte1 = *(s - 4),
			byte2 = *(s - 3),
			byte3 = *(s - 2),
			byte4 = *(s - 1);
			uint32_t codepoint = ((byte1 & 0x07u) << 18) |
				((byte2 & 0x3Fu) << 12) |
				((byte3 & 0x3Fu) << 6) |
				(byte4 & 0x3Fu);
			return codepoint;
		}
		sv->data++; s++;
		sv->size--;
		return *(s - 1);
	}
	CBUILDDEF cbuild_sv_t cbuild_sv_chop_by_utf8(cbuild_sv_t* sv, uint32_t delim) {
		char* chrptr = cbuild_sv_utf8chr(*sv, delim);
		if(chrptr != NULL) {
			size_t i = (size_t)(chrptr - sv->data);
			cbuild_sv_t ret = cbuild_sv_from_parts(sv->data, i);
			sv->data += i;
			sv->size -= i;
			int codepoint_size = cbuild_sv_utf8cp_len(*sv);
			sv->data += (size_t)codepoint_size;
			sv->size -= (size_t)codepoint_size;
			return ret;
		}
		return cbuild_sv_chop(sv, sv->size);
	}
	CBUILDDEF cbuild_sv_t cbuild_sv_chop_by_func_utf8(cbuild_sv_t* sv,
		cbuild_sv_utf8delim_func delim, void* args) {
		cbuild_sv_t ret = cbuild_sv_from_parts(sv->data, 0);
		while(sv->size > 0) {
			size_t cplen = (size_t)cbuild_sv_utf8cp_len(*sv);
			if(delim(sv, args)) {
				sv->data += cplen;
				sv->size -= cplen;
				break;
			} else {
				sv->data += cplen;
				sv->size -= cplen;
				ret.size += cplen;
			}
		}
		return ret;
	}
	CBUILDDEF int cbuild_sv_utf8cmp(cbuild_sv_t a, cbuild_sv_t b) {
		return cbuild_sv_cmp(a, b);
	}
	CBUILDDEF size_t cbuild_sv_utf8len(cbuild_sv_t sv) {
		size_t ret = 0;
		while(sv.size > 0) {
			cbuild_sv_chop_utf8(&sv);
			ret++;
		}
		return ret;
	}
	CBUILDDEF bool cbuild_sv_utf8valid(cbuild_sv_t sv, size_t* idx) {
		size_t ret = 0;
		while(sv.size > 0) {
			signed char cs = *sv.data;
			ret++;
			if(cs > 0) { // ASCII, 'signed char' abuse
				sv.size--;
				sv.data++;
				continue;
			}
			unsigned char c = (unsigned char)cs;
			if(0xC2 <= c && c <= 0xDF) { // 2 byte
				sv.size--;
				sv.data++;
				if(sv.size == 0) goto invalid;
				unsigned char c1 = *(unsigned char*)sv.data;
				if(0x80 <= c1 && c1 <= 0xBF) {
					sv.size--;
					sv.data++;
				} else {
					goto invalid;
				}
			} else if(c == 0xE0) { // 3 byte
				sv.size--;
				sv.data++;
				if(sv.size == 0) goto invalid;
				unsigned char c1 = *(unsigned char*)sv.data;
				if(0xA0 <= c1 && c1 <= 0xBF) {
					sv.size--;
					sv.data++;
					if(sv.size == 0) goto invalid;
					unsigned char c2 = *(unsigned char*)sv.data;
					if(0x80 <= c2 && c2 <= 0xBF) {
						sv.size--;
						sv.data++;
					} else {
						goto invalid;
					}
				} else {
					goto invalid;
				}
			} else if(0xE1 <= c && c <= 0xEC) {
				sv.size--;
				sv.data++;
				if(sv.size == 0) goto invalid;
				unsigned char c1 = *(unsigned char*)sv.data;
				if(0x80 <= c1 && c1 <= 0xBF) {
					sv.size--;
					sv.data++;
					if(sv.size == 0) goto invalid;
					unsigned char c2 = *(unsigned char*)sv.data;
					if(0x80 <= c2 && c2 <= 0xBF) {
						sv.size--;
						sv.data++;
					} else {
						goto invalid;
					}
				} else {
					goto invalid;
				}
			} else if(c == 0xED) {
				sv.size--;
				sv.data++;
				if(sv.size == 0) goto invalid;
				unsigned char c1 = *(unsigned char*)sv.data;
				if(0x80 <= c1 && c1 <= 0x9F) {
					sv.size--;
					sv.data++;
					if(sv.size == 0) goto invalid;
					unsigned char c2 = *(unsigned char*)sv.data;
					if(0x80 <= c2 && c2 <= 0xBF) {
						sv.size--;
						sv.data++;
					} else {
						goto invalid;
					}
				} else {
					goto invalid;
				}
			} else if(0xEE <= c && c <= 0xEF) {
				sv.size--;
				sv.data++;
				if(sv.size == 0) goto invalid;
				unsigned char c1 = *(unsigned char*)sv.data;
				if(0x80 <= c1 && c1 <= 0xBF) {
					sv.size--;
					sv.data++;
					if(sv.size == 0) goto invalid;
					unsigned char c2 = *(unsigned char*)sv.data;
					if(0x80 <= c2 && c2 <= 0xBF) {
						sv.size--;
						sv.data++;
					} else {
						goto invalid;
					}
				} else {
					goto invalid;
				}
			} else if(c == 0xF0) { // 4 byte
				sv.size--;
				sv.data++;
				if(sv.size == 0) goto invalid;
				unsigned char c1 = *(unsigned char*)sv.data;
				if(0x90 <= c1 && c1 <= 0xBF) {
					sv.size--;
					sv.data++;
					if(sv.size == 0) goto invalid;
					unsigned char c2 = *(unsigned char*)sv.data;
					if(0x80 <= c2 && c2 <= 0xBF) {
						sv.size--;
						sv.data++;
						if(sv.size == 0) goto invalid;
						unsigned char c3 = *(unsigned char*)sv.data;
						if(0x80 <= c3 && c3 <= 0xBF) {
							sv.size--;
							sv.data++;
						} else {
							goto invalid;
						}
					} else {
						goto invalid;
					}
				} else {
					goto invalid;
				}
			} else if(0xF1 <= c && c <= 0xF3) {
				sv.size--;
				sv.data++;
				if(sv.size == 0) goto invalid;
				unsigned char c1 = *(unsigned char*)sv.data;
				if(0x80 <= c1 && c1 <= 0xBF) {
					sv.size--;
					sv.data++;
					if(sv.size == 0) goto invalid;
					unsigned char c2 = *(unsigned char*)sv.data;
					if(0x80 <= c2 && c2 <= 0xBF) {
						sv.size--;
						sv.data++;
						if(sv.size == 0) goto invalid;
						unsigned char c3 = *(unsigned char*)sv.data;
						if(0x80 <= c3 && c3 <= 0xBF) {
							sv.size--;
							sv.data++;
						} else {
							goto invalid;
						}
					} else {
						goto invalid;
					}
				} else {
					goto invalid;
				}
			} else if(c == 0xF4) {
				sv.size--;
				sv.data++;
				if(sv.size == 0) goto invalid;
				unsigned char c1 = *(unsigned char*)sv.data;
				if(0x80 <= c1 && c1 <= 0x8F) {
					sv.size--;
					sv.data++;
					if(sv.size == 0) goto invalid;
					unsigned char c2 = *(unsigned char*)sv.data;
					if(0x80 <= c2 && c2 <= 0xBF) {
						sv.size--;
						sv.data++;
						if(sv.size == 0) goto invalid;
						unsigned char c3 = *(unsigned char*)sv.data;
						if(0x80 <= c3 && c3 <= 0xBF) {
							sv.size--;
							sv.data++;
						} else {
							goto invalid;
						}
					} else {
						goto invalid;
					}
				} else {
					goto invalid;
				}
			} else {
				goto invalid;
			}
		}
		return true;
	invalid:
		if(idx != NULL) *idx = ret;
		return false;
	}
	#if defined(CBUILD_API_POSIX) || defined(CBUILD_API_STRICT_POSIX)
		CBUILDDEF int cbuild_nproc(void) {
			// Reasoning: no alternatives are available and benefits are high-enough to go slightly off-spec
			#if defined(_SC_NPROCESSORS_ONLN) // Also used on strict POSIX
				return abs((int)sysconf(_SC_NPROCESSORS_ONLN));
			#else
				return 1;
			#endif // Extension check
		}
	#endif // CBUILD_API_*
	/* Command.h impl */
	CBUILDDEF cbuild_sb_t cbuild_cmd_to_sb(cbuild_cmd_t cmd) {
		cbuild_sb_t sb = {0};
		if(cmd.size < 1) {
			return sb;
		}
		for(size_t i = 0; i < cmd.size; i++) {
			const char* tmp = cmd.data[i];
			if(!strchr(tmp, ' ')) {
				cbuild_sb_append_cstr(&sb, tmp);
			} else {
				cbuild_sb_appendf(&sb, "\'%s\'", tmp);
			}
			if(i < cmd.size - 1) {
				cbuild_sb_append(&sb, ' ');
			}
		}
		return sb;
	}
	CBUILDDEF cbuild_proc_t cbuild_cmd_async(cbuild_cmd_t cmd) {
		cbuild_proc_t ret = CBUILD_INVALID_PROC;
		if(!cbuild_cmd_run(&cmd, .no_reset = true,
				.no_print_cmd = true, .proc = &ret)) {
			return CBUILD_INVALID_PROC;
		}
		return ret;
	}
	CBUILDDEF cbuild_proc_t cbuild_cmd_async_redirect(cbuild_cmd_t cmd,
		cbuild_cmd_fd_t fd) {
		cbuild_proc_t ret = CBUILD_INVALID_PROC;
		if(!cbuild_cmd_run(&cmd, .no_reset = true, .no_print_cmd = true, .proc = &ret,
				.fdstdin = fd.fdstdin == CBUILD_INVALID_FD ? NULL : &fd.fdstdin,
				.fdstdout = fd.fdstdout == CBUILD_INVALID_FD ? NULL : &fd.fdstdout,
				.fdstderr = fd.fdstderr == CBUILD_INVALID_FD ? NULL : &fd.fdstderr)) {
			return CBUILD_INVALID_PROC;
		}
		return ret;
	}
	CBUILDDEF bool cbuild_cmd_sync(cbuild_cmd_t cmd) {
		return cbuild_cmd_run(&cmd, .no_reset = true, .no_print_cmd = true);
	}
	CBUILDDEF bool cbuild_cmd_sync_redirect(cbuild_cmd_t cmd, cbuild_cmd_fd_t fd) {
		return cbuild_cmd_run(&cmd, .no_reset = true, .no_print_cmd = true,
			.fdstdin = fd.fdstdin == CBUILD_INVALID_FD ? NULL : &fd.fdstdin,
			.fdstdout = fd.fdstdout == CBUILD_INVALID_FD ? NULL : &fd.fdstdout,
			.fdstderr = fd.fdstderr == CBUILD_INVALID_FD ? NULL : &fd.fdstderr,);
	}
	#if defined(CBUILD_API_POSIX) || defined(CBUILD_API_STRICT_POSIX)
		// We needs opts here, because I dont want to bloat function signature when I will add more call-level flags
		CBUILDDEF cbuild_proc_t __cbuild_int_cmd_run_opt(cbuild_cmd_t* cmd,
			cbuild_cmd_opt_t* opts, cbuild_fd_t fdstdin, cbuild_fd_t fdstdout,
			cbuild_fd_t fdstderr) {
			// Get args
			cbuild_cmd_t argv = {0};
			cbuild_cmd_append_arr(&argv, cmd->data, cmd->size);
			cbuild_cmd_append(&argv, (char*)NULL);
			cbuild_proc_t proc = fork();
			if(proc < 0) {
				cbuild_log(CBUILD_LOG_ERROR, "Could not create child process, error: \"%s\"",
					strerror(errno));
				cbuild_cmd_clear(&argv);
				return false;
			}
			if(proc == 0) {
				fflush(NULL);
				// Setup stdin, stdout and stderr
				if(fdstdin != CBUILD_INVALID_FD) {
					if(dup2(fdstdin, STDIN_FILENO) < 0) {
						cbuild_log(
							CBUILD_LOG_ERROR,
							"Could not redirect stdin inside of a child process, error: \"%s\"",
							strerror(errno));
						cbuild_cmd_clear(&argv);
						exit(1);
					}
				}
				if(fdstdout != CBUILD_INVALID_FD) {
					if(dup2(fdstdout, STDOUT_FILENO) < 0) {
						cbuild_log(
							CBUILD_LOG_ERROR,
							"Could not redirect stdout inside of a child process, error: \"%s\"",
							strerror(errno));
						cbuild_sb_clear(&argv);
						exit(1);
					}
				}
				if(fdstderr != CBUILD_INVALID_FD) {
					if(dup2(fdstderr, STDERR_FILENO) < 0) {
						cbuild_log(
							CBUILD_LOG_ERROR,
							"Could not redirect stderr inside of a child process, error: \"%s\"",
							strerror(errno));
						cbuild_cmd_clear(&argv);
						exit(1);
					}
				}
				// Autokill
				if(opts->autokill) {
					#if defined(CBUILD_API_POSIX) && defined(CBUILD_OS_LINUX)
						prctl(PR_SET_PDEATHSIG, SIGKILL);
					#else
						cbuild_log(CBUILD_LOG_WARN, "Autokill is supported only on POSIX with Linux extensions!");
					#endif // Extension check
				}
				// Call command
				if(execvp(argv.data[0], (char* const*)argv.data) < 0) {
					cbuild_log(CBUILD_LOG_ERROR,
						"Could not execute command in child process, error: \"%s\"",
						strerror(errno));
					exit(1);
				}
				exit(0);
			}
			cbuild_cmd_clear(&argv);
			return proc;
		}
	#endif // CBUILD_API_*
	CBUILDDEF bool cbuild_cmd_run_opt(cbuild_cmd_t* cmd, cbuild_cmd_opt_t opts) {
		if(cmd->size == 0) {
			cbuild_log(CBUILD_LOG_ERROR, "Empty command requested to be executed!");
			return CBUILD_INVALID_PROC;
		}
		if(!opts.no_print_cmd) {
			cbuild_sb_t cmd_sb = cbuild_cmd_to_sb(*cmd);
			cbuild_log(CBUILD_LOG_TRACE,
				"Running command '"CBuildSBFmt"'", CBuildSBArg(cmd_sb));
			cbuild_sb_clear(&cmd_sb);
		}
		// Open files
		cbuild_fd_t fdstdin = CBUILD_INVALID_FD;
		cbuild_fd_t fdstdout = CBUILD_INVALID_FD;
		cbuild_fd_t fdstderr = CBUILD_INVALID_FD;
		cbuild_fd_t file_stdin = CBUILD_INVALID_FD;
		cbuild_fd_t file_stdout = CBUILD_INVALID_FD;
		cbuild_fd_t file_stderr = CBUILD_INVALID_FD;
		if(opts.fdstdin != NULL) fdstdin = *opts.fdstdin;
		if(opts.fdstdout != NULL) fdstdout = *opts.fdstdout;
		if(opts.fdstderr != NULL) fdstderr = *opts.fdstderr;
		if(opts.file_stdin) {
			file_stdin = cbuild_fd_open_read(opts.file_stdin);
			fdstdin = file_stdin;
		}
		if(opts.file_stdout) {
			file_stdout = cbuild_fd_open_write(opts.file_stdout);
			fdstdout = file_stdout;
		}
		if(opts.file_stderr) {
			file_stderr = cbuild_fd_open_write(opts.file_stderr);
			fdstderr = file_stderr;
		}
		// Check for limits
		size_t proc_idx = 0;
		int code = 0;
		if(opts.procs != NULL && opts.async_threads != -1) {
			if(opts.async_threads == 0) opts.async_threads = cbuild_nproc() + 1;
			if((size_t)opts.async_threads == opts.procs->size) {
				proc_idx = (size_t)cbuild_procs_wait_any(*opts.procs, &code);
				if(code != 0) {
					goto cleanup;
				}
				
			}
		}
		// Call app
		cbuild_proc_t proc = __cbuild_int_cmd_run_opt(cmd, &opts, fdstdin, fdstdout, fdstderr);
		// Finish process creation
		if(opts.proc != NULL) {
			*opts.proc = proc;
		} else if(opts.procs != NULL) {
			if(opts.async_threads != -1) {
				if((size_t)opts.async_threads == opts.procs->size) {
					cbuild_da_set(opts.procs, proc_idx, proc);
				} else {
					cbuild_da_append(opts.procs, proc);
				}
			} else {
				cbuild_da_append(opts.procs, proc);
			}
		} else {
			code = cbuild_proc_wait_code(proc);
		}
	cleanup:
		// Cleanup
		if(!opts.no_reset) {
			cmd->size = 0;
			if(fdstdin != CBUILD_INVALID_FD) cbuild_fd_close(fdstdin);
			if(fdstdout != CBUILD_INVALID_FD) cbuild_fd_close(fdstdout);
			if(fdstderr != CBUILD_INVALID_FD) cbuild_fd_close(fdstderr);
		} else {
			if(file_stdin != CBUILD_INVALID_FD) cbuild_fd_close(file_stdin);
			if(file_stdout != CBUILD_INVALID_FD) cbuild_fd_close(file_stdout);
			if(file_stderr != CBUILD_INVALID_FD) cbuild_fd_close(file_stderr);
		}
		return code == 0;
	}
	/* Log.h impl */
	static cbuild_log_level_t __CBUILD_LOG_MIN_LEVEL = CBUILD_LOG_MIN_LEVEL;
	char* __cbuild_int_log_level_names[CBUILD_LOG_PRINT] = { // Print is next index so should be fine
		[CBUILD_LOG_ERROR] = CBUILD_TERM_FG(CBUILD_TERM_RED)"[ERROR]"CBUILD_TERM_RESET" ",
		[CBUILD_LOG_WARN] = CBUILD_TERM_FG(CBUILD_TERM_YELLOW)"[WARN]"CBUILD_TERM_RESET" ",
		[CBUILD_LOG_INFO] = "[INFO] ",
		[CBUILD_LOG_TRACE] = CBUILD_TERM_FG(CBUILD_TERM_BRBLACK)"[TRACE]"CBUILD_TERM_RESET" ",
	};
	CBUILDDEF void __cbuild_int_vlog(const char* level, const char* fmt,
		va_list args) {
		time_t t = time(NULL);
		struct tm* tm_info = localtime(&t);
		__CBUILD_ERR_PRINTF("[%02d:%02d:%02d] ", tm_info->tm_hour, tm_info->tm_min,
			tm_info->tm_sec);
		__CBUILD_ERR_PRINT(level);
		__CBUILD_ERR_VPRINTF(fmt, args);
		__CBUILD_ERR_PRINT("\n");
	}
	CBUILDDEF void __cbuild_int_log(const char* level, const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
		__cbuild_int_vlog(level, fmt, args);
		va_end(args);
	}
	CBUILDDEF void cbuild_log(cbuild_log_level_t level, const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
		cbuild_vlog(level, fmt, args);
		va_end(args);
	}
	CBUILDDEF void cbuild_vlog(cbuild_log_level_t level, const char* fmt,
		va_list args) {
		if(level == CBUILD_LOG_NO_LOGS) {
			return;
		}
		if(level == CBUILD_LOG_PRINT) {
			__cbuild_int_vlog("", fmt, args);
		} else if (level <= __CBUILD_LOG_MIN_LEVEL) {
			__cbuild_int_vlog(__cbuild_int_log_level_names[level], fmt, args);
		}
	}
	CBUILDDEF void cbuild_log_set_min_level(cbuild_log_level_t level) {
		__CBUILD_LOG_MIN_LEVEL = level;
	}
	CBUILDDEF cbuild_log_level_t cbuild_log_get_min_level(void) {
		return __CBUILD_LOG_MIN_LEVEL;
	}
	/* Arena.h impl */
	size_t __cbuild_int_temp_size = 0;
	char __cbuild_int_temp[CBUILD_TEMP_ARENA_SIZE] = {0};
	CBUILDDEF void* cbuild_temp_alloc(size_t size) {
		if(size > CBUILD_TEMP_ARENA_SIZE - __cbuild_int_temp_size) return NULL;
		void* ptr = (void*)(__cbuild_int_temp + __cbuild_int_temp_size);
		__cbuild_int_temp_size += size;
		return ptr;
	}
	CBUILDDEF char* cbuild_temp_sprintf(const char* fmt, ...) {
		va_list va;
		va_start(va, fmt);
		char* ret = cbuild_temp_vsprintf(fmt, va);
		va_end(va);
		return ret;
	}
	CBUILDDEF char* cbuild_temp_vsprintf(const char* fmt, va_list ap) {
		va_list va;
		va_copy(va, ap);
		int n = vsnprintf(NULL, 0, fmt, va);
		va_end(va);
		if(n >= 0) {
			char* ret =	cbuild_temp_alloc((size_t)n + 1);
			vsnprintf(ret, (size_t)n + 1, fmt, ap);
			return ret;
		} else {
			return NULL;
		}
	}
	CBUILDDEF char* cbuild_temp_strdup(const char* str) {
		size_t len = strlen(str) + 1;
		char* dup = (char*)cbuild_temp_alloc(len);
		if(dup == NULL) return NULL;
		memcpy(dup, str, len);
		return dup;
	}
	CBUILDDEF void* cbuild_temp_memdup(const void* mem, size_t size) {
		char* dup = (char*)cbuild_temp_alloc(size);
		if(dup == NULL) return NULL;
		memcpy(dup, mem, size);
		return dup;
	}
	CBUILDDEF void cbuild_temp_reset(size_t checkpoint) {
		__cbuild_int_temp_size = checkpoint;
	}
	CBUILDDEF size_t cbuild_temp_checkpoint(void) {
		return __cbuild_int_temp_size;
	}
	/* Proc.h impl */
	#if defined(CBUILD_API_POSIX) || defined(CBUILD_API_STRICT_POSIX)
		CBUILDDEF int cbuild_proc_wait_code(cbuild_proc_t proc) {
			if(proc == CBUILD_INVALID_PROC) {
				return INT_MIN;
			}
			while(true) {
				int status = 0;
				errno = 0;
				if(waitpid(proc, &status, 0) < 0) {
					if(errno ==	ECHILD) {
						return INT_MAX;
					} else {
						cbuild_log(CBUILD_LOG_ERROR,
							"Could not wait for child process (pid %d), error: \"%s\"", proc,
							strerror(errno));
					}
					errno = 0;
				} else {
					if(WIFEXITED(status)) {
						int code = WEXITSTATUS(status);
						return code;
					}
					if(WIFSIGNALED(status)) {
						cbuild_log(CBUILD_LOG_ERROR,
							"Process (pid %d) was terminated by signal \"%d\"", proc,
							WTERMSIG(status));
						return -WTERMSIG(status);
					}
				}
			}
		}
		CBUILDDEF ssize_t cbuild_procs_wait_any(cbuild_proclist_t procs, int* code) {
			if(procs.size == 0) {
				return -1;
			}
			while(true) {
				for(size_t i = 0; i < procs.size; i++) {
					cbuild_proc_t proc = procs.data[i];
					int status = 0;
					errno = 0;
					int ret = waitpid(proc, &status, WNOHANG);
					if(ret < 0) {
						if(errno ==	ECHILD) {
							return INT_MAX;
						} else {
							cbuild_log(CBUILD_LOG_ERROR,
								"Could not wait for child process (pid %d), error: \"%s\"", proc,
								strerror(errno));
						}
						errno = 0;
					} else if(ret > 0) {
						if(WIFEXITED(status)) {
							if(code != NULL) *code = WEXITSTATUS(status);
							return (ssize_t)i;
						}
						if(WIFSIGNALED(status)) {
							cbuild_log(CBUILD_LOG_ERROR,
								"Process (pid %d) was terminated by signal \"%d\"", proc,
								WTERMSIG(status));
							if(code != NULL) *code = -WTERMSIG(status);
							return (ssize_t)i;
						}
					}
				}
			}
			return -1;
		}
		CBUILDDEF bool cbuild_proc_is_running(cbuild_proc_t proc) {
			if(proc <= 0) return false;
			return kill(proc, 0) <= 0;
		}
		CBUILDDEF cbuild_proc_ptr_t cbuild_proc_malloc(size_t n) {
			// Reasoning: Tooo big benefits
			#if defined(MAP_ANONYMOUS) // Also used on strict POSIX
				void* ptr =
					mmap(NULL, n, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
			#elif defined(MAP_ANON) // Also used on strict POSIX
				void* ptr =
					mmap(NULL, n, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
			#else // Strict POSIX fallback
				int (*shm_open)(const char* name, int oflag, mode_t mode) =
					(int (*)(const char*, int, mode_t))dlsym(RTLD_DEFAULT, "shm_open");
				if(shm_open == NULL) {
					if(!dlopen("librt.so", RTLD_LAZY | RTLD_GLOBAL)) {
						if(!dlopen("librt.so.1", RTLD_LAZY | RTLD_GLOBAL)) {
							CBUILD_UNREACHABLE("libc dont expose 'shm_open' and librt not found!");
						}
					}
					shm_open = (int (*)(const char*, int, mode_t))dlsym(RTLD_DEFAULT, "shm_open");
				}
				int (*shm_unlink)(const char* name) =
					(int (*)(const char*))dlsym(RTLD_DEFAULT, "shm_unlink");
				int shmemfd = shm_open("/cbuild-proc-malloc-shmem", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
				if(shmemfd == -1) {
					return (cbuild_proc_ptr_t) {
						.ptr = NULL, .size = 0
					};
				}
				if(ftruncate(shmemfd, (long)n) == -1) {
					shm_unlink("/cbuild-proc-malloc-shmem");
					close(shmemfd);
					return (cbuild_proc_ptr_t) {
						.ptr = NULL, .size = 0
					};
				}
				void* ptr =
					mmap(NULL, n, PROT_READ | PROT_WRITE, MAP_SHARED, shmemfd, 0);
				if(ptr == MAP_FAILED) {
					close(shmemfd);
					shm_unlink("/cbuild-proc-malloc-shmem");
					return (cbuild_proc_ptr_t) {
						.ptr = NULL, .size = 0
					};
				} else {
					close(shmemfd);
					shm_unlink("/cbuild-proc-malloc-shmem");
					return (cbuild_proc_ptr_t) {
						.ptr = ptr, .size = n
					};
				}
			#endif // Extension check
			if(ptr == MAP_FAILED) {
				return (cbuild_proc_ptr_t) {
					.ptr = NULL, .size = 0
				};
			} else {
				return (cbuild_proc_ptr_t) {
					.ptr = ptr, .size = n
				};
			}
		}
		CBUILDDEF void cbuild_proc_free(cbuild_proc_ptr_t ptr) {
			if(ptr.ptr != NULL) {
				munmap(ptr.ptr, ptr.size);
			}
		}
		CBUILDDEF cbuild_proc_t cbuild_proc_start(int (*callback)(void* context),
			void* context) {
			cbuild_proc_t proc = fork();
			if(proc < 0) {
				cbuild_log(CBUILD_LOG_ERROR, "Could not create child process, error: \"%s\"",
					strerror(errno));
				return CBUILD_INVALID_PROC;
			}
			if(proc == 0) {
				int code = callback(context);
				exit(code);
			}
			return proc;
		}
	#endif // CBUILD_API_*
	CBUILDDEF bool cbuild_proc_wait(cbuild_proc_t proc) {
		return cbuild_proc_wait_code(proc) == 0;
	}
	CBUILDDEF bool cbuild_procs_wait(cbuild_proclist_t procs) {
		bool ret = true;
		cbuild_da_foreach(procs, proc) {
			if(!cbuild_proc_wait(*proc)) ret = false;
		}
		return ret;
	}
	/* FS.h impl */
	#if defined(CBUILD_API_POSIX) || defined(CBUILD_API_STRICT_POSIX)
		CBUILDDEF bool cbuild_fd_close(cbuild_fd_t fd) {
			if(fd == CBUILD_INVALID_FD) {
				cbuild_log(CBUILD_LOG_ERROR,
					"Invalid file descriptor passed to close, error: \"%s\"",
					strerror(errno));
				return false;
			}
			return close(fd) < 0;
		}
		CBUILDDEF cbuild_fd_t cbuild_fd_open_read(const char* path) {
			cbuild_fd_t fd = open(path, O_RDONLY);
			if(fd < 0) {
				cbuild_log(CBUILD_LOG_ERROR,
					"Could not open file \"%s\" for reading, error: \"%s\"", path,
					strerror(errno));
				return CBUILD_INVALID_FD;
			}
			fcntl(fd, F_SETFD, FD_CLOEXEC);
			return fd;
		}
		CBUILDDEF cbuild_fd_t cbuild_fd_open_write(const char* path) {
			cbuild_fd_t fd = open(path, O_WRONLY | O_CREAT | O_TRUNC,
				S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
			if(fd < 0) {
				cbuild_log(CBUILD_LOG_ERROR,
					"Could not open file \"%s\" for writing, error: \"%s\"", path,
					strerror(errno));
				return CBUILD_INVALID_FD;
			}
			fcntl(fd, F_SETFD, FD_CLOEXEC);
			return fd;
		}
		CBUILDDEF bool cbuild_fd_open_pipe(cbuild_fd_t* read, cbuild_fd_t* write) {
			cbuild_fd_t fds[2];
			int ret = pipe(fds);
			if(ret < 0) {
				cbuild_log(CBUILD_LOG_ERROR, "Could not create pipe, error: \"%s\"",
					strerror(errno));
				*read = CBUILD_INVALID_FD;
				*write = CBUILD_INVALID_FD;
				return false;
			}
			*read = fds[0];
			*write = fds[1];
			fcntl(*read, F_SETFD, FD_CLOEXEC);
			fcntl(*write, F_SETFD, FD_CLOEXEC);
			return true;
		}
		CBUILDDEF ssize_t cbuild_fd_read(cbuild_fd_t fd, void* buf, size_t nbytes) {
			ssize_t len = read(fd, buf, nbytes);
			if(len < 0) {
				cbuild_log(CBUILD_LOG_ERROR,
					"Could not execute 'cbuild_fd_read', error: \"%s\"", strerror(errno));
			}
			return len;
		}
		CBUILDDEF ssize_t cbuild_fd_read_file(cbuild_fd_t fd, void* buf,
			size_t nbytes, const char* path) {
			ssize_t len = read(fd, buf, nbytes);
			if(len < 0) {
				cbuild_log(CBUILD_LOG_ERROR,
					"Could not read from file \"%s\", error: \"%s\"", path,
					strerror(errno));
			}
			return len;
		}
		CBUILDDEF ssize_t cbuild_fd_write(cbuild_fd_t fd, const void* buf,
			size_t nbytes) {
			ssize_t len = write(fd, buf, nbytes);
			if(len < 0) {
				cbuild_log(CBUILD_LOG_ERROR,
					"Could not execute 'cbuild_fd_write', error: \"%s\"", strerror(errno));
			}
			return len;
		}
		CBUILDDEF ssize_t cbuild_fd_write_file(cbuild_fd_t fd, const void* buf,
			size_t nbytes, const char* path) {
			ssize_t len = write(fd, buf, nbytes);
			if(len < 0) {
				cbuild_log(CBUILD_LOG_ERROR,
					"Could not write to file \"%s\", error: \"%s\"", path,
					strerror(errno));
			}
			return len;
		}
		CBUILDDEF ssize_t cbuild_file_len(const char* path) {
			struct stat statbuff;
			if(stat(path, &statbuff) < 0) {
				cbuild_log(CBUILD_LOG_ERROR, "Could not stat \"%s\", error: \"%s\"", path,
					strerror(errno));
				return -1;
			}
			return statbuff.st_size;
		}
	#endif // CBUILD_API_*
	CBUILDDEF bool cbuild_file_read(const char* path, cbuild_sb_t* data) {
		cbuild_fd_t fd = cbuild_fd_open_read(path);
		if(CBUILD_INVALID_FD == fd) {
			return false;
		}
		size_t size = (size_t)cbuild_file_len(path);
		cbuild_sb_resize(data, size + 1); // Assert data != NULL
		ssize_t len = cbuild_fd_read_file(fd, data->data, size, path);
		if(len < 0) {
			cbuild_fd_close(fd);
			return false;
		}
		data->size = (size_t)len;
		data->capacity = (size_t)len;
		cbuild_fd_close(fd);
		return true;
	}
	CBUILDDEF bool cbuild_file_write(const char* path, cbuild_sb_t* data) {
		cbuild_fd_t fd = cbuild_fd_open_write(path);
		if(CBUILD_INVALID_FD == fd) {
			return false;
		}
		char* buf = data->data;
		ssize_t cnt = (ssize_t)data->size;
		while(cnt > 0) {
			ssize_t written = cbuild_fd_write_file(fd, buf, (size_t)cnt, path);
			if(written < 0) {
				cbuild_fd_close(fd);
				return false;
			}
			cnt -= written;
			buf += written;
		}
		cbuild_fd_close(fd);
		return true;
	}
	CBUILDDEF bool cbuild_file_copy(const char* src, const char* dst) {
		cbuild_fd_t src_fd = cbuild_fd_open_read(src);
		cbuild_fd_t dst_fd = cbuild_fd_open_write(dst);
		if(src_fd == CBUILD_INVALID_FD) {
			return false;
		}
		if(dst_fd == CBUILD_INVALID_FD) {
			cbuild_fd_close(src_fd);
			return false;
		}
		char* tmp_buff = (char*)cbuild_malloc(CBUILD_TMP_BUFF_SIZE);
		cbuild_assert(tmp_buff != NULL, "(LIB_CBUILD_FS) Allocation failed.y\n");
		while(true) {
			ssize_t cnt = cbuild_fd_read_file(src_fd, tmp_buff, CBUILD_TMP_BUFF_SIZE, src);
			if(cnt == 0) {
				break;
			}
			if(cnt < 0) {
				cbuild_fd_close(src_fd);
				cbuild_fd_close(dst_fd);
				return false;
			}
			char* buf = tmp_buff;
			while(cnt > 0) {
				ssize_t written = cbuild_fd_write_file(dst_fd, buf, (size_t)cnt, dst);
				if(written < 0) {
					cbuild_fd_close(src_fd);
					cbuild_fd_close(dst_fd);
					cbuild_free(tmp_buff);
					return false;
				}
				cnt -= written;
				buf += written;
			}
		}
		cbuild_fd_close(src_fd);
		cbuild_fd_close(dst_fd);
		cbuild_free(tmp_buff);
		return true;
	}
	#if defined(CBUILD_API_POSIX) || defined(CBUILD_API_STRICT_POSIX)
		CBUILDDEF bool cbuild_file_move(const char* src, const char* dst) {
			if(rename(src, dst) == 0) {
				return true;
			}
			if(errno == EXDEV) {
				if(!cbuild_file_copy(src, dst)) return false;
				return cbuild_file_remove(src);
			}
			return false;
		}
	#endif // CBUILD_API_*
	CBUILDDEF bool cbuild_file_rename(const char* src, const char* dst) {
		return cbuild_file_move(src, dst);
	}
	CBUILDDEF bool cbuild_file_check(const char* path) {
		return !(cbuild_file_len(path) < 0);
	}
	#if defined(CBUILD_API_POSIX) || defined(CBUILD_API_STRICT_POSIX)
		CBUILDDEF bool cbuild_file_remove(const char* path) {
			if(unlink(path) < 0) {
				cbuild_log(CBUILD_LOG_ERROR, "Could not remove file \"%s\", error: \"%s\"",
					path, strerror(errno));
				return false;
			}
			return true;
		}
		CBUILDDEF bool cbuild_symlink(const char* src, const char* dst) {
			char* base = cbuild_path_base(dst);
			if(*base) {
				if(!cbuild_dir_check(base)) {
					if(!cbuild_dir_create(base)) {
						cbuild_log(CBUILD_LOG_ERROR,
							"Destination base path \"%s\" is invalid!", base);
						cbuild_free(base);
						return false;
					}
				}
			}
			cbuild_free(base);
			int ret = symlink(src, dst);
			if(ret < 0) {
				if(errno == EEXIST) {
					cbuild_filetype_t type = cbuild_path_filetype(dst);
					switch(type) {
					case CBUILD_FTYPE_DIRECTORY: cbuild_dir_remove(dst); break;
					case CBUILD_FTYPE_REGULAR:
					case CBUILD_FTYPE_SYMLINK:
					case CBUILD_FTYPE_OTHER:
						cbuild_file_remove(dst);
						break;
					case CBUILD_FTYPE_MISSING:
						CBUILD_UNREACHABLE("File cannot be missing if it exists.");
						break;
					default: CBUILD_UNREACHABLE("Invalid filetype in create_symlink.");
					}
					if(symlink(src, dst) == 0) return true;
				}
				cbuild_log(CBUILD_LOG_ERROR,
					"Could not create symbolic link \"%s\", error: \"%s\"",
					dst, strerror(errno));
				return false;
			}
			return true;
		}
	#endif // CBUILD_API_*
	CBUILDDEF bool cbuild_dir_copy(const char* src, const char* dst) {
		bool err = cbuild_dir_create(dst);
		if(err == false) {
			cbuild_log(CBUILD_LOG_ERROR, "Could not create destination directory \"%s\"",
				dst);
			return false;
		}
		cbuild_pathlist_t list = {0};
		err = cbuild_dir_list(src, &list);
		if(err == false) {
			cbuild_log(CBUILD_LOG_ERROR, "Could not list source directory \"%s\"", src);
			if(list.data != NULL) {
				cbuild_pathlist_clear(&list);
			}
			return false;
		}
		bool ret = true;
		for(size_t i = 0; i < list.size; i++) {
			size_t lensrc = strlen(src) + 1 + strlen(list.data[i]);
			char* tmpsrc = cbuild_malloc(lensrc + 1);
			cbuild_assert(tmpsrc != NULL, "(LIB_CBUILD_SB) Allocation failed.\n");
			sprintf(tmpsrc, "%s/%s", src, list.data[i]);
			size_t lendst = strlen(dst) + 1 + strlen(list.data[i]);
			char* tmpdst = cbuild_malloc(lendst + 1);
			cbuild_assert(tmpdst != NULL, "(LIB_CBUILD_SB) Allocation failed.\n");
			sprintf(tmpdst, "%s/%s", dst, list.data[i]);
			cbuild_filetype_t f = cbuild_path_filetype(tmpsrc);
			if (f == CBUILD_FTYPE_MISSING) {
				CBUILD_UNREACHABLE("cbuild_dir_list should not return invalid files.");
			} else if(f == CBUILD_FTYPE_DIRECTORY) {
				bool tmp = ret && cbuild_dir_copy(tmpsrc, tmpdst);
				ret = tmp;
			} else {
				bool tmp = ret && cbuild_file_copy(tmpsrc, tmpdst);
				ret = tmp;
			}
			cbuild_free(tmpsrc);
			cbuild_free(tmpdst);
		}
		cbuild_pathlist_clear(&list);
		return ret;
	}
	CBUILDDEF bool cbuild_dir_move(const char* src, const char* dst) {
		bool ret = cbuild_dir_copy(src, dst);
		if(ret == false) {
			return false;
		}
		return cbuild_dir_remove(src);
	}
	CBUILDDEF bool cbuild_dir_rename(const char* src, const char* dst) {
		return cbuild_dir_move(src, dst);
	}
	CBUILDDEF bool __cbuild_int_fs_rmdir(const char* path);
	#if defined(CBUILD_API_POSIX) || defined(CBUILD_API_STRICT_POSIX)
		CBUILDDEF bool __cbuild_int_fs_rmdir(const char* path) {
			int stat = rmdir(path);
			if(stat < 0) {
				cbuild_log(CBUILD_LOG_ERROR,
					"Could not remove directory directory \"%s\", error: \"%s\"", path,
					strerror(errno));
				return false;
			}
			return true;
		}
	#endif // CBUILD_API_*
	CBUILDDEF bool cbuild_dir_remove(const char* path) {
		cbuild_pathlist_t list = {0};
		bool err = cbuild_dir_list(path, &list);
		if(err == false) {
			cbuild_log(CBUILD_LOG_ERROR,
				"Could not remove source directory \"%s\", error: \"%s\"", path,
				strerror(errno));
			if(list.data != NULL) {
				cbuild_pathlist_clear(&list);
			}
			return false;
		}
		bool ret = true;
		for(size_t i = 0; i < list.size; i++) {
			size_t lenpath = strlen(path) + 1 + strlen(list.data[i]);
			char* tmppath = cbuild_malloc(lenpath + 1);
			cbuild_assert(tmppath != NULL, "(LIB_CBUILD_SB) Allocation failed.\n");
			sprintf(tmppath, "%s/%s", path, list.data[i]);
			cbuild_filetype_t f = cbuild_path_filetype(tmppath);
			if (f == CBUILD_FTYPE_MISSING) {
				CBUILD_UNREACHABLE("cbuild_dir_list should not return invalid files.");
			} else if(f == CBUILD_FTYPE_DIRECTORY) {
				if(!cbuild_dir_remove(tmppath)) ret = false;
			} else {
				if(!cbuild_file_remove(tmppath)) ret = false;
			}
			cbuild_free(tmppath);
		}
		if(!__cbuild_int_fs_rmdir(path)) {
			cbuild_pathlist_clear(&list);
			return false;
		}
		cbuild_pathlist_clear(&list);
		return ret;
	}
	CBUILDDEF bool cbuild_dir_check(const char* path) {
		return cbuild_file_check(path);
	}
	#if defined(CBUILD_API_POSIX) || defined(CBUILD_API_STRICT_POSIX)
		CBUILDDEF int __cbuild_int_fs_dir_list_no_dots(const struct dirent *d) {
			return !(strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0);
		}
		CBUILDDEF int __cbuild_int_fs_compare(const void* a, const void* b) {
			return strcmp(*(const char**)a, *(const char**)b);
		}
		CBUILDDEF bool cbuild_dir_list(const char* path, cbuild_pathlist_t* elements) {
			cbuild_da_clear(elements);
			#if (defined(_POSIX_C_SOURCE) && (_POSIX_C_SOURCE >= 200809L)) || \
				(defined(CBUILD_API_POSIX) && ( \
						defined(CBUILD_OS_LINUX_GLIBC) || \
						defined(CBUILD_OS_BSD) || \
						defined(CBUILD_OS_MACOS)))
				struct dirent** namelist;
				int n = scandir(path, &namelist, __cbuild_int_fs_dir_list_no_dots, alphasort);
				if(n < 0) {
					cbuild_log(CBUILD_LOG_ERROR, "Could not list directory \"%s\", error: \"%s\"",
						path, strerror(errno));
					return false;
				}
				for(int i = 0; i < n; i++) {
					size_t len = strlen(namelist[i]->d_name);
					char* str = (char*)cbuild_malloc(len + 1);
					cbuild_assert(str != NULL, "(LIB_CBUILD_FS) Allocation failed.\n");
					memcpy(str, namelist[i]->d_name, len);
					str[len] = '\0';
					cbuild_da_append(elements, str);
					free(namelist[i]);
				}
				free(namelist);
			#else // POSIX 2001 fallback
				DIR* dir = opendir(path);
				if(!dir) {
					cbuild_log(CBUILD_LOG_ERROR, "Could not open directory \"%s\", error: \"%s\"",
						path, strerror(errno));
					return false;
				}
				struct dirent* entry;
				while((entry = readdir(dir)) != NULL) {
					if(!__cbuild_int_fs_dir_list_no_dots(entry)) continue;
					size_t len = strlen(entry->d_name);
					char* str = (char*)cbuild_malloc(len + 1);
					cbuild_assert(str != NULL, "(LIB_CBUILD_FS) Allocation failed.\n");
					memcpy(str, entry->d_name, len);
					str[len] = '\0';
					cbuild_da_append(elements, str);
				}
				closedir(dir);
				qsort(elements->data, elements->size, sizeof(char*), __cbuild_int_fs_compare);
			#endif // Extension check
			return true;
		}
		CBUILDDEF char* cbuild_dir_current(void) {
			#if defined(CBUILD_API_POSIX) && \
				(defined(CBUILD_OS_LINUX_GLIBC) || defined(CBUILD_OS_LINUX_MUSL))
				return get_current_dir_name();
			#else
				size_t size;
				long path_max = pathconf(".", _PC_PATH_MAX);
				if (path_max == -1) size = 1024;
				else if (path_max > 10240) size = 10240;
				else size = (size_t)path_max;
				while(true) {
					char* buf = malloc(size);
					cbuild_assert(buf != NULL, "(LIB_CBUILD_FS) Allocation failed.\n");
					errno = 0;
					if(getcwd(buf, size) == NULL) {
						if(errno == ERANGE) {
							size = size*2;
						} else {
							free(buf);
							return NULL;
						}
					} else {
						return buf;
					}
				}
				return NULL;
			#endif // Extension check
		}
		CBUILDDEF bool cbuild_dir_set_current(const char* path) {
			if(chdir(path) < 0) {
				cbuild_log(CBUILD_LOG_ERROR,
					"Could not set current working directory to \"%s\", error: \"%s\"",
					path, strerror(errno));
				return false;
			}
			return true;
		}
	#endif // CBUILD_API_*
	CBUILDDEF void cbuild_pathlist_clear(cbuild_pathlist_t* list) {
		for(size_t i = 0; i < list->size; i++) {
			free((void*)list->data[i]);
		}
		cbuild_da_clear(list);
	}
	#if defined(CBUILD_API_POSIX) || defined(CBUILD_API_STRICT_POSIX)
		CBUILDDEF bool __cbuild_dir_create_int(const char* path_, bool inplace) {
			int ret = mkdir(path_, 0755);
			if(ret < 0) {
				if(errno == EEXIST) {
					cbuild_log(CBUILD_LOG_WARN, "Directory \"%s\" exist", path_);
					return false;
				} else if(errno == ENOENT) {
					char* path = (char*)path_;
					if(!inplace) path = cbuild_path_normalize(path_);
					cbuild_log_level_t old_log_level = cbuild_log_get_min_level();
					if(!inplace) cbuild_log_set_min_level(CBUILD_LOG_ERROR);
					char* slash = strrchr(path, '/');
					if(slash) *slash = '\0';
					if(!__cbuild_dir_create_int(path, true)) {
						if(!inplace) {
							cbuild_free(path);
							cbuild_log_set_min_level(old_log_level);
						}
						return false;
					}
					if(slash) *slash = '/';
					ret = mkdir(path, 0755);
					if(ret == 0) {
						if(!inplace) {
							cbuild_free(path);
							cbuild_log_set_min_level(old_log_level);
						}
						return true;
					}
					if(!inplace) {
						cbuild_free(path);
						cbuild_log_set_min_level(old_log_level);
					}
				}
				cbuild_log(CBUILD_LOG_ERROR,
					"Could not create directory \"%s\", error: \"%s\"", path_,
					strerror(errno));
				return false;
			}
			return true;
		}
		CBUILDDEF bool cbuild_dir_create(const char* path) {
			return __cbuild_dir_create_int(path, false);
		}
		CBUILDDEF cbuild_filetype_t cbuild_path_filetype(const char* path) {
			struct stat statbuff;
			if(lstat(path, &statbuff) < 0) {
				cbuild_log(CBUILD_LOG_ERROR, "Could not stat file \"%s\", error: \"%s\"", path,
					strerror(errno));
				return CBUILD_FTYPE_MISSING;
			}
			if(S_ISREG(statbuff.st_mode)) {
				return CBUILD_FTYPE_REGULAR;
			}
			if(S_ISDIR(statbuff.st_mode)) {
				return CBUILD_FTYPE_DIRECTORY;
			}
			if(S_ISLNK(statbuff.st_mode)) {
				return CBUILD_FTYPE_SYMLINK;
			}
			return CBUILD_FTYPE_OTHER;
		}
	#endif // CBUILD_API_*
	CBUILDDEF char* cbuild_path_ext(const char* path) {
		ssize_t i = (ssize_t)strlen(path);
		bool found = false;
		for(; i >= 0; i--) {
			if(path[i] == '.') {
				found = true;
				break;
			}
		}
		if(!found) {
			char* ret = cbuild_malloc(1);
			cbuild_assert(ret != NULL, "(LIB_CBUILD_SB) Allocation failed.\n");
			memcpy(ret, "\0", 1);
			return ret;
		}
		size_t len = strlen(path) - (size_t)i + 1;
		char* ret = (char*)cbuild_malloc(len);
		cbuild_assert(ret != NULL, "(LIB_CBUILD_SB) Allocation failed.\n");
		memcpy(ret, path + i + 1, len);
		return ret;
	}
	CBUILDDEF char* cbuild_path_name(const char* path) {
		ssize_t i = (ssize_t)strlen(path);
		if(path[i - 1] == '/') {
			i -= 2;
		}
		for(; i >= 0; i--) {
			if(path[i] == '/') {
				break;
			}
		}
		size_t len = strlen(path) - (size_t)i + 1;
		char* ret = (char*)cbuild_malloc(len);
		cbuild_assert(ret != NULL, "(LIB_CBUILD_SB) Allocation failed.\n");
		memcpy(ret, path + i + 1, len);
		return ret;
	}
	CBUILDDEF char* cbuild_path_base(const char* path) {
		ssize_t i = (ssize_t)strlen(path);
		if(path[i - 1] == '/') {
			i -= 2;
		}
		bool found = false;
		for(; i >= 0; i--) {
			if(path[i] == '/') {
				found = true;
				break;
			}
		}
		if(!found) {
			char* ret = cbuild_malloc(1);
			cbuild_assert(ret != NULL, "(LIB_CBUILD_SB) Allocation failed.\n");
			memcpy(ret, "\0", 1);
			return ret;
		}
		size_t len = (size_t)i + 2;
		char* ret = (char*)cbuild_malloc(len);
		cbuild_assert(ret != NULL, "(LIB_CBUILD_SB) Allocation failed.\n");
		memcpy(ret, path, len - 1);
		ret[len - 1] = '\0';
		return ret;
	}
	typedef struct __cbuild_int_stack_cstr_t {
		cbuild_sv_t* data;
		size_t ptr;
		size_t capacity;
	} __cbuild_int_stack_cstr_t;
	CBUILDDEF char* cbuild_path_normalize(const char* path_) {
		cbuild_sb_t ret = {0};
		__cbuild_int_stack_cstr_t dirs = {0};
		cbuild_sv_t path = cbuild_sv_from_cstr(path_);
		if(*path_ == '/') cbuild_sb_append(&ret, '/');
		#if defined(CBUILD_API_POSIX) || defined(CBUILD_API_STRICT_POSIX)
			if(cbuild_sv_prefix(path, cbuild_sv_from_lit("//")) &&
				!cbuild_sv_prefix(path, cbuild_sv_from_lit("///"))) {
				cbuild_sb_append(&ret, '/');
			}
		#endif
		do {
			cbuild_sv_t dir = cbuild_sv_chop_by_delim(&path, '/');
			if(dir.size == 0) continue;
			if(cbuild_sv_cmp(dir, cbuild_sv_from_lit(".")) == 0) {
				// Do nothing
			} else if(cbuild_sv_cmp(dir, cbuild_sv_from_lit("..")) == 0) {
				if(dirs.ptr == 0) { // Underflow
					cbuild_sb_append_cstr(&ret, "../");
					// Underflow on absolute path is undefined anyway
					// and on relative path we can guarantee that this will be fine
					// (we have nothing in directory stack anyway)
				} else {
					cbuild_stack_pop(&dirs);
				}
			} else {
				cbuild_stack_push(&dirs, dir);
			}
		} while(path.size > 0);
		for(size_t i = 0; i <	dirs.ptr; i++) {
			cbuild_sb_appendf(&ret, CBuildSVFmt"/", CBuildSVArg(dirs.data[i]));
		}
		if(ret.size == 0) cbuild_sb_append(&ret, '.');
		if(!((ret.size == 1 && ret.data[0] == '/') ||
				(ret.size == 2 && ret.data[0] == '/' && ret.data[1] == '/')) &&
			(ret.data[ret.size - 1] == '/')) ret.size--;
		cbuild_sb_append_null(&ret);
		cbuild_stack_clear(&dirs);
		return ret.data;
	}
	/* Compile.h impl */
	CBUILDDEF void __cbuild_int_compile_mark_exec(const char* file);
	#if defined(CBUILD_API_POSIX) || defined(CBUILD_API_STRICT_POSIX)
		CBUILDDEF void __cbuild_int_compile_mark_exec(const char* file) {
			if(chmod(file, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH |
					S_IXOTH) != 0) {
				cbuild_log(CBUILD_LOG_ERROR, "Could not mark file as executable!");
			}
		}
	#endif // CBUILD_API_*
	void (*cbuild_selfrebuild_hook)(cbuild_cmd_t* cmd) = NULL;
	CBUILDDEF void __cbuild_selfrebuild(int argc, char** argv, size_t num_files, ...) {
		va_list va;
		cbuild_cmd_t files = {0};;
		va_start(va, num_files);
		for(size_t i = 0; i < num_files; i++) {
			char* arg = va_arg(va, char*);
			cbuild_da_append(&files, arg);
		}
		va_end(va);
		__cbuild_selfrebuild_ex(argc, argv, files);
		cbuild_cmd_clear(&files);
	}
	CBUILDDEF void __cbuild_selfrebuild_ex(int argc, char** argv,
		cbuild_cmd_t files) {
		__cbuild_selfrebuild_ex_argv(argc, argv, argv[0], files);
	}
	CBUILDDEF void __cbuild_selfrebuild_ex_argv(int argc, char** argv, 
		const char* argv0_path, cbuild_cmd_t files) {
		const char* spath = files.data[0];
		char* bname_new_raw = cbuild_shift(argv, argc);
		const char* bname_new = argv0_path; 
		cbuild_sb_t bname_old = {0};
		cbuild_sb_append_cstr(&bname_old, bname_new);
		cbuild_sb_append_cstr(&bname_old, ".old");
		cbuild_sb_append_null(&bname_old);
		int cond = cbuild_compare_mtime_many(bname_new, files.data, files.size);
		if(cond < 0) {
			cbuild_log(CBUILD_LOG_ERROR, "Error while performing self-rebuild");
			cbuild_sb_clear(&bname_old);
			exit(1);
		}
		if(cond == 0) {
			cbuild_sb_clear(&bname_old);
			return;
		}
		cbuild_log(CBUILD_LOG_INFO, "Rebuilding CBuild buildscript");
		if(!cbuild_file_rename(bname_new, bname_old.data)) {
			cbuild_log(CBUILD_LOG_ERROR, "Could not rename old buildscript!");
			cbuild_sb_clear(&bname_old);
			exit(1);
		}
		cbuild_cmd_t cmd = {0};
		cbuild_cmd_append_many(&cmd, CBUILD_CC, CBUILD_SELFREBUILD_ARGS);
		if(cbuild_selfrebuild_hook != NULL) {
			cbuild_selfrebuild_hook(&cmd);
		}
		cbuild_cmd_append_many(&cmd, CBUILD_CC_OUT, (char*)bname_new, (char*)spath);
		if(!cbuild_cmd_run(&cmd)) {
			cbuild_file_rename(bname_old.data, bname_new);
			cbuild_log(CBUILD_LOG_TRACE, "Rebuild failed\n");
			return; // If compilation failed the let old version run
		}
		__cbuild_int_compile_mark_exec(bname_new);
		cbuild_cmd_append(&cmd, bname_new_raw);
		cbuild_cmd_append_arr(&cmd, argv, (size_t)argc);
		if(!cbuild_cmd_run(&cmd)) {
			cbuild_sb_clear(&bname_old);
			exit(1);
		}
		cbuild_sb_clear(&bname_old);
		exit(0);
	}
	#if defined(CBUILD_API_POSIX) || defined(CBUILD_API_STRICT_POSIX)
		CBUILDDEF int cbuild_compare_mtime(const char* output, const char* input) {
			struct stat statbuff;
			if(stat(input, &statbuff) < 0) {
				cbuild_log(CBUILD_LOG_ERROR, "Could not stat file \"%s\", error: \"%s\"",
					input, strerror(errno));
				return -1;
			}
			time_t in_mtime = statbuff.st_mtime;
			if(stat(output, &statbuff) < 0) {
				if(errno == ENOENT) {
					return 1;
				}
				cbuild_log(CBUILD_LOG_ERROR, "Could not stat file \"%s\", error: \"%s\"",
					output, strerror(errno));
				return -1;
			}
			if(in_mtime > statbuff.st_mtime) {
				return 1;
			} else {
				return 0;
			}
		}
	#endif //CBUILD_API_*
	CBUILDDEF int cbuild_compare_mtime_many(const char* output, const char** inputs,
		size_t num_inputs) {
		int ret = 0;
		for(size_t i = 0; i < num_inputs; i++) {
			int check = cbuild_compare_mtime(output, inputs[i]);
			if(check < 0) {
				return check;
			} else {
				ret += check;
			}
		}
		return ret;
	}
	/* Map.h impl */
	CBUILDDEF size_t __cbuild_int_map_hash_func(const void* data, size_t len) {
		const unsigned char* ucPtr = (const unsigned char*)data;
		size_t hash = 5381;
		for(size_t i = 0; i < len; i++) {
			hash = ((hash << 5) + hash) + ucPtr[i]; // hash * 33 + data[i]
		}
		return hash;
	}
	CBUILDDEF size_t __cbuild_int_map_get_hash(const cbuild_map_t* map, const void* key) {
		size_t hash = 0;
		if(map->hash_func == NULL) {
			if(map->key_size > 0) {
				hash = CBUILD_MAP_DEFAULT_HASH_FUNC(key, map->key_size)
				% map->nbuckets;
			} else {
				hash = CBUILD_MAP_DEFAULT_HASH_FUNC(*((void**)key), strlen(*((char**)key)))
				% map->nbuckets;
			}
		} else {
			hash = map->hash_func(map, key) % map->nbuckets;
		}
		return hash;
	}
	CBUILDDEF void* __cbuild_int_map_check_bucket(const cbuild_map_t* map,
		const cbuild_map_bucket_t* bucket, const void* key) {
		if(map->keycmp_func == NULL) {
			if(map->key_size > 0) {
				for(size_t i = 0; i < bucket->nvals; i++) {
					if(memcmp(key, ((char*)bucket->vals + (i * map->elem_size)),
							map->key_size) == 0) {
						return ((char*)bucket->vals + (i * map->elem_size));
					}
				}
			} else {
				for(size_t i = 0; i < bucket->nvals; i++) {
					if(strcmp(*((char**)key),
							*(char**)(void*)(((char*)bucket->vals + (i * map->elem_size)))) == 0) {
						return ((char*)bucket->vals + (i * map->elem_size));
					}
				}
			}
		} else {
			for(size_t i = 0; i < bucket->nvals; i++) {
				if(map->keycmp_func(map, key,
						((char*)bucket->vals + (i * map->elem_size)))) {
					return ((char*)bucket->vals + (i * map->elem_size));
				}
			}
		}
		return NULL;
	}
	CBUILDDEF void cbuild_map_init(cbuild_map_t* map, size_t nbuckets) {
		map->nbuckets = nbuckets;
		map->buckets = cbuild_malloc(map->nbuckets * sizeof(cbuild_map_bucket_t));
		cbuild_assert(map->buckets != NULL, "(LIB_CBUILD_MAP) Allocation failed.\n");
		memset(map->buckets, 0, nbuckets * sizeof(cbuild_map_bucket_t));
	}
	CBUILDDEF void* cbuild_map_get_raw(const cbuild_map_t* map, const void* key) {
		if(map->nbuckets == 0) {
			cbuild_log(CBUILD_LOG_ERROR,
				"Trying to call 'cbuild_map_get' on an empty map!");
			return CBUILD_PTR_ERR;
		}
		size_t hash = __cbuild_int_map_get_hash(map, key);
		cbuild_map_bucket_t* bucket = &map->buckets[hash];
		return __cbuild_int_map_check_bucket(map, bucket, key);
	}
	CBUILDDEF void* cbuild_map_get_or_alloc_raw(cbuild_map_t* map, const void* key) {
		if(map->nbuckets == 0) {
			cbuild_log(CBUILD_LOG_ERROR,
				"Trying to call 'cbuild_map_get_or_alloc' with an empty map!");
			return NULL;
		}
		size_t hash = __cbuild_int_map_get_hash(map, key);
		cbuild_map_bucket_t* bucket = &map->buckets[hash];
		void* ret = __cbuild_int_map_check_bucket(map, bucket, key);
		if(ret != NULL) return ret;
		bucket->nvals++;
		bucket->vals = cbuild_realloc((char*)bucket->vals,
			bucket->nvals * map->elem_size);
		cbuild_assert(bucket->vals != NULL, "(LIB_CBUILD_MAP) Allocation failed.\n");
		memset((char*)bucket->vals + (bucket->nvals - 1) * map->elem_size, 0,
			map->elem_size);
		return (char*)bucket->vals + ((bucket->nvals - 1) * map->elem_size);
	}
	CBUILDDEF bool cbuild_map_remove_ex_raw(cbuild_map_t* map, const void* key,
		cbuild_map_elem_clear_t elem_clear_func) {
		if(map->nbuckets == 0) {
			cbuild_log(CBUILD_LOG_ERROR,
				"Trying to call 'cbuild_map_remove' with an empty map!");
			return NULL;
		}
		size_t hash = __cbuild_int_map_get_hash(map, key);
		cbuild_map_bucket_t* bucket = &map->buckets[hash];
		void* elem = __cbuild_int_map_check_bucket(map, bucket, key);
		if(elem == NULL) return false;
		if(elem_clear_func) elem_clear_func(map, elem);
		if(bucket->nvals == 1) {
			cbuild_free(bucket->vals);
			bucket->vals = NULL;
			bucket->nvals = 0;
		} else {
			char* last = ((char*)bucket->vals) +
				((bucket->nvals - 1) * map->elem_size);
			memcpy(elem, last, map->elem_size);
			bucket->nvals--;
		}
		return true;
	}
	CBUILDDEF void cbuild_map_clear_ex(cbuild_map_t* map,
		cbuild_map_elem_clear_t elem_clear_func) {
		if(elem_clear_func == NULL) {
			for(size_t i = 0; i < map->nbuckets; i++) {
				cbuild_map_bucket_t* bucket = &map->buckets[i];
				cbuild_free(bucket->vals);
				bucket->vals = NULL;
				bucket->nvals = 0;
			}
			cbuild_free(map->buckets);
			map->buckets = NULL;
			map->nbuckets = 0;
		} else {
			for(size_t i = 0; i < map->nbuckets; i++) {
				cbuild_map_bucket_t* bucket = &map->buckets[i];
				for(size_t j = 0; j < bucket->nvals; j++) {
					elem_clear_func(map, bucket->vals + (j * map->elem_size));
				}
				cbuild_free(bucket->vals);
				bucket->vals = NULL;
				bucket->nvals = 0;
			}
			cbuild_free(map->buckets);
			map->buckets = NULL;
			map->nbuckets = 0;
		}
	}
	CBUILDDEF void cbuild_map_iter_reset(cbuild_map_t* map) {
		map->iter_buckets = 0;
		map->iter_vals = 0;
	}
	CBUILDDEF void* cbuild_map_iter_next(cbuild_map_t* map) {
		while(map->iter_buckets < map->nbuckets) {
			cbuild_map_bucket_t* bucket = &map->buckets[map->iter_buckets];
			while(map->iter_vals < bucket->nvals) {
				return bucket->vals + (map->iter_vals++ * map->elem_size);
			}
			map->iter_buckets++;
			map->iter_vals = 0;
		}
		return NULL;
	}
	/* FlagParse.h impl */
	struct __cbuild_int_flag_spec_t {
		// Spec
		/* Bit-mask
		 * 0-1 -> Type
		 *    - 0b00 - lopt
		 *    - 0b01 - lopt+sopt
		 *    - 0b10 - reserved
		 *    - 0b11 - reserved
		 * 2-4 -> Argument type
		 *    - 0b000 - No arg
		 *    - 0b001 - Argument
		 *    - 0b010 - List
		 *    - 0b011 - Terminator list
		 *    - Other are reserved
		 * 5 -> Argument optionality marker
		 *    - 0b0 - Required
		 *    - 0b1 - Optional
		 * 8-15 - Reserved, always 0
		 * 16-23 - Param 1 (list length)
		 * 24-31 - Param 2 (tlist terminator)
		 */
		uint32_t          type; // TODO: real bitfield
		char              sopt;
		bool              found; // State
		char              _padding[2];
		size_t            aliases_len;
		cbuild_sv_t*      aliases;
		cbuild_sv_t       opt;
		cbuild_sv_t       description;
		cbuild_sv_t       type_hint;
		cbuild_sv_t       group_name; // If size is 0 then ungrouped
		cbuild_arglist_t  args; // State
	};
	#define __CBUILD_INT_FLAG_SET_TYPE(where, val) (where) |= (((val) & 3) << 0)
	#define __CBUILD_INT_FLAG_SET_ARGT(where, val) (where) |= (((val) & 7) << 2)
	#define __CBUILD_INT_FLAG_SET_ARGO(where, val) (where) |= (((val) & 1) << 5)
	#define __CBUILD_INT_FLAG_SET_PRM1(where, val) (where) |= (((val) & 255) << 16)
	#define __CBUILD_INT_FLAG_SET_PRM2(where, val) (where) |= (((val) & 255) << 24)
	#define __CBUILD_INT_FLAG_GET_TYPE(from)       (((from) >> 0) & 0b11)
	#define __CBUILD_INT_FLAG_GET_ARGT(from)       (((from) >> 2) & 0b111)
	#define __CBUILD_INT_FLAG_GET_ARGO(from)       (((from) >> 5) & 0b1)
	#define __CBUILD_INT_FLAG_GET_PRM1(from)       (((from) >> 16) & 0xFF)
	#define __CBUILD_INT_FLAG_GET_PRM2(from)       (((from) >> 24) & 0xFF)
	struct __cbuild_int_da_flag_spec_t {
		struct __cbuild_int_flag_spec_t* data;
		size_t size;
		size_t capacity;
	};
	struct __cbuild_int_flag_context_t {
		/* Bit-mask
		 * 0 -> Separator
		 *    - 0b0 - Don;t push
		 *    - 0b1 - Push
		 * 0-31 -> Reserved, always 0
		 */
		size_t                              metadata;
		const char*                         app_name;
		cbuild_sv_t*                        group_desc;
		size_t                              group_desc_len;
		cbuild_arglist_t                    pargs;
		struct __cbuild_int_da_flag_spec_t  flags;
	};
	#define __CBUILD_INT_FLGCTX_SET_SEPARATOR(val)                                 \
		(__cbuild_int_flag_context.metadata) |= (((val) & 1) << 0)
	#define __CBUILD_INT_FLGCTX_GET_SEPARATOR()                                    \
		(((__cbuild_int_flag_context.metadata) >> 0) & 0b1)
	static struct __cbuild_int_flag_context_t __cbuild_int_flag_context = {0};
	CBUILDDEF struct __cbuild_int_flag_spec_t*
		__cbuild_int_flag_get_lopt(cbuild_sv_t opt) {
		cbuild_da_foreach(__cbuild_int_flag_context.flags, spec) {
			if((__CBUILD_INT_FLAG_GET_TYPE(spec->type) == 0b00 ||
					__CBUILD_INT_FLAG_GET_TYPE(spec->type) == 0b01) &&
				cbuild_sv_cmp(spec->opt, opt) == 0) {
				return spec;
			}
		}
		return NULL;
	}
	CBUILDDEF struct __cbuild_int_flag_spec_t*
		__cbuild_int_flag_get_lopt_aliased(cbuild_sv_t opt) {
		cbuild_da_foreach(__cbuild_int_flag_context.flags, spec) {
			if((__CBUILD_INT_FLAG_GET_TYPE(spec->type) == 0b00 ||
					__CBUILD_INT_FLAG_GET_TYPE(spec->type) == 0b01) &&
				cbuild_sv_cmp(spec->opt, opt) == 0) {
				return spec;
			}
			for(size_t i = 0; i < spec->aliases_len; i++) {
				if(cbuild_sv_cmp(spec->aliases[i], opt) == 0) {
					return spec;
				}
			}
		}
		return NULL;
	}
	CBUILDDEF struct __cbuild_int_flag_spec_t* __cbuild_int_flag_get_sopt(char opt) {
		cbuild_da_foreach(__cbuild_int_flag_context.flags, spec) {
			if(__CBUILD_INT_FLAG_GET_TYPE(spec->type) == 0b01 && spec->sopt == opt) {
				return spec;
			}
		}
		return NULL;
	}
	CBUILDDEF bool __cbuild_int_flag_first_delim_func(const cbuild_sv_t* sv,
		size_t idx, void* args) {
		if(sv->data[idx] == '\t' || sv->data[idx] == '\n' || sv->data[idx] == '\r') {
			*((char*)args) = sv->data[idx];
			return true;
		} else {
			return false;
		}
	}
	CBUILDDEF bool __cbuild_int_flag_metadata_delim_func(const cbuild_sv_t* sv,
		size_t idx, void* args) {
		if(sv->data[idx] == '\t' || sv->data[idx] == ';') {
			*((char*)args) = sv->data[idx];
			return true;
		} else {
			return false;
		}
	}
	CBUILDDEF void __cbuild_int_flag_parse_metadata_entry(
		struct __cbuild_int_flag_spec_t* new_spec, size_t parse_offset,
		cbuild_sv_t opt) {
		static const cbuild_sv_t key_arg    = cbuild_sv_from_lit("arg");
		static const cbuild_sv_t key_len    = cbuild_sv_from_lit("len");
		static const cbuild_sv_t key_thint  = cbuild_sv_from_lit("thint");
		static const cbuild_sv_t key_tdelim = cbuild_sv_from_lit("tdelim");
		static const cbuild_sv_t key_group  = cbuild_sv_from_lit("group");
		static const cbuild_sv_t arg_arg    = cbuild_sv_from_lit("arg");
		static const cbuild_sv_t arg_list   = cbuild_sv_from_lit("list");
		static const cbuild_sv_t arg_tlist  = cbuild_sv_from_lit("tlist");
		cbuild_sv_t              key        = cbuild_sv_chop_by_delim(&opt, '=');
		if(cbuild_sv_cmp(key, key_arg) == 0) {
			if(cbuild_sv_prefix(opt, arg_arg)) {
				__CBUILD_INT_FLAG_SET_ARGT(new_spec->type, 0b001);
			} else if(cbuild_sv_prefix(opt, arg_list)) {
				__CBUILD_INT_FLAG_SET_ARGT(new_spec->type, 0b010);
			} else if(cbuild_sv_prefix(opt, arg_tlist)) {
				__CBUILD_INT_FLAG_SET_ARGT(new_spec->type, 0b011);
			}
			if(opt.data[opt.size - 1] == '?') {
				__CBUILD_INT_FLAG_SET_ARGO(new_spec->type, 0b1);
			}
		} else if(cbuild_sv_cmp(key, key_len) == 0) {
			__CBUILD_INT_FLAG_SET_PRM1(new_spec->type, (unsigned int)atoi(opt.data));
		} else if(cbuild_sv_cmp(key, key_thint) == 0) {
			new_spec->type_hint = opt;
		} else if(cbuild_sv_cmp(key, key_tdelim) == 0) {
			__CBUILD_INT_FLAG_SET_PRM2(new_spec->type, (unsigned int)opt.data[0]);
		} else if(cbuild_sv_cmp(key, key_group) == 0) {
			new_spec->group_name = opt;
		} else {
			cbuild_log(CBUILD_LOG_ERROR,
				"Syntax error [%zu]: Invalid metadata entry \"" CBuildSVFmt
				"\"!",
				parse_offset, CBuildSVArg(key));
			exit(1);
		}
	}
	CBUILDDEF void __cbuild_int_flag_parse_metadata_spec(
		struct __cbuild_int_flag_spec_t* new_spec, cbuild_sv_t* spec,
		size_t* parse_offset) {
		char delim = '\t';
		cbuild_sv_t opt = cbuild_sv_chop_by_func(
			spec, __cbuild_int_flag_metadata_delim_func, &delim);
		while(delim != '\t') {
			if(opt.size > 0) {
				__cbuild_int_flag_parse_metadata_entry(new_spec, * parse_offset, opt);
			}
			// Parse next block
			opt = cbuild_sv_chop_by_func(spec, __cbuild_int_flag_metadata_delim_func,
				&delim);
			(*parse_offset) += (opt.size + 1);
		}
		if(opt.size > 0) {
			__cbuild_int_flag_parse_metadata_entry(new_spec, * parse_offset, opt);
		}
	}
	CBUILDDEF void __cbuild_int_flag_parse_cmd(cbuild_sv_t spec) {
		static const cbuild_sv_t cmd_separator = cbuild_sv_from_lit("separator");
		static const cbuild_sv_t cmd_alias     = cbuild_sv_from_lit("alias");
		static const cbuild_sv_t cmd_group     = cbuild_sv_from_lit("group");
		if(cbuild_sv_prefix(spec, cmd_separator)) {
			__CBUILD_INT_FLGCTX_SET_SEPARATOR(1);
		} else if(cbuild_sv_prefix(spec, cmd_alias)) {
			cbuild_sv_chop_by_delim(&spec, ':');
			cbuild_sv_t flag = cbuild_sv_chop_by_delim(&spec, ':');
			struct __cbuild_int_flag_spec_t* flg = __cbuild_int_flag_get_lopt(flag);
			if(flg == NULL) {
				cbuild_log(CBUILD_LOG_ERROR,
					"Syntax error: \"alias\" command require valid flag name "
					"(long option should be used here)!");
				exit(1);
			}
			cbuild_sv_t alias;
			do {
				alias = cbuild_sv_chop_by_delim(&spec, ',');
				if(alias.size == 0) {
					cbuild_log(CBUILD_LOG_ERROR,
						"Syntax error: empty alias can not be defined!");
					exit(1);
				}
				flg->aliases_len++;
				flg->aliases = cbuild_realloc(flg->aliases,
					sizeof(cbuild_sv_t) * flg->aliases_len);
				flg->aliases[flg->aliases_len - 1] = alias;
			} while(spec.size > 0);
		} else if(cbuild_sv_prefix(spec, cmd_group)) {
			cbuild_sv_chop_by_delim(&spec, ':');
			cbuild_sv_t group = cbuild_sv_chop_by_delim(&spec, ':');
			CBUILD_UNUSED(group);
			__cbuild_int_flag_context.group_desc_len += 2;
			__cbuild_int_flag_context.group_desc = cbuild_realloc(
				__cbuild_int_flag_context.group_desc,
				sizeof(cbuild_sv_t) * 2 * __cbuild_int_flag_context.group_desc_len);
			__cbuild_int_flag_context.group_desc[
			__cbuild_int_flag_context.group_desc_len - 2] = group;
			__cbuild_int_flag_context.group_desc[
			__cbuild_int_flag_context.group_desc_len - 1] = spec;
		}
	}
	CBUILDDEF void cbuild_flag_new(const char* spec_cstr) {
		cbuild_sv_t                     spec     = cbuild_sv_from_cstr(spec_cstr);
		struct __cbuild_int_flag_spec_t new_spec = { 0 };
		if(spec.data[0] == '-') {
			spec.data++;
			spec.size--;
			__cbuild_int_flag_parse_cmd(spec);
			return;
		}
		// Parse long options / positional arg ID
		char type_delim = '\0';
		new_spec.opt = cbuild_sv_chop_by_func(
			&spec, __cbuild_int_flag_first_delim_func, &type_delim);
		size_t parse_offset = new_spec.opt.size;
		switch(type_delim) {
		case '\t':
			if(spec.size < 2) {
				cbuild_log(
					CBUILD_LOG_ERROR,
					"Syntax error [%zu]: Expected short option name, but found nothing!",
					parse_offset);
				exit(1);
			}
			__CBUILD_INT_FLAG_SET_TYPE(new_spec.type, 0b01);
			new_spec.sopt = spec.data[0];
			if(spec.data[1] != '\t') {
				cbuild_log(CBUILD_LOG_ERROR,
					"Syntax error [%zu]: Expected short option name but found "
					"more than 1 character!.",
					parse_offset + 2);
				exit(1);
			}
			spec.data += 2;
			spec.size -= 2;
			parse_offset += 3; // Offset is one lower than real parse position
			__attribute__((fallthrough));
		case '\n':
			__CBUILD_INT_FLAG_SET_TYPE(new_spec.type, 0b00);
			__cbuild_int_flag_parse_metadata_spec(&new_spec, &spec, &parse_offset);
			break;
		default:
			cbuild_log(
				CBUILD_LOG_ERROR,
				"Syntax error [%zu]: Invalid type specifier, expected '\\t', '\\n' "
				", but got '%02x'!",
				parse_offset, type_delim);
			exit(1);
			break;
		}
		new_spec.description = spec;
		new_spec.found       = false;
		new_spec.args        = (cbuild_arglist_t) {
			0
		};
		cbuild_da_append(&(__cbuild_int_flag_context.flags), new_spec);
	}
	CBUILDDEF void __cbuild_int_parse_flag_args(
		struct __cbuild_int_flag_spec_t* spec, int argc, char** argv,
		int* parse_ptr) {
		// no args
		if(__CBUILD_INT_FLAG_GET_ARGT(spec->type) == 0b000) {
			(*parse_ptr)--;
			return;
		}
		// No args but some required
		if(argc == *parse_ptr) {
			if(__CBUILD_INT_FLAG_GET_ARGO(spec->type) == 0) {
				char* type = "";
				if(__CBUILD_INT_FLAG_GET_ARGT(spec->type) == 0b001) {
					type = "one argument";
				} else if(__CBUILD_INT_FLAG_GET_ARGT(spec->type) == 0b010) {
					type = "list of arguments";
				} else if(__CBUILD_INT_FLAG_GET_ARGT(spec->type) == 0b011) {
					type = "list of arguments";
				}
				cbuild_log(CBUILD_LOG_ERROR,
					"(CBUILD_FLAG_PARSE) Flag \"" CBuildSVFmt
					"\" requires %s but none provided!",
					CBuildSVArg(spec->opt), type);
				exit(1);
			} else {
				return;
			}
		}
		// One argument
		if(__CBUILD_INT_FLAG_GET_ARGT(spec->type) == 0b001) {
			char* arg = argv[*parse_ptr];
			if(arg[0] == '-') {
				if(__CBUILD_INT_FLAG_GET_ARGO(spec->type) == 0) {
					cbuild_log(CBUILD_LOG_ERROR,
						"(CBUILD_FLAG_PARSE) Flag \"" CBuildSVFmt
						"\" requires one argument but none provided!",
						CBuildSVArg(spec->opt));
					exit(1);
				} else {
					(*parse_ptr)--;
					return;
				}
			} else {
				cbuild_da_clear(&spec->args);
				cbuild_da_append(&spec->args, arg);
				return;
			}
		}
		// List or TList arguments
		while(*parse_ptr < argc) {
			char* arg = argv[*parse_ptr];
			// Terminate list on argument
			if(__CBUILD_INT_FLAG_GET_ARGT(spec->type) == 0b010 && arg[0] == '-') {
				(*parse_ptr)--;
				break;
			}
			// Terminate tlist on terminator
			if(__CBUILD_INT_FLAG_GET_ARGT(spec->type) == 0b011 && strlen(arg) == 1 &&
				(char)__CBUILD_INT_FLAG_GET_PRM2(spec->type) == arg[0]) {
				break;
			}
			// Append argument
			cbuild_da_append(&spec->args, arg);
			(*parse_ptr)++;
		}
		// Check args count
		if(__CBUILD_INT_FLAG_GET_PRM1(spec->type) != 0 &&
			__CBUILD_INT_FLAG_GET_PRM1(spec->type) != spec->args.size) {
			cbuild_log(CBUILD_LOG_ERROR,
				"(CBUILD_FLAG_PARSE) Flag \"" CBuildSVFmt
				"\" requires list of %d arguments, but %zu provided!",
				CBuildSVArg(spec->opt), __CBUILD_INT_FLAG_GET_PRM1(spec->type),
				spec->args.size);
			exit(1);
		}
	}
	CBUILDDEF void cbuild_flag_parse(int argc, char** argv) {
		static const cbuild_sv_t arg_help    = cbuild_sv_from_cstr("help");
		static const cbuild_sv_t arg_version = cbuild_sv_from_cstr("version");
		static const cbuild_sv_t arg_lprefix = cbuild_sv_from_cstr("--");
		static const cbuild_sv_t arg_sprefix = cbuild_sv_from_cstr("-");
		__cbuild_int_flag_context.app_name   = argv[0];
		bool parse_no_flags                  = false;
		for(int i = 1; i < argc; i++) {
			cbuild_sv_t arg = cbuild_sv_from_cstr(argv[i]);
			if(!parse_no_flags && cbuild_sv_prefix(arg, arg_lprefix)) {
				arg.size -= 2;
				arg.data += 2;
				// --
				if(arg.size == 0) {
					parse_no_flags = true;
					if(__CBUILD_INT_FLGCTX_GET_SEPARATOR() == 1) {
						cbuild_da_append(&__cbuild_int_flag_context.pargs, "--");
					}
					continue;
				}
				if(cbuild_sv_cmp(arg, arg_help) == 0) {
					cbuild_flag_help(__cbuild_int_flag_context.app_name);
					exit(0);
				}
				if(cbuild_sv_cmp(arg, arg_version) == 0) {
					cbuild_flag_version(__cbuild_int_flag_context.app_name);
					exit(0);
				}
				struct __cbuild_int_flag_spec_t* spec =
					__cbuild_int_flag_get_lopt_aliased(arg);
				if(spec == NULL) {
					cbuild_log(CBUILD_LOG_ERROR,
						"(CBUILD_FLAG_PARSE) Invalid long flag \"" CBuildSVFmt "\"!",
						CBuildSVArg(arg));
					cbuild_flag_help(__cbuild_int_flag_context.app_name);
					exit(1);
				}
				spec->found = true;
				i++;
				__cbuild_int_parse_flag_args(spec, argc, argv, &i);
			} else if(!parse_no_flags && cbuild_sv_prefix(arg, arg_sprefix)) {
				arg.size--;
				arg.data++;
				size_t sopts_len = strlen(arg.data);
				if(sopts_len == 0) { // raw '-' option, stdin
					cbuild_da_append(&__cbuild_int_flag_context.pargs, argv[i]);
				}
				for(size_t j = 0; j < sopts_len; j++) {
					char opt = arg.data[j];
					if(opt == 'h') {
						cbuild_flag_help(__cbuild_int_flag_context.app_name);
						exit(0);
					}
					if(opt == 'V') {
						cbuild_flag_version(__cbuild_int_flag_context.app_name);
						exit(0);
					}
					struct __cbuild_int_flag_spec_t* spec = __cbuild_int_flag_get_sopt(opt);
					if(spec == NULL) {
						cbuild_log(CBUILD_LOG_ERROR,
							"(CBUILD_FLAG_PARSE) Invalid short flag \"%c\"!", opt);
						cbuild_flag_help(__cbuild_int_flag_context.app_name);
						exit(1);
					}
					spec->found = true;
					i++;
					if(j == strlen(arg.data) - 1) {
						__cbuild_int_parse_flag_args(spec, argc, argv, &i);
					} else {
						if(__CBUILD_INT_FLAG_GET_ARGT(spec->type) != 0b000 &&
							__CBUILD_INT_FLAG_GET_ARGO(spec->type) == 0b0) {
							char* type = "";
							if(__CBUILD_INT_FLAG_GET_ARGT(spec->type) == 0b001) {
								type = "one argument";
							} else if(__CBUILD_INT_FLAG_GET_ARGT(spec->type) == 0b010) {
								type = "list of arguments";
							} else if(__CBUILD_INT_FLAG_GET_ARGT(spec->type) == 0b011) {
								type = "list of arguments";
							}
							cbuild_log(CBUILD_LOG_ERROR,
								"(CBUILD_FLAG_PARSE) Flag \"%c\" requires %s but none "
								"provided!",
								spec->sopt, type);
							exit(1);
						} else {
							i--;
						}
					}
				}
			} else {
				cbuild_da_append(&__cbuild_int_flag_context.pargs, argv[i]);
			}
		}
	}
	CBUILDDEF char* __cbuild_int_flag_help_fmt(
		struct __cbuild_int_flag_spec_t* spec) {
		cbuild_sb_t sb = {0};
		// Short opt
		if(__CBUILD_INT_FLAG_GET_TYPE(spec->type) == 0b01) {
			cbuild_sb_append_cstr(&sb, "\t-");
			cbuild_sb_append(&sb, spec->sopt);
			cbuild_sb_append_cstr(&sb, ", ");
		} else {
			cbuild_sb_append_cstr(&sb, "\t");
		}
		// Long opt
		cbuild_sb_append_cstr(&sb, "--");
		cbuild_sb_append_sv(&sb, spec->opt);
		// For normal args
		switch(__CBUILD_INT_FLAG_GET_ARGT(spec->type)) {
		case 0b000: break;
		case 0b001:
			if(spec->type_hint.size > 0) {
				cbuild_sb_append_cstr(&sb, " <");
				cbuild_sb_append_sv(&sb, spec->type_hint);
				if(__CBUILD_INT_FLAG_GET_ARGO(spec->type) == 0b1) {
					cbuild_sb_append(&sb, '?');
				}
				cbuild_sb_append(&sb, '>');
			}
			break;
		case 0b010:
			if(spec->type_hint.size > 0 ||
				__CBUILD_INT_FLAG_GET_ARGO(spec->type) == 0b1 ||
				__CBUILD_INT_FLAG_GET_PRM1(spec->type) != 0) {
				cbuild_sb_append_cstr(&sb, " <");
			}
			if(spec->type_hint.size > 0) {
				cbuild_sb_append_sv(&sb, spec->type_hint);
			}
			if(__CBUILD_INT_FLAG_GET_ARGO(spec->type) == 0b1) {
				cbuild_sb_append(&sb, '?');
			}
			if(__CBUILD_INT_FLAG_GET_PRM1(spec->type) != 0) {
				cbuild_sb_append(&sb, ':');
				cbuild_sb_appendf(&sb, "%d", (int)__CBUILD_INT_FLAG_GET_PRM1(spec->type));
			}
			if(spec->type_hint.size > 0 ||
				__CBUILD_INT_FLAG_GET_ARGO(spec->type) == 0b1 ||
				__CBUILD_INT_FLAG_GET_PRM1(spec->type) != 0) {
				cbuild_sb_append(&sb, '>');
			}
			break;
		case 0b011:
			cbuild_sb_append_cstr(&sb, " <");
			if(spec->type_hint.size > 0) {
				cbuild_sb_append_sv(&sb, spec->type_hint);
			}
			if(__CBUILD_INT_FLAG_GET_ARGO(spec->type) == 0b1) {
				cbuild_sb_append(&sb, '?');
			}
			if(__CBUILD_INT_FLAG_GET_PRM1(spec->type) != 0) {
				cbuild_sb_append(&sb, ':');
				cbuild_sb_appendf(&sb, "%d", (int)__CBUILD_INT_FLAG_GET_PRM1(spec->type));
			}
			cbuild_sb_append(&sb, '|');
			cbuild_sb_append(&sb, (char)__CBUILD_INT_FLAG_GET_PRM2(spec->type));
			cbuild_sb_append(&sb, '>');
			break;
		default: CBUILD_UNREACHABLE("Flag argument type help printer."); break;
		}
		// Null terminator
		cbuild_sb_append_null(&sb);
		return sb.data;
	}
	CBUILDDEF size_t __cbuild_int_flag_get_flgh_len(
		struct __cbuild_int_flag_spec_t* spec) {
		char* str = __cbuild_int_flag_help_fmt(spec);
		size_t ret = strlen(str);
		cbuild_free(str);
		return ret;
	}
	CBUILDDEF void cbuild_flag_print_help(void) {
		// Get length of longest option
		size_t opt_len = strlen("\t-v, --version"); // minimal length
		cbuild_da_foreach(__cbuild_int_flag_context.flags, spec) {
			size_t new_opt_len = __cbuild_int_flag_get_flgh_len(spec);
			opt_len            = new_opt_len > opt_len ? new_opt_len : opt_len;
		}
		// Help for flags
		__CBUILD_PRINT("Flags:\n");
		// Help
		int written = __CBUILD_PRINT("\t-h, --help");
		__CBUILD_PRINTF("%-*s", (int)(((int)opt_len + 2) - written), "");
		__CBUILD_PRINT("Shows app help (this message).\n");
		// Version
		written = __CBUILD_PRINT("\t-v, --version");
		__CBUILD_PRINTF("%-*s", (int)(((int)opt_len + 2) - written), "");
		__CBUILD_PRINT("Shows app version information.\n");
		// Extract groups and print ungrouped args
		cbuild_sv_t* groups = NULL;
		size_t groups_len = 0;
		cbuild_da_foreach(__cbuild_int_flag_context.flags, spec) {
			if(spec->group_name.size == 0) {
				char* opt   = __cbuild_int_flag_help_fmt(spec);
				written = __CBUILD_PRINTF("%s", opt);
				cbuild_free(opt);
				__CBUILD_PRINTF("%-*s", (int)(((int)opt_len + 2) - written), "");
				__CBUILD_PRINTF(CBuildSVFmt, CBuildSVArg(spec->description));
				__CBUILD_PRINT("\n");
			} else {
				bool found = false;
				for(size_t i = 0; i < groups_len; i++) {
					if(cbuild_sv_cmp(spec->group_name, groups[i]) == 0) {
						found = true;
						break;
					}
				}
				if(!found) {
					groups_len++;
					groups = cbuild_realloc(groups, sizeof(cbuild_sv_t) * groups_len);
					groups[groups_len - 1] = spec->group_name;
				}
			}
		}
		cbuild_da_foreach(__cbuild_int_flag_context.flags, spec) {
			if(spec->group_name.size == 0) {
				if(spec->aliases_len == 0) continue;
				__CBUILD_PRINT("\t");
				for(size_t i = 0; i < spec->aliases_len; i++) {
					__CBUILD_PRINTF("--"CBuildSVFmt, CBuildSVArg(spec->aliases[i]));
					if(i + 1 < spec->aliases_len) {
						__CBUILD_PRINT(", ");
					}
				}
				__CBUILD_PRINTF(" -> --"CBuildSVFmt"\n", CBuildSVArg(spec->opt));
			}
		}
		// Print grouped args
		for(size_t i = 0; i < groups_len; i++) {
			__CBUILD_PRINTF(CBuildSVFmt":\n", CBuildSVArg(groups[i]));
			for(size_t j = 0; j < __cbuild_int_flag_context.group_desc_len; j += 2) {
				if(cbuild_sv_cmp(__cbuild_int_flag_context.group_desc[j],
						groups[i]) == 0) {
					__CBUILD_PRINTF(CBuildSVFmt"\n",
						CBuildSVArg(__cbuild_int_flag_context.group_desc[j + 1]));
					break;
				}
			}
			cbuild_da_foreach(__cbuild_int_flag_context.flags, spec) {
				if(cbuild_sv_cmp(spec->group_name, groups[i]) == 0) {
					char* opt   = __cbuild_int_flag_help_fmt(spec);
					written = __CBUILD_PRINTF("%s", opt);
					cbuild_free(opt);
					__CBUILD_PRINTF("%-*s", (int)(((int)opt_len + 2) - written), "");
					__CBUILD_PRINTF(CBuildSVFmt, CBuildSVArg(spec->description));
					__CBUILD_PRINT("\n");
				}
			}
			cbuild_da_foreach(__cbuild_int_flag_context.flags, spec) {
				if(cbuild_sv_cmp(spec->group_name, groups[i]) == 0) {
					if(spec->aliases_len == 0) continue;
					__CBUILD_PRINT("\t");
					for(size_t j = 0; j < spec->aliases_len; j++) {
						__CBUILD_PRINTF("--"CBuildSVFmt, CBuildSVArg(spec->aliases[i]));
						if(j + 1 < spec->aliases_len) {
							__CBUILD_PRINT(", ");
						}
					}
					__CBUILD_PRINTF(" -> --"CBuildSVFmt"\n", CBuildSVArg(spec->opt));
				}
			}
		}
	}
	CBUILDDEF cbuild_arglist_t* cbuild_flag_get_pargs(void) {
		return &__cbuild_int_flag_context.pargs;
	}
	CBUILDDEF cbuild_arglist_t* cbuild_flag_get_flag(const char* opt) {
		struct __cbuild_int_flag_spec_t* spec =
			__cbuild_int_flag_get_lopt(cbuild_sv_from_cstr(opt));
		if(spec == NULL) {
			return NULL;
		}
		if(spec->found == false) {
			return NULL;
		}
		return &spec->args;
	}
	CBUILDDEF const char* cbuild_flag_app_name(void) {
		return __cbuild_int_flag_context.app_name;
	}
	CBUILDDEF void __cbuild_int_flag_help(const char* name) {
		__CBUILD_PRINTF("Usage: %s [OPTIONS]\n\n", name);
		cbuild_flag_print_help();
	}
	void (*cbuild_flag_help)(const char* app_name) = __cbuild_int_flag_help;
	CBUILDDEF void __cbuild_int_flag_version(const char* name) {
		__CBUILD_PRINTF("%s - v1.0\n", name);
	}
	void (*cbuild_flag_version)(const char* app_name) = __cbuild_int_flag_version;
#endif // CBUILD_IMPLEMENTATION
