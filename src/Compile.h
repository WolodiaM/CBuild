#pragma once // For LSP
//! Some helpers specifically for a compilation
//!
//! # `CBUILD_CC` [line:cbuild-compiler-self]
//!
//! This will hold compiler command for compiler that was used to compiler CBuild.
//!
//! # `CC` [line:cbuild-bare-cc]
//!
//! CBuild can provide bare define `CC`. Because it is not prefixed it can be disabled.
//! This defined will be an alias to `CBUILD_CC`. If `CBUILD_NO_CC` is defined, CBuild
//! will not provide `CC` define.

#include "Common.h"
#include "Command.h"

//@ cbuild-compiler-self
#if !defined(CBUILD_CC)
	#if defined(CBUILD_CC_CLANG)
		#define CBUILD_CC "clang"
	#elif defined(CBUILD_CC_GCC)
		#define CBUILD_CC "gcc"
	#else
		#define CBUILD_CC "cc"
	#endif // Compiler select
#endif // CBUILD_CC

//@ cbuild-bare-cc
#if !defined(CBUILD_NO_CC)
	#define CC CBUILD_CC
#endif

/// Default warning options.
/// Enables almost all warnings except `-Wcomments` and `-Woverride-init`.
#define CBUILD_CARGS_WARN \
	"-Wall", "-Wextra", "-Wno-comments", "-Wconversion", "-Wcast-align", "-Wvla", \
	"-Wno-override-init", "-Wshadow"
/// Promote all warnings to errors.
#define CBUILD_CARGS_WERROR           "-Werror"
/// Setup profiles,
#define CBUILD_CARGS_PROFILE          "-pg"
/// Add debug symbols,
#define CBUILD_CARGS_DEBUG            "-g"
/// Add debug symbols targeting **gdb**.
#define CBUILD_CARGS_DEBUG_GDB        "-ggdb"
/// Enable multithreading (eg. add `-pthread` for POSIX).
#define CBUILD_CARGS_MT               "-pthread"
/// Add a define based on compile-time literal.
#define CBUILD_CARGS_DEFINE(defname)  "-D" defname
/// Add a define based on runtime string.
/// Uses [`cbuild_temp_malloc`](Temp.html#cbuild_temp_malloc).
#define CBUILD_CARGS_DEFINE_TEMP(defname) \
	cbuild_temp_sprintf("-D%s", defname)
/// Add a define with a value. Both based on compile-time literal.
#define CBUILD_CARGS_DEFINE_VAL(defname, val)      "-D" defname "=" val
/// Add a define with a value. Both based on runtime string.
/// Uses [`cbuild_temp_malloc`](Temp.html#cbuild_temp_malloc).
#define CBUILD_CARGS_DEFINE_VAL_TEMP(defname, val) \
	cbuild_temp_sprintf("-D%s=%s", defname, val)
/// Remove define based on compile-time literal.
#define CBUILD_CARGS_UNDEF(defname)   "-U" defname
/// Add include based on compile-time literal.
#define CBUILD_CARGS_INCLUDE(file)    "--include", file
/// Add library include (`-l`) based on compile-time literal.
#define CBUILD_CARGS_LIBINCLUDE(lib)  "-l" lib
/// Add library directories (`-I` and `-L`) based on compile-time literal.
#define CBUILD_CARGS_LIBDIR(src, obj) "-I" src, "-L" obj
/// Set standard based on compile-time literal.
#define CBUILD_CARGS_STD(std)         "-std=" std

/// Simple wrapper for self-rebuild. Only `argv[0]`{.c} is used.
/// Other elements are used only to re-exec new binary with same arguments.
#define cbuild_selfrebuild(argc, argv)                                         \
	do {                                                                         \
		const char* expanded_file = __FILE__;                                      \
		cbuild_cmd_t files = { .data = &expanded_file, .size = 1 };                \
		__cbuild_selfrebuild(argc, argv, argv[0], files);                          \
	} while(0)
/// Complex wrapper for self-rebuild. Only `argv[0]`{.c} is used.
/// This function allow to specify additional files to be checked when
/// decision is made to rebuild binary or not.
#define cbuild_selfrebuild_ex(argc, argv, ...)                                 \
	do {                                                                         \
		const char* tmp_files[] = {__FILE__ __VA_OPT__(,) __VA_ARGS__ };           \
		size_t tmp_files_len = cbuild_arr_len(tmp_files);                          \
		cbuild_cmd_t files = { .data = tmp_files, .size = tmp_files_len };         \
		__cbuild_selfrebuild(argc, argv, argv[0], files);                          \
	} while(0)
/// Perms self-rebuild.
/// 
/// * [pl:argc] ARGC of program.
/// * [pl:argv] ARGV of program.
/// * [pl:argv0_path] Path to file specified by `argv[0]`{.c}.
/// * [pl:files] Additional dependency files.
///
/// # When this will result in a new binary
///
/// If any of the files from [p:files] are newer than `argv[0]`{.c}.
/// If any of files in this array does not exist program will exit with error.
///
/// # How rebuild is performed
///
/// `CBUILD_CC` is used as compiler. `CBUILD_CARGS_WARN` is used as default set
/// of arguments. [`cbuild_selfrebuild_hook`](#cbuild_selfrebuild_hook) is called
/// to resolve dynamic or user-provided arguments.
CBUILDDEF void __cbuild_selfrebuild(int argc, char** argv,
	const char* argv0_path, cbuild_cmd_t files);
/// This function will receive self-rebuild command with all arguments
/// applied but without files. It can modify it to add arguments.
extern void (*cbuild_selfrebuild_hook)(cbuild_cmd_t* cmd);
/// Compare mtime of 2 files.
/// Same check as done by `make`.
///
/// [r:=0] - Output is newer than input.
/// [r:<0] - Error.
/// [r:>0] - Output is older than input.
CBUILDDEF int cbuild_compare_mtime(const char* output, const char* input);
/// Compare mtime of 1 output file and multiple input files.
/// Same check as done by `make`.
///
/// [r:=0] - Output is newer than input.
/// [r:<0] - Error.
/// [r:>0] - Output is older than input. Number of files that are newer.
CBUILDDEF int cbuild_compare_mtime_many(const char* output, const char** inputs,
	size_t num_inputs);
