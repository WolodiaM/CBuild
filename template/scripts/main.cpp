/**
 * @file main.cpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Main user file of CBuild
 * @version 1.0
 * @date 2023-01-18
 *
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
// C++ libraries
#include "filesystem"
#include "stdlib.h"
#include "string"
#include "vector"
// CBuild headers
#include "CBuild/CBuild.hpp"
// Userspace headers
#include "user_init.hpp"
// Rebuild call
void rebuild() {
  auto path = std::string(std::filesystem::current_path().c_str());
  path += "/scripts";
  CBuild::rebuild(path);
}
// Main function
int main(int argc, char **argv, char **envp) {
  // Run user init
  init();
  // Hold parsed command line arguments, see CBuild::parse in CBuild.cpp for
  // more details
  lib::map<std::string, std::string> args;
  // Parse arguments, and also get type of run
  CBuild::RType mode = CBuild::parse(&args, argc, argv);
  // We have some error
  if (mode == CBuild::ERROR)
    exit(0xFF);
  // If we need to rebuild
  if (mode == CBuild::REBUILD)
    rebuild();
  // Add base path
  args.push_back("curr_path",
                 std::string(std::filesystem::current_path().c_str()));
  // Run main loop of CBuild (execute given toolchain / module and exit)
  CBuild::loop(mode, &args);
  // Safe exit without errors
  return 0;
}
