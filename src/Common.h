#pragma once // For LSP 
//! This file provides OS-selection logic, includes and some general utilities.
//!
//! License: `GPL-3.0-or-later`.
//! 
//! ::: note
//! CBuild should be first header included in translation unit. On `CBUILD_API_POSIX`
//! (default for modern Unixes) it defines `_GNU_SOURCE`.
//! :::

//! # Compiler defines [line:cbuild-cc-defines]
//!
//! Valid compilers:
//! - `CBUILD_CC_CLANG` - clang or its derivative.
//! - `CBUILD_CC_GCC` - gcc or its derivative.
//!
//! ::: note
//! This selection can be overriden by manually defining valid compiler and `CBUILD_CC_DEFINED`.
//! :::
//!
//! Several compiler extensions are required from supported compiler:
//! - Binary literals support (`0b`) either trough c standard (c23+) or as extension.
//! - Attributes trough `__attribute__((...))` syntax, some are supported trough `[[...]]` syntax if support is detected 
//!   * deprecated.
//!   * format.
//!   * noreturn.
//!   * falltrough.
//! - Unnamed structs/enums/unions.
//! - typeof.
//! - `__VA_OPT__`.
//! - GNU statement expressions `({...})`.
//! - `__auto_type`.
//!
//! Explicetely unsupported compilers:
//! - TCC - no statement expression (at least).
//! - MSVC - no extensions at all.
//!
//! # Debug
//!
//! By default CBuild provides _debug_ logging from most of its modules. All logging goes
//! trough `cbuild_log`, so if your app does no need any messages in terminal you can
//! disable CBuild's logger (which will hide all messages).

//@ cbuild-cc-defines
#if !defined(CBUILD_CC_DEFINED)
	#if defined(__clang__)
		#define CBUILD_CC_CLANG
	#elif defined(__TINYC__)
		#error "You are trying to compile CBuild with TinyC compiler. This compile is not supported. It should support 'gnu99' extensions, including binary literals; attributes; unnamed (anonymous) structs, enums, unions; typeof; __VA_OPT__; statement expressions."
	#elif defined(__GNUC__)
		#define CBUILD_CC_GCC
	#elif defined(_MSC_VER)
		#error "You are trying to compile CBuild with MSVC compiler. It does not support 'gnu99' extensions, including binary literals; attributes; unnamed (anonymous) structs, enums, unions; typeof; __VA_OPT__; statement expressions."
	#else
		#error "This compile is unsupported. If it supports 'gnu99' extensions, including binary literals; attributes; unnamed (anonymous) structs, enums, unions; typeof; __VA_OPT__; statement expressions; then you can simply add a check to this block for it."
	#endif // Compiler check
	#define CBUILD_CC_DEFINED
#endif // !CBUILD_CC_DEFINNED

//! # OS defines [line:cbuild-os-defines]
//!
//! Valid OSes:
//! - `CBUILD_OS_LINUX` - Any desktop Linux (excluding Android or ChromeOS for example).
//! - `CBUILD_OS_MACOS` - MacOS.
//! - `CBUILD_OS_BSD` - Any desktop BSD.
//! - `CBUILD_OS_UNIX` - Generic Unix, should support at least POSIX.1-2001.
//! - `CBUILD_OS_WINDOWS` - Windows.
//!
//! Also, there are some auxiliary OS checks, valid values are:
//! - `CBUILD_OS_WINDOWS`:
//!   * `CBUILD_OS_WINDOWS_CYGWIN `- Windows under Cygwin.
//! - `CBUILD_OS_LINUX`:
//!   * `CBUILD_OS_LINUX_GLIBC` - Linux with glibc.
//!   * `CBUILD_OS_LINUX_UCLIBC` - Linux with uclibc.
//!   * `CBUILD_OS_LINUX_MUSL` - Linux with musl (fallback, because musl provides no way of detection).

//@ cbuild-os-defines
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
	#elif defined(__CYGWIN__) || defined(__cygwin__)
		#define CBUILD_OS_WINDOWS
		#define CBUILD_OS_WINDOWS_CYGWIN
	#elif defined(__unix__) || defined(unix) || defined(__unix)
		#define CBUILD_OS_UNIX
	#elif defined(_WIN32) || defined(_WIN64) || defined(__MINGW32__) || defined(__MINGW64__)
		#define CBUILD_OS_WINDOWS
	#else
		#error "This OS is unsupported by CBuild. If it supports POSIX.1-2001 you can simply add check for it to a list of checks here. If not you should add a check for OS and a macro for its API and then implement OS/API specifc parts of CBuild."
	#endif // OS detect
	// TODO: Android and IOS
#endif // !CBUILD_OS_DEFINED

//! # API defines [line:cbuild-api-defines]
//!
//! Valid APIs:
//! - `CBUILD_API_POSIX`.
//!   POSIX.1-2001 (`_POSIX_C_SOURCE == 200112L`). 
//!   Any detected extension can be used.
//!   Permorms detection for extensions on MacOS, BSD, Linux.
//!   Default for:
//!   * `CBUILD_OS_LINUX`
//!   * `CBUILD_OS_MACOS`
//!   * `CBUILD_OS_BSD`
//!   * `CBUILD_OS_WINDOWS_CYGWIN`
//! - `CBUILD_API_STRICT_POSIX`
//!   POSIX.1-2001 (`_POSIX_C_SOURCE == 200112L`). 
//!   Only three extension are used and one extension type is considered: 
//!   * `MAP_ANON/MAP_ANONYMOUS`. If none is available fallabck to `shm_open` is provided but needs `dlopen` and `dlsym` provided.
//!   * `sysconf(_SC_NPROCESSORS_ONLN)`. Literal `1` is used as fallback.
//!   * `ioctl` and `TIOCGWINSZ`.
//!   * Newer POSIX versions can be used if detected.
//! - `CBUILD_API_WINAPI`
//!   WinAPI and possibly NT API.
//!   Not implemented for now.

//@ cbuild-api-defines
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
//! # Platform selector [line:cbuild-platform-selector]
//!
//! This selector include platform-specific headers and configures platforms's libc.

//@ cbuild-platform-selector
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
	#include <termios.h>
	#include <signal.h>
	#include <sys/mman.h>
	#include <sys/stat.h>
	#include <sys/time.h>
	#include <sys/types.h>
	#include <sys/wait.h>
	#include <sys/ioctl.h>
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
	/// Process handle
	typedef pid_t cbuild_proc_t;
	/// Invalid process handle
	#define CBUILD_INVALID_PROC -1
	/// File descriptor
	typedef int cbuild_fd_t;
	/// Invalid file descriptor
	#define CBUILD_INVALID_FD -1
	/// Directory descriptor
	typedef DIR* cbuild_dir_t;
	/// Invalid directory descriptor
	#define CBUILD_INVALID_DIR NULL
	/// Additional error value for pointers (when error and not found are separate cases)
	#define CBUILD_PTR_ERR (void*)((intptr_t)-1)
	/// Handle for dynamic library
	typedef void* cbuild_dlib_t;
#elif defined(CBUILD_API_STRICT_POSIX)
	#if !defined(_POSIX_C_SOURCE)
		#define _POSIX_C_SOURCE 200112L
	#endif
	// Platform includes
	#include <dirent.h>
	#include <dlfcn.h>
	#include <errno.h>
	#include <fcntl.h>
	#include <termios.h>
	#include <signal.h>
	#include <sys/mman.h>
	#include <sys/stat.h>
	#include <sys/time.h>
	#include <sys/types.h>
	#include <sys/wait.h>
	#include <sys/ioctl.h>
	#include <unistd.h>
	// Process and file handles
	typedef pid_t cbuild_proc_t;
	#define CBUILD_INVALID_PROC -1
	typedef int cbuild_fd_t;
	#define CBUILD_INVALID_FD -1
	/// Directory descriptor
	typedef DIR* cbuild_dir_t;
	/// Invalid directory descriptor
	#define CBUILD_INVALID_DIR NULL
	// For pointer errors
	#define CBUILD_PTR_ERR (void*)((intptr_t)-1)
	/// Handle for dynamic library
	typedef void* cbuild_dlib_t;
#elif defined(CBUILD_API_WINAPI)
	// Platform includes
	#include <windows.h>
	#include <io.h>
	/// Process handle
	typedef HANDLE cbuild_proc_t;
	/// Invalid process handle
	#define CBUILD_INVALID_PROC ((HANDLE)(intptr_t)-1)
	/// File descriptor
	typedef HANDLE cbuild_fd_t;
	/// Invalid file descriptor
	#define CBUILD_INVALID_FD ((HANDLE)(intptr_t)-1)
	/// Additional error value for pointers (when error and not found are separate cases)
	#define CBUILD_PTR_ERR (void*)((intptr_t)-1)
	/// Handle for dynamic library
	typedef HINSTANCE cbuild_dlib_t;
#endif // CBUILD_API_*

//! # Normal platform-independent includes [lines:cbuild-includes]

//@ cbuild-includes
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
#include <regex.h> // mingw seems to provide it too

//! # Configuration values [line:cbuild-configuration]
//!
//! All of this are defines that user can redefine.
//!
//! There are few more configuration define not listed here:
//!
//! - `CBUILD_PROFILER` - Enable profiler features in modules.
//! - `CBUILD_LOG_CUSTOM_LEVELS` - used to inject custom log levels in 
//!    CBuild's logger

//@ cbuild-configuration
#ifndef CBUILD_INIT_CAPACITY
	/// Initial capacity of cbuild dynamic arrays. Size is in elements.
	///
	/// Type: `size_t`{.c}.
	#define CBUILD_INIT_CAPACITY (size_t)256
#endif // CBUILD_INIT_CAPACITY
#ifndef CBUILD_QUICK_SPRINTF_SIZE
	/// Size of stack buffer used to speedup `sprintf` for small strings.
	///
	/// Type: `size_t`{.c}.
	#define CBUILD_QUICK_SPRINTF_SIZE (size_t)512
#endif // CBUILD_QUICK_SPRINTF_SIZE
#ifndef CBUILD_FS_TMP_SIZE
	/// Size of temporary buffer used for FS operations
	///
	/// Type: `size_t`{.c}.
	#define CBUILD_FS_TMP_SIZE ((size_t)32 * (size_t)1024 * (size_t)1024)
#endif // CBUILD_FS_TMP_SIZE
#ifndef CBUILD_TEMP_ARENA_SIZE
	/// Size of temporary allocator.
	///
	/// Type: `size_t`{.c}.
	#define CBUILD_TEMP_ARENA_SIZE ((size_t)8 * (size_t)1024 * (size_t)1024)
#endif // CBUILD_TEMP_ARENA_SIZE
#ifndef CBUILD_MAP_DEFAULT_HASH_FUNC
	/// Default hash function for CBuild map. User can override it in runtime 
	/// for specific map instance.
	///
	/// [Type](map.html#cbuild_map_hash_t): `size_t func(const void* key, size_t len)`{.c}. 
	#define CBUILD_MAP_DEFAULT_HASH_FUNC __cbuild_map_hash_func
#endif // CBUILD_MAP_DEFAULT_HASH_FUNC
#ifndef CBUILD_SELFREBUILD_ARGS
	/// Default arguments for `cbuild_selfrebuild`.
	///
	/// Type: Comma-separated list of `const char*`{.c}.
	#define CBUILD_SELFREBUILD_ARGS CBUILD_CARGS_WARN
#endif // CBUILD_SEFLREBUILD_ARGS
#ifndef CBUILD_LOG_MIN_LEVEL
	/// Minimal log level for `cbuild_log`.
	/// 
	/// [Type](log.html#cbuild_log_level_t): `cbuild_log_level_t`.
	#define CBUILD_LOG_MIN_LEVEL CBUILD_LOG_ERROR
#endif // CBUILD_LOG_MIN_LEVEL
#ifndef CBUILD_GLOB_CAPTURE_COUNT
	/// Number og "capture groups" glob supports. This is used for underlying regex engine.
	#define CBUILD_GLOB_CAPTURE_COUNT 10
#endif // CBUILD_GLOB_CAPTURE_COUNT
#ifndef CBUILDDEF
	/// This is prepended to all cbuild's functions. Can be set to eg. `static inline`
	/// for build that use only one translation unit.
	///
	/// Type: function attributes.
	#define CBUILDDEF
#endif // CBUILDDEF

//! # Preprocessor configuration [line:cbuild-misc-preproc] {#preprocessor-configuration}
//!
//! This section provides several miscellaneous preprocessor macro.
//! - Print hooks. These can not be overridden by user and should be edited here.
//! - Macro helpers for '##' and '#' operators
//! - Attribute abstractions to support both new C23 attributes and older 
//!   gnu-style attributes.
//! - Allocator hooks (`__CBUILD_MALLOC`, `__CBUILD_REALLOC`, `__CBUILD_FREE`) which
//!   can be overridden by defining CBUILD_CUSTOM_ALLOC

//@ cbuild-misc-preproc
// These are used for logging only (both from assert and from logger).
#define __CBUILD_PRINT(str)                printf("%s", (str))
#define __CBUILD_PRINTF(fmt, ...)          printf((fmt), __VA_ARGS__)
#define __CBUILD_VPRINTF(fmt, va_args)     vprintf((fmt), (va_args))
#define __CBUILD_FLUSH()                   fflush(stdout)
// Macro helpers
#define __CBUILD_STRINGIFY(var)  #var
#define __CBUILD_XSTRINGIFY(var) __CBUILD_STRINGIFY(var)
#define __CBUILD_CONCAT(a, b)    a ## b
#define __CBUILD_XCONCAT(a, b)   __CBUILD_CONCAT(a, b)
#define __CBUILD_EXPAND(a)       a
// Attributes
#if defined(__has_c_attribute)
	#if __has_c_attribute(deprecated)
		#define CBUILD_ATTR_DEPRECATED(msg, decl) [[deprecated(msg)]] decl
	#else
		#define CBUILD_ATTR_DEPRECATED(msg, decl) decl __attribute__((deprecated(msg)))
	#endif //__has_c_attribute(deprecated)
#else
	#define CBUILD_ATTR_DEPRECATED(msg, decl) decl __attribute__((deprecated(msg)))
#endif // defined(__has_c_attribute)
#if defined(__has_c_attribute)
	#if __has_c_attribute(noreturn)
		#define CBUILD_ATTR_NORETURN(decl) [[noreturn]] decl
	#else
		#define CBUILD_ATTR_NORETURN(decl) decl __attribute__((noreturn))
	#endif // __has_c_attribute(noreturn)
#else
	#define CBUILD_ATTR_NORETURN(decl) decl __attribute__((noreturn))
#endif // defined(__has_c_attribute)
#if defined(__has_c_attribute)
	#if __has_c_attribute(fallthrough)
		#define CBUILD_ATTR_FALLTHROUGH() [[fallthrough]] 
	#else
		#define CBUILD_ATTR_FALLTHROUGH() __attribute__((fallthrough))
	#endif // __has_c_attribute(fallthrough)
#else
	#define CBUILD_ATTR_FALLTHROUGH() __attribute__((fallthrough))
#endif // defined(__has_c_attribute)
#define __CBUILD_ATTR_ADD1_0 1
#define __CBUILD_ATTR_ADD1_1 2
#define __CBUILD_ATTR_ADD1_2 3
#define __CBUILD_ATTR_ADD1_3 4
#define __CBUILD_ATTR_ADD1_4 5
#define __CBUILD_ATTR_ADD1_5 6
#if defined(__has_c_attribute)
	#if __has_c_attribute(gnu::format)
		#define CBUILD_ATTR_PRINTF(arg, decl) [[gnu::format(printf, arg, __CBUILD_XCONCAT(__CBUILD_ATTR_ADD1_, arg))]] decl
	#else
		#define CBUILD_ATTR_PRINTF(arg, decl) decl __attribute__((format(printf, arg, __CBUILD_XCONCAT(__CBUILD_ATTR_ADD1_, arg))))
	#endif // __has_c_attribute(gnu::format)
#else
	#define CBUILD_ATTR_PRINTF(arg, decl) decl __attribute__((format(printf, arg, __CBUILD_XCONCAT(__CBUILD_ATTR_ADD1_, arg))))
#endif // defined(__has_c_attribute)
// Allocator hooks
#ifndef CBUILD_CUSTOM_ALLOC
	#define __CBUILD_MALLOC(size)       malloc(size)
	#define __CBUILD_REALLOC(ptr, size) realloc(ptr, size)
	#define __CBUILD_FREE(ptr)          free(ptr)
#endif // CBUILD_CUSTOM_ALLOC

/// Mark some variable as unused.
///
/// * [pl:val:name] Variable name.
#define CBUILD_UNUSED(val)       (void)(val)

/// Place `TODO` marker in code. Upon reaching it execution will abort with message. 
///
/// * [pl:message:const char*] Message that will be printed before aborting. printf-style.
/// * [pl:...:...any[]] Format string arguments.
#define CBUILD_TODO(message, ...)                              \
	do {                                                         \
		__CBUILD_PRINTF("%s:%d: TODO: %s\n", __FILE__, __LINE__,   \
			cbuild_temp_sprintf(message __VA_OPT__(,) __VA_ARGS__)); \
		abort();                                                   \
	} while (0)
/// Mark some code as unreachable. This will abort with message if executed.
///
/// * [pl:message:const char*] Message that will be printed before aborting. printf-style.
/// * [pl:...:...any[]] Format string arguments.
#define CBUILD_UNREACHABLE(message, ...)                            \
	do {                                                              \
		__CBUILD_PRINTF("%s:%d: UNREACHABLE: %s\n", __FILE__, __LINE__, \
			cbuild_temp_sprintf(message __VA_OPT__(,) __VA_ARGS__));      \
		abort();                                                        \
	} while (0)

/// Assert that can pretty-print messages.
///
/// * [pl:expr:() => bool] Any expression returns boolean.
/// * [pl:...:format string] Printf-style format string and its arguments.
#define cbuild_assert(expr, ...)                                      \
	(expr) ? (void)(0)                                                  \
	: __cbuild_assert(__FILE__, __LINE__, __func__, #expr, __VA_ARGS__)
/// Pretty-printer for `cbuild_assert`.
///
/// * [pl:file] File that should be reported.
/// * [pl:line] Line that should be reported.
/// * [pl:func] Function that should be reported.
/// * [pl:expr] Expression that should be reported.
/// * [pl:...:format string] Printf-style format string and its arguments.
CBUILD_ATTR_NORETURN(CBUILDDEF void __cbuild_assert(const char* file, unsigned int line,
		const char* func, const char* expr, ...));

/// Get length of array whose size is know at compile-time
///
/// * [pl:array:any*] Array, not just a pointer.
///
/// [r:size_t] Length of an array in elements.
#define cbuild_arr_len(array) (sizeof(array) / sizeof((array)[0]))
/// Get element from array with bound checking (array size should be know at compile-time)
/// Abort execution if index is out of bounds.
///
/// * [pl:array:any*] Array, not just a pointer.
/// * [pl:index:size_t] 
///
/// [r:any] Element from an array.
#define cbuild_arr_get(array, index)                      \
	(cbuild_assert((size_t)(index) < cbuild_arr_len(array), \
			"Index %zu is out of bounds.", (size_t)index),      \
		(array)[(size_t)(index)])

/// Shift arguments from "standard" C array view - pointer and size as separate variables.
/// Size will be decremented and pointer will be incremented.
///
/// * [pl:argv:any*] Array.
/// * [pl:argc:size_t] Length of an array.
///
/// [r:any] Shifted First element of an array.
#define cbuild_shift(argv, argc)                                        \
	(cbuild_assert((argc) > 0, "More arguments are required."), (argc)--, \
		*(argv)++)
/// Shift arguments from "standard" C array view - pointer and size as separate variables.
/// Size will be decremented and pointer will be incremented.
/// Allows for a used-defined message.
///
/// * [pl:argv:any*] Array.
/// * [pl:argc:size_t] Length of an array.
/// * [pl:...:format string] Message that will be printed if array does not have enough elements.
///
/// [r:any] Shifted First element of an array.
#define cbuild_shift_expect(argv, argc, ...)                    \
	(cbuild_assert((argc) > 0, __VA_ARGS__), (argc)--, *(argv)++)

/// Get time in nanoseconds.
///
/// [r:] Nanoseconds from unspecified epoch.
CBUILDDEF uint64_t cbuild_time_nanos(void);
/// Number of nanoseconds in one second
#define CBUILD_NANOS_PER_SEC (long long)(1000*1000*1000)

/// Return bigger number from 2 numbers
#define CBUILD_MAX(a, b) a > b ? a : b
/// Return smaller number from 2 numbers
#define CBUILD_MIN(a, b) a < b ? a : b

//! # Internal API [line:cbuild-api-internal]
//! These are internal function, so their API can break at any time.
//! But they still can be considered mostly stable (eg. I does not
//! break their API that often).

/// Get name of program (`argv[0]`).
CBUILDDEF const char* __cbuild_progname(void);
/// `memrchr` wrapper.
CBUILDDEF void* __cbuild_memrchr(const void* s, int c, size_t n);
/// `memmem` wrapper
CBUILDDEF void* __cbuild_memmem(const void* haystack, size_t hsize,
	const void* needle, size_t nsize);
