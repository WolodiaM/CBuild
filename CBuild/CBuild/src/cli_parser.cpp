/**
 * @file cli_parser.cpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief CBuild cli arguments parser
 * @date 2023-08-22
 *
 * @copyright (C) 2023 WolodiaM
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
// C++ libraries
#include "string"
#include "vector"
// Project includes
#include "../../headers/CBuild.hpp"
#include "../../headers/map.hpp"
#include "../../headers/print.hpp"
#include "../../headers/register.hpp"
/*	SPECIFICATIONS
 * - Parse arguments one by one
 * - If invelid argument/argument with modifier, not have modifier - exit with
 *	error message
 * - If no args - exit with error message
 * - If one of internl tasks (--init, --version, --help) - clear all and return
 *immideately
 * - map entries:
 *   	a<num> - argumenent, val = string
 *   	pa<num> - program argument, val = string
 *   	force - force mode, val = "force"
 *	out - output mode, val = "verbose", "none"
 *	gen - generator, val = CBuild::Registry::GetGeneratorsList()
 *	toolchain_id - toolchain, val = CBuild::Registry::GetToolchainsList()
 *	task_id - task, val = CBuild::Registry::GetTasksList(), "CBuild_init",
 *		"CBuild_version", "CBuild_help"
 *
 *## CLI arguments
 * `-f` - force recompilation of all files.
 * `-v` - verbose output, some addition info and some debugging info, mask
 *	`--no-cli-out` if used before.
 * `--no-cli-out` - no output from CBuild at all, mask `-v` if used before.
 * `-g make` - generate Makefile based on output from CBuild, simly log all
 *	executed command and pack it in Makefile syntax, so, sadly, CBuild::fs
 *	calls not propagate to Makefile.
 * `-g ccj` - generate compile_commands.json, use internal command evaluator in
 *	CBuild::Toolchain, from the box work only for toolchains, that use
 *	standard compiler calling convention, see below.
 * `-ld <toolchain_name>` - load dependencies of specified target, only for
 *	CBuild dependencies.
 * `-b <toolchain_name>` - build specified target.
 * `-b all` - build all registered targets
 * `-r <toolchain_name>` - run specified target.
 * `-br <toolchain_name>` - build and then run specified target.
 * `-d <toolchain_name>` - build and then run specified target in debug mode.
 *	Now work only with gdb on linux.
 * `-c <toolchain_name>` - clear all data of specified target.
 * `-t <task_name>` - run specified task.
 * `-a <arg>` - set some argument (see it as #define arg, without value). It can
 *	be used by ran toolchain or ran task. No preprocessing, simple
 *	propagation for user-defined arguments, preserver order in what
 *	arguments is specified.
 * `-pa <arg>` - specify argument for ran app, propagated to app ran by `-r` and
 *	`-br`, in `-d` arguments propagates to debugger.
 * `--rebuild` - rebuild CBuild executable.
 * `--init` - init CBuild folder structure, not always required.
 * `--help` - print help message - this information.
 * `--version` - print curent version of CBuild
 * `--get-cli-autocompetition` - generate autocompetition data for bash, usage
 *	with custom autocompetition script
 */
/* data */
namespace CBuild {
bool real_exit = true;
void exit(int code) {
  if (CBuild::real_exit) {
    std::exit(code);
  }
}
enum ARG_TYPE {
  BLANK_ARG     = 0,
  SIMPLE_ARG    = 1,
  TASK_ARG      = 2,
  TOOLCHAIN_ARG = 2,
  GENERICK_ARG  = 2,
  GENERATOR_ARG = 2
};
/**
 * @brief Handler for specific argument
 *
 * Receives four arguments:
 * 	lib::map<std::string, std::string>	- arguments map
 * 	char**					- argv
 * 	int					- argc
 * 	int					- current parsing pointer, point
 * 						to argument that call handler,
 * 						default - 1 (self)
 * 	CBuild::RType				- type of run
 * Returns int - consumed elements of argv
 */
using handler = ARG_TYPE (*)(lib::map<std::string, std::string> *, char **, int,
                             int, CBuild::RType *);
ARG_TYPE force_handler(lib::map<std::string, std::string> *args, char **argv,
                       int argc, int ptr, CBuild::RType *type) {
  try {
    args->push_back_check("force", "force");
  } catch (std::exception &e) {
    CBuild::print_full("Force recompilation mode is already set!)",
                       CBuild::RED);
  }
  return CBuild::ARG_TYPE::SIMPLE_ARG;
}
ARG_TYPE out_handler(lib::map<std::string, std::string> *args, char **argv,
                     int argc, int ptr, CBuild::RType *type) {
  std::string str = "";
  // Select proper output type based on token
  if (std::string(argv[ptr]) == std::string("-v")) {
    str = "verbose";
  } else if (std::string(argv[ptr]) == std::string("--no-cli-out")) {
    str = "none";
  } else {
    CBuild::print(std::string("Illegal output mode! - \"") +
                      std::string(argv[ptr]) + std::string("\""),
                  CBuild::RED);
    exit(0xFF);
  }
  // Return it to the rest of CBuild
  try {
    args->push_back_check("out", str);
  } catch (std::exception &e) {
    CBuild::print_full(
        std::string("Output mode is already set! New output mode - \"") +
            std::string(argv[ptr + 1]) +
            std::string("\", old output mode is - \"") +
            std::string(args->get_ptr("out")->data) + std::string("\"!"),
        CBuild::RED);
  }
  return CBuild::ARG_TYPE::SIMPLE_ARG;
}
ARG_TYPE generator_handler(lib::map<std::string, std::string> *args,
                           char **argv, int argc, int ptr,
                           CBuild::RType *type) {
  // User do not provide needed argument (ptr is current argument, so (ptr + 1)
  // is next argument
  if (!(argc >= (ptr + 1))) {
    CBuild::print("Illegal command format!", CBuild::RED);
    CBuild::print("Usage: -g <generator id>", CBuild::GREEN);
    exit(0xFF);
  }
  // Get genrators and check if one of them match our needed
  std::vector<std::string> gens = CBuild::Registry::GetGeneratorsList();
  bool                     err  = true;
  for (std::string gen : gens) {
    if (gen == std::string(argv[ptr + 1])) {
      err = false;
    }
  }
  // If no match, yes, we are safe to crash here and remaind proper format of
  // the argument to user
  if (err == true) {
    CBuild::print(std::string("Illegal or unregistered generator name! - \"") +
                      std::string(argv[ptr + 1]) + std::string("\""),
                  CBuild::RED);
    CBuild::print("Usage: -g <generator id>", CBuild::GREEN);
    exit(0xFF);
  }
  // If all good - save selected generator for later
  try {
    args->push_back_check("gen", argv[ptr + 1]);
  } catch (std::exception &e) {
    CBuild::print_full(
        std::string("Generator is already set! New generator - \"") +
            std::string(argv[ptr + 1]) +
            std::string("\", old generator is - \"") +
            std::string(args->get_ptr("gen")->data) + std::string("\"!"),
        CBuild::RED);
  }
  return CBuild::ARG_TYPE::GENERATOR_ARG;
}
ARG_TYPE build_handler(lib::map<std::string, std::string> *args, char **argv,
                       int argc, int ptr, CBuild::RType *type) {
  // User do not provide needed argument (ptr is current argument, so (ptr + 1)
  // is next argument
  if (!(argc >= (ptr + 1))) {
    CBuild::print("Illegal command format!", CBuild::RED);
    CBuild::print(std::string("Usage: ") + std::string(argv[ptr]) +
                      std::string(" <toolchain id>"),
                  CBuild::GREEN);
    exit(0xFF);
  }
  // Now we need to figure why user run this app
  // Lets save current token to std::string to ease of use
  std::string token = argv[ptr];
  // There go boring comparisions
  if (token == std::string("-b")) {
    *type = CBuild::BUILD;
  } else if (token == std::string("-br")) {
    *type = CBuild::BUILD_RUN;
  } else if (token == std::string("-r")) {
    *type = CBuild::RUN;
  } else if (token == std::string("-d")) {
    *type = CBuild::DEBUG;
  } else if (token == std::string("-c")) {
    *type = CBuild::CLEAR;
  } else if (token == std::string("-ld")) {
    *type = CBuild::LOAD_DEPS;
  }
  // Get toolchains and check if one of them match our needed
  std::vector<std::string> tools = CBuild::Registry::GetToolchainsList();
  bool                     err   = true;
  for (std::string tool : tools) {
    if (tool == std::string(argv[ptr + 1])) {
      err = false;
    }
  }
  // We have exception for -b all
  if (*type == CBuild::BUILD &&
      std::string(argv[ptr + 1]) == std::string("all")) {
    err = false;
  }
  // If no match, yes, we are safe to crash here and remaind proper format of
  // the argument to user
  if (err == true) {
    CBuild::print(std::string("Illegal or unregistered toolchain name! - \"") +
                      std::string(argv[ptr + 1]) + std::string("\""),
                  CBuild::RED);
    CBuild::print(std::string("Usage: ") + std::string(argv[ptr]) +
                      std::string(" <toolchain id>"),
                  CBuild::GREEN);
    exit(0xFF);
  }
  // If all good - save selected toolchain for later
  try {
    args->push_back_check("toolchain_id", argv[ptr + 1]);
  } catch (std::exception &e) {
    CBuild::print_full(
        std::string("Toolchain is already set! New toolchain - \"") +
            std::string(argv[ptr + 1]) +
            std::string("\", old toolchain is - \"") +
            std::string(args->get_ptr("toolchain_id")->data) +
            std::string("\"!"),
        CBuild::RED);
  }
  return CBuild::ARG_TYPE::TOOLCHAIN_ARG;
}
ARG_TYPE task_handler(lib::map<std::string, std::string> *args, char **argv,
                      int argc, int ptr, CBuild::RType *type) {
  // User do not provide needed argument (ptr is current argument, so (ptr + 1)
  // is next argument
  if (!(argc > (ptr + 1))) {
    CBuild::print("Illegal command format!", CBuild::RED);
    CBuild::print(std::string("Usage: -t <task id>"), CBuild::GREEN);
    exit(0xFF);
  }
  // Get task from user
  std::string task = argv[ptr + 1];
  // Get registered tasks
  auto tasks = CBuild::Registry::GetTasksList();
  // Check if task is registered
  bool err = true;
  for (std::string t : tasks) {
    if (task == t) {
      err = false;
    }
  }
  // If no match, yes, we are safe to crash here and remaind proper format of
  // the argument to user
  if (err == true) {
    CBuild::print(std::string("Illegal or unregistered task name! - \"") +
                      std::string(argv[ptr + 1]) + std::string("\""),
                  CBuild::RED);
    CBuild::print(std::string("Usage: -t <task id>"), CBuild::GREEN);
    exit(0xFF);
  }
  // Return correct data
  args->push_back_check("task_id", task);
  try {
    args->push_back_check("task_id", argv[ptr + 1]);
  } catch (std::exception &e) {
    CBuild::print_full(
        std::string("Task is already set! New task - \"") +
            std::string(argv[ptr + 1]) + std::string("\", old task is - \"") +
            std::string(args->get_ptr("task_id")->data) + std::string("\"!"),
        CBuild::RED);
  }
  *type = CBuild::TASK;
  return CBuild::ARG_TYPE::TASK_ARG;
}
// Some vars for arg and parg
int      arg  = 0;
int      parg = 0;
ARG_TYPE arg_handler(lib::map<std::string, std::string> *args, char **argv,
                     int argc, int ptr, CBuild::RType *type) {
  // User do not provide needed argument (ptr is current argument, so (ptr + 1)
  // is next argument
  if (!(argc < (ptr + 1))) {
    CBuild::print("Illegal command format!", CBuild::RED);
    CBuild::print(std::string("Usage: ") + std::string(argv[ptr]) +
                      std::string(" <argument>"),
                  CBuild::GREEN);
    exit(0xFF);
  }
  // Check `pa`
  if (std::string(argv[ptr]) == std::string("-pa")) {
    try {
      args->push_back_check(std::string("pa") + std::to_string(CBuild::parg),
                            argv[ptr + 1]);
    } catch (std::exception &e) {
    }
    CBuild::parg++;
    // Check `a`
  } else if (std::string(argv[ptr]) == std::string("-a")) {
    try {
      args->push_back_check(std::string("a") + std::to_string(CBuild::arg),
                            argv[ptr + 1]);
    } catch (std::exception &e) {
    }
    CBuild::arg++;
    // Else - error
  } else {
    CBuild::print(std::string("Uknown argument! - ") + std::string(argv[ptr]),
                  CBuild::RED);
  }
  // Repack data to other format ;)
  return CBuild::ARG_TYPE::GENERICK_ARG;
}
ARG_TYPE rebuild_handler(lib::map<std::string, std::string> *args, char **argv,
                         int argc, int ptr, CBuild::RType *type) {
  *type = CBuild::REBUILD;
  return CBuild::ARG_TYPE::SIMPLE_ARG;
}

/**
 * @brief All handlers
 */
lib::map<std::string, handler> arguments = {
    {"-f", &force_handler},         {"-v", &out_handler},
    {"--no-cli-out", &out_handler}, {"-g", &generator_handler},
    {"-b", &build_handler},         {"-r", &build_handler},
    {"-br", &build_handler},        {"-d", &build_handler},
    {"-ld", &build_handler},        {"-c", &build_handler},
    {"-t", &task_handler},          {"-a", &arg_handler},
    {"-pa", &arg_handler},          {"--rebuild", &rebuild_handler}};
} // namespace CBuild
/* cli_parser.hpp */
CBuild::RType CBuild::parse(lib::map<std::string, std::string> *args, int argc,
                            char **argv) {
  // Init CBuild registry
  CBuild::Registry::init();
  // Some internal variables
  CBuild::RType ret      = CBuild::ERROR;
  int           ptr      = 1;
  auto          keywords = CBuild::Registry::GetKeywordsList();
  // For every token
  while (ptr < argc) {
    // Get handler
    auto             h   = CBuild::arguments.get_ptr(argv[ptr]);
    CBuild::ARG_TYPE inc = CBuild::BLANK_ARG;
    // If it is available
    if (h != NULL) {
      // Call it
      inc = h->data(args, argv, argc, ptr, &ret);
    } else {
      // Else check for custom keywords
      const std::string *task = keywords.get(argv[ptr]);
      if (task != NULL) {
        ret = CBuild::TASK;
        try {
          args->push_back_check("task_id", *task);
        } catch (std::exception &e) {
        }
        inc = CBuild::ARG_TYPE::SIMPLE_ARG;
      } else {
        // Else - error
        CBuild::print(std::string("Illegal argument! - ") +
                          std::string(argv[ptr]),
                      CBuild::RED);
        exit(0xFF);
      }
    }
    // Handler must return number of consumed tokens, so, increment pointer by
    // this number
    ptr += inc;
  }
  return ret;
}
