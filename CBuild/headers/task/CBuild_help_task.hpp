/**
 * @file CBuild_help_task.hpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Help print routine for CBuild
 * @date 2023-06-08
 *
 * @copyright (C) 2023 WolodiaM
 * @license GPL v3.0 or later
 *
 * Copyright (C) 2022  WolodiaM
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
// Include c++ libraries
#include "string"
// Include CBuild headers
#include "../print.hpp"
#include "Task.hpp"
namespace CBuild {
class Help : public CBuild::Task {
  private:
    std::string data = "CBuild help - v9.9\nCommand line options:\n\t `-f` - force recompilation of all files.\n\t `-v` - verbose output, some addition info and some debugging info, mask `--no-cli-out` if used before.\n\t `--no-cli-out` - no output from CBuild at all, mask `-v` if used before.\n\t `-g make` - generate Makefile based on output from CBuild, simly log all executed command and pack it in Makefile syntax, so, sadly, CBuild::fs calls not propagate to Makefile.\n\t `-g ccj` - generate compile_commands.json, use internal command evaluator in CBuild::Toolchain, from the box work only for toolchains, that use standard compiler calling convention, see below.\n\t `-ld <toolchain_name>` - load dependencies of specified target, only for CBuild dependencies.\n\t `-b <toolchain_name>` - build specified target.\n\t `-b all` - build all registered targets\n\t `-r <toolchain_name>` - run specified target.\n\t `-br <toolchain_name>` - build and then run specified target.\n\t `-d <toolchain_name>` - build and then run specified target in debug mode. Now work only with gdb on linux.\n\t `-c <toolchain_name>` - clear all data of specified target.\n\t `-t <task_name>` - run specified task.\n\t `-a <arg>` - set some argument (see it as #define arg, without value). It can be used by ran toolchain or ran task. No preprocessing, simple propagation for user-defined arguments, preserver order in what arguments is specified.\n\t `-pa <arg>` - specify argument for ran app, propagated to app ran by `-r` and `-br`, in `-d` arguments propagates to debugger.\n\t `--rebuild` - rebuild CBuild executable.\n\t `--init` - init CBuild folder structure, not always required.\n\t `--help` - print help message - this information.\n\t `--version` - print curent version of CBuild\n\t `--task-list` - print list of all registered tasks\n\t `--target-list` - print list of all registered targets\n\t `--generator-list` - print list of all registered generators\n\t `--commands-list` - print list of all registered user commands\nCBuild folder structure: \n<project_root>\n |\n | - src                    : Project sources folder\n |    |\n |    | - main.cpp          : Have main() function implemented\n |\n | - scripts\n |    |\n |    | - main.cpp          : Main file of CBuild user scripts, init CBuild and call user init\n |    | - user_init.cpp     : Implement user init function, have build sript implemented\n |    | - user_init.hpp     : Header to glue together two .cpp files\n | - cache                  : CBuild dir for some temporary data\n |    |\n |    | - headers           : Header of included CBuild projects\n |    | - libs              : .so / .a files of included CBuild projects\n |    | - tmp               : Directory for temporary runtime data of CBuild \n |\n | - build\n |    |\n |    | - <toolchain_name>\n |    |    | - hash         : Hash storage directory\n |    |    |    |\n |    |    |    | - hash    : Hash database for specified target\n |    |    | - objs         : Saved .o files of specified target\n |    |    | - out          : Output files of specified target\n |    |\n |\n | - CBuild.run             : Main executable of CBuild\nSome useful information: \n     Compiler calling convention:: \n\t - All compilations is done in two steps: compilation `<compiler> -c <file> <arguments> -o <object>` and then linking `<linker> <objects> <arguments> -o <output binary>`\n";

  public:
    Help() : CBuild::Task("CBuild_help", {}){};
    void call(std::vector<std::string> args __attribute_maybe_unused__) {
        CBuild::print(data);
    }
};
} // namespace CBuild
