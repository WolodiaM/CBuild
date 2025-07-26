/**
 * @file Compile.h
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Compilation helper
 * Providde self-rebuild macro and some more macroses
 *
 * @date 2024-12-09
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
#ifndef __CBUILD_COMPILE_H__
#define __CBUILD_COMPILE_H__
// Project includes
#include "Command.h"
#include "common.h"
// Code
// _MSC_VER could be used to detect MSVC
#if !defined(CBUILD_CC)
#	if defined(__clang__)
#		define CBUILD_CC "clang"
#	elif defined(__MINGW32__)
#		ifdef _WIN32
#			define CBUILD_CC "gcc"
#		else
#			define CBUILD_CC "x86_64-w64-mingw32-gcc"
#		endif // _WIN32
#	elif defined(__CYGWIN__) && defined(_WIN32)
#		define CBUILD_CC "gcc"
#	elif defined(__GNUC__)
#		define CBUILD_CC "gcc"
#	else
#		define CBUILD_CC "cc"
#	endif // Compiler select
#endif   // CBUILD_CC
#if !defined(CBUILD_LD)
#	if defined(__clang__)
#		define CBUILD_LD "clang"
#	elif defined(__MINGW32__)
#		ifdef _WIN32
#			define CBUILD_LD "gcc"
#		else
#			define CBUILD_LD "x86_64-w64-mingw32-gcc"
#		endif // _WIN32
#	elif defined(__CYGWIN__) && defined(_WIN32)
#		define CBUILD_LD "gcc"
#	elif defined(__GNUC__)
#		define CBUILD_LD "gcc"
#	else
#		define CBUILD_LD "cc"
#	endif // Compiler select
#endif   // CBUILD_LD
#if !defined(CBUILD_CXX)
#	if defined(__clang__)
#		define CBUILD_CXX "clang++"
#	elif defined(__MINGW32__)
#		ifdef _WIN32
#			define CBUILD_CXX "g++"
#		else
#			define CBUILD_CXX "x86_64-w64-mingw32-g++"
#		endif // _WIN32
#	elif defined(__CYGWIN__) && defined(_WIN32)
#		define CBUILD_CXX "g++"
#	elif defined(__GNUC__)
#		define CBUILD_CXX "g++"
#	else
#		define CBUILD_CXX "cc -x c++"
#	endif // Compiler select
#endif   // CBUILD_CXX
#if !defined(CBUILD_AR)
#	if defined(__clang__)
#		define CBUILD_AR "llvm-ar"
#	elif defined(__MINGW32__)
#		ifdef _WIN32
#			define CBUILD_AR "ar"
#		else
#			define CBUILD_AR "x86_64-w64-mingw32-ar"
#		endif // _WIN32
#	elif defined(__CYGWIN__) && defined(_WIN32)
#		define CBUILD_AR "ar"
#	elif defined(__GNUC__)
#		define CBUILD_AR "ar"
#	else
#		define CBUILD_AR "ar"
#	endif // Compiler select
#endif   // CBUILD_AR
#if !defined(CBUILD_AS)
#	if defined(__clang__)
#		define CBUILD_AS "llvm-as"
#	elif defined(__MINGW32__)
#		ifdef _WIN32
#			define CBUILD_AS "as"
#		else
#			define CBUILD_AS "x86_64-w64-mingw32-as"
#		endif // _WIN32
#	elif defined(__CYGWIN__) && defined(_WIN32)
#		define CBUILD_AS "as"
#	elif defined(__GNUC__)
#		define CBUILD_AS "as"
#	else
#		define CBUILD_AS "as"
#	endif // Compiler select
#endif   // CBUILD_AS
#if !defined(CBUILD_CPP)
#	define CBUILD_CPP CBUILD_CC "-E"
#endif   // CPP
#if !defined(CBUILD_CC_OUT)
#	if defined(__clang__)
#		define CBUILD_CC_OUT "-o"
#	elif defined(__MINGW32__)
#		ifdef _WIN32
#			define CBUILD_CC_OUT "-o"
#		else
#			define CBUILD_CC_OUT "-o"
#		endif // _WIN32
#	elif defined(__CYGWIN__) && defined(_WIN32)
#		define CBUILD_CC_OUT "-o"
#	elif defined(__GNUC__)
#		define CBUILD_CC_OUT "-o"
#	else
#		define CBUILD_CC_OUT "-o"
#	endif // Compiler select
#endif   // CBUILD_CC_OUT
// Predefined compiler arguments
#define CBUILD_CARGS_WARN                     "-Wall", "-Wextra", "-Wno-comments", "-Wconversion", "-Wcast-align"
#define CBUILD_CARGS_WERROR                   "-Werror"
#if defined(__clang__)
# define CBUILD_CARGS_STATIC_ANALYZER         "--analyze", "-Xanalyzer", "-analyzer-output=text"
#else
# define CBUILD_CARGS_STATIC_ANALYZER         "-fanalyzer", "-Wanalyzer-too-complex"
#endif // Compiler select (clang/gcc)
#define CBUILD_CARGS_PROFILE                  "-pg"
#define CBUILD_CARGS_DEBUG                    "-g"
#define CBUILD_CARGS_MT                       "-pthread"
#define CBUILD_CARGS_DEFINE(defname)          "-D" defname
#define CBUILD_CARGS_DEFINE_VAL(defname, val) "-D" defname "=" val
#define CBUILD_CARGS_UNDEF(defname)           "-U" defname
#define CBUILD_CARGS_INCLUDE(file)            "--include \"" file "\""
#define CBUILD_CARGS_LIBINCLUDE(lib)          "-l" lib
#define CBUILD_CARGS_LIBDIR(src, obj)         "-I" src, "-L" obj
#define CBUILD_CARGS_STD(std)                 "-std=" std
// Self-rebuild wrapper macro
#define cbuild_selfrebuild(argc, argv)                                         \
	__cbuild_selfrebuild(argc, argv, __FILE__)
/**
 * @brief Enables self-rebuild functionality
 *
 * @param argc => int -> Number of cli arguments
 * @param argv => char** -> Params passed to an app. Needed to get binary path.
 * @param spath => const char* -> Source file path. Will be generated by a
 * wrapper macro
 */
void __cbuild_selfrebuild(int argc, char** argv, const char* const spath);
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
int cbuild_compare_mtime(const char* output, const char* input);
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
int cbuild_compare_mtime_many(const char* output, const char** inputs,
                              size_t num_inputs);
#endif // __CBUILD_COMPILE_H__
