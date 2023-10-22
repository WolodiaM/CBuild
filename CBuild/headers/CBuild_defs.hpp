/**
 * @file CBuild_defs.hpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Defines
 * @date 2023-02-02
 *
 * @license GPL v3.0 or later
 *
 * Copyright (C) 2023  WolodiaM
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
// std::string
#include "string"
// Defines
/**
 * @brief Cache directory of CBuild
 */
#define CBUILD_CACHE_DIR std::string("cache")
/**
 * @brief tmp dir in cache dir
 */
#define CBUILD_COPY_CACHE_DIR std::string("tmp")
/**
 * @brief Hash cache in build/toolchain dir
 */
#define CBUILD_HASH_DIR std::string("hash")
/**
 * @brief Build directory of CBuild
 */
#define CBUILD_BUILD_DIR std::string("build")
/**
 * @brief Build out in build/toolchain
 */
#define CBUILD_BUILD_OUT_DIR std::string("out")
/**
 * @brief Object cache in build/toolchain
 */
#define CBUILD_BUILD_CACHE_DIR std::string("objs")
/**
 * @brief Scripts dir of CBuild
 */
#define CBUILD_SCRIPTS_DIR std::string("scripts")
/**
 * @brief Output Makefile path
 */
#define CBUILD_MAKEFILE std::string("Makefile")
/**
 * @brief Output compile_command.json path
 */
#define CBUILD_COMPILE_COMMAND_JSON std::string("compile_commands.json")
/**
 * @brief For other included project, in cache dir
 */
#define CBUILD_PROJECT_DEPS_DIR std::string("libs")
/**
 * @brief For other included project, in cache dir
 */
#define CBUILD_PROJECT_DEPS_HEADERS std::string("headers")
/**
 * @brief Hash table save file
 */
#define CBUILD_HASH_FILE std::string("hash")
/**
 * @brief Different representation of CBuild version
 */
#define CBUILD_VERSION_MAJOR (int)9
#define CBUILD_VERSION_MINOR (int)7
#define CBUILD_VERSION_STR std::string("9.7")
/**
 * @brief Data about CBuild compilation
 */
#define CBUILD_COMPILER_VERSION std::string(__VERSION__)
#define CBUILD_COMPILATION_DATE std::string(__DATE__)
#define CBUILD_COMPILATION_TIME std::string(__TIME__)
/**
 * @brief For cli autocompletion
 */
#define CBUILD_AUTOCOMPLETION_BASH std::string("bash_return")
#define CBUILD_AUTOCOMPLETION_ZSH std::string("zsh_return")
#define CBUILD_AUTOCOMPLETION_ZSH_DESCRIPTION std::string("zsh_return_desc")
