/**
 * @file cli_parse_handler.hpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Defines and registering for handlers for cli parser
 * @date 2023-10-30
 *
 * @copyright (C) 2023 WolodiaM
 * @license GPL v3.0 or later
 *
 * Copyright (C) 2023 WolodiaM
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
#ifndef __CBUILD_CLI_PARSE_HANDLER_HPP__
#define __CBUILD_CLI_PARSE_HANDLER_HPP__
// C++ libraries
#include "string"
// Project headers
#include "CBuild.hpp"
#include "map.hpp"
// Code
namespace CBuild {
void exit(int code);
/**
 * @typedef ARG_TYPE
 * @brief Type of argument, indicate how many tokens this argument consumes,
 * including self
 */
typedef enum ARG_TYPE {
    /**
     * @brief No argument
     */
    BLANK_ARG = 0,
    /**
     * @brief Only argument and no subarguments (like -f, -v)
     */
    SIMPLE_ARG = 1,
    /**
     * @brief Task argument (-t <task_id>)
     */
    TASK_ARG = 2,
    /**
     * @brief Toolchain argument (-b/-r/-br/-c/-d <toolchain_id>)
     */
    TOOLCHAIN_ARG = 2,
    /**
     * @brief Generick argument with subargument (-<arg> <subarg>
     */
    GENERICK_ARG = 2,
    /**
     * @brief Generator arg (-g <generator_id>)
     */
    GENERATOR_ARG = 2,
    /**
     * @brief Increment by one, consumes only self
     */
    INC_ONE = 1,
    /**
     * @brief Increment by two, consumes self and 1 subarg
     */
    INC_TWO = 2,
    /**
     * @brief Increment by two, consumes self and 2 subarg
     */
    INC_THREE = 3,
    /**
     * @brief Increment by two, consumes self and 3 subarg
     */
    INC_FOUR = 4,
    /**
     * @brief Increment by two, consumes self and 4 subarg
     */
    INC_FIVE = 5,
    /**
     * @brief Increment by two, consumes self and 5 subarg
     */
    INC_SIX = 6,
    /**
     * @brief Increment by two, consumes self and 6 subarg
     */
    INC_SEVEN = 7,
    /**
     * @brief Increment by two, consumes self and 7 subarg
     */
    INC_EIGHT = 8,
    /**
     * @brief Increment by two, consumes self and 8 subarg
     */
    INC_NINE = 9,
    /**
     * @brief Increment by two, consumes self and 9 subarg
     */
    INC_TEN = 10
} ARG_TYPE;
/**
 * @brief Handler for specific argument
 *
 * Receives four arguments:
 * 	lib::map<std::string, std::string>	- arguments map
 * 	char**				- argv
 * 	int					- argc
 * 	int					- current parsing pointer, point
 * 						to argument that call handler,
 * 						default - 1 (self)
 * 	CBuild::RType				- type of run
 * Returns int - consumed elements of argv
 *
 * Possible internal args:
 *  out:none - disable cli out
 *  out:verbose - full cli out
 *  gen:<generator id> - pass used generator
 *  toolchain_id:<toolchain id> - pass used toolchain
 *  task_id:<task id> - pass used task id
 *  paX:<arg>, where X - unique number - pass arg to runned program (for -r and -br and possibly -d)
 *  aX:<arg>, where X - inique number - pass arg to runned toolchain/task
 */
using handler = ARG_TYPE (*)(lib::map<std::string, std::string>*, char**, int, int, CBuild::RType*);
/**
 * @brief Register new handler for parsing cli args, see handler typedef comment
 * to guide how to write handlers
 *
 * @param cmd => std::string -> Command that trigers this parser
 * @param parser => CBuild::handler -> Custom parsing function
 * @returns bool -> true for success and false for failure
 */
bool register_parse_handler(std::string cmd, CBuild::handler parser);
} // namespace CBuild
#endif // __CBUILD_CLI_PARSE_HANDLER_HPP__
