/**
 * @file generator.cpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief CBuild internal generators implementation
 * @date 2023-07-24
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
#include "array"
#include "fstream"
#include "iostream"
#include "sstream"
#include "vector"
// Project includes
#include "../../headers/CBuild.hpp"
#include "../../headers/filesystem++.hpp"
#include "../../headers/generator/ccj.hpp"
#include "../../headers/generator/makefile.hpp"
#include "../../headers/map.hpp"
#include "../../headers/print.hpp"
#include "../../headers/register.hpp"
#include "../../headers/system.hpp"
/* makefile.hpp */
void CBuild::makefile_out::generate(CBuild::RType			mode,
				    lib::map<std::string, std::string> *args) {
  CBuild::print("Generating Makefile ...", CBuild::color::MAGENTA);
  // Get command log
  auto log = CBuild::get_log();
  // Create Makefile if needed
  if (!CBuild::fs::exists(this->OUT)) {
    CBuild::fs::create({this->OUT}, CBuild::fs::FILE);
  }
  // Open file
  std::ofstream makefile;
  makefile.open(this->OUT, std::ios::app);
  std::string name = "";
  // Create name of make task, first part - CBuild run type or
  // cbuild_ placeholder
  switch (mode) {
  case CBuild::BUILD:
    name += "build_";
    break;
  case CBuild::BUILD_RUN:
    name += "build_run_";
    break;
  case CBuild::RUN:
    name += "run_";
    break;
  case CBuild::CLEAR:
    name += "clear_";
    break;
  case CBuild::DEBUG:
    name += "debug_";
    break;
  case CBuild::TASK:
    name += "task_";
    break;
  default:
    name += "cbuild_";
    break;
  }
  // Second part of make task - target or task id or cbuild
  // placeholder
  if (mode == CBuild::BUILD || mode == CBuild::BUILD_RUN ||
      mode == CBuild::RUN || mode == CBuild::DEBUG || mode == CBuild::CLEAR) {
    name += *(args->get("toolchain_id"));
  } else if (mode == CBuild::TASK) {
    name += *(args->get("task_id"));
  } else {
    name += CBuild::get_random_string(6);
  }
  // Write all executed command to Makefile
  makefile << "\n";
  makefile << name << ":\n";
  for (auto elem : *log) {
    makefile << "\t" << elem << "\n";
  }
  // End Makefile
  makefile << "\n";
  makefile.close();
}
bool CBuild::makefile_out::init() { return true; }
/* ccj.hpp - helper */
namespace CBuild {
/**
 * @brief Evaluate compilation cmd
 *
 * @param data => std::array<std::string, 4> -> Data:
 * 		- 0: File path
 * 		- 1: Object file
 * 		- 2: Compiler command
 * 		- 3: Args
 * @return std::string -> Command
 */
std::string eval_cmd(std::array<std::string, 4> data) {
  std::string cmd = data[2] + " -c ";
  cmd += data[0];
  cmd += " ";
  cmd += data[3];
  cmd += " -o ";
  cmd += data[1];
  return cmd;
}
} // namespace CBuild
/* ccj.hpp */
void CBuild::ccj_out::generate(CBuild::RType			   mode,
			       lib::map<std::string, std::string> *args) {
  // Fail fast if we not building
  if (mode != CBuild::BUILD) {
    CBuild::print("Not in build mode, ERROR!", CBuild::RED);
    return;
  }
  // Cannot generate compile_commands.json for meta-toolchain 'all'
  if (*(args->get("toolchain_id")) == std::string("all")) {
    CBuild::print("Cannot generate compile commands for meta-toolchain \"all\"",
		  CBuild::RED);
    return;
  }
  // Get commands
  // Vars
  std::vector<std::pair<std::string, std::string>> data;
  // Get data from target
  auto toolchain = CBuild::Registry::GetToolchain(*(args->get("toolchain_id")));
  std::vector<std::string> dummy_vec = {};
  toolchain->call(&dummy_vec, true, false, true);
  auto files = toolchain->gen_file_list_force();
  auto cmds  = toolchain->get_cmds();
  auto targs = toolchain->get_compile_args();
  for (auto file : files) {
    data.push_back(std::pair<std::string, std::string>(
	file.key, CBuild::eval_cmd(std::array<std::string, 4>{
		      file.key, file.data, cmds[0], targs})));
  }
  // Store in file
  // Create file if needed
  if (!CBuild::fs::exists(this->OUT)) {
    CBuild::fs::create({this->OUT}, CBuild::fs::FILE);
  }
  std::ofstream file(this->OUT);
  // Store data
  CBuild::print("Generating compile_commands.json ...", CBuild::color::MAGENTA);
  file << "[\n";
  // For every generated command
  bool first = true;
  for (unsigned int i = 0; i < data.size(); i++) {
    // Get line data
    // Preload some data to .json
    if (first == false)
      file << ",\n";
    first = false;
    file << "{\n";
    // Add \ for needed data
    std::string	       cmd_prep = "";
    std::ostringstream ss;
    for (char c : data.at(i).second) {
      switch (c) {
      case '\"':
	ss << "\\\"";
	break;
      case '\\':
	ss << "\\\\";
	break;
      case '/':
	ss << "\\/";
	break;
      case '\b':
	ss << "\\b";
	break;
      case '\f':
	ss << "\\f";
	break;
      case '\n':
	ss << "\\n";
	break;
      case '\r':
	ss << "\\r";
	break;
      case '\t':
	ss << "\\t";
	break;
      case '$':
	ss << "$";
	break;
      default:
	ss << c;
	break;
      }
    }
    // Other data, properly formatted
    cmd_prep = ss.str();
    file << "\t\"directory\": \"";
    file << args->get_ptr("curr_path")->data;
    file << "\",\n";
    file << "\t\"command\": \"";
    file << cmd_prep;
    file << "\",\n";
    file << "\t\"file\": \"";
    file << data.at(i).first;
    file << "\"\n";
    file << "}";
  }
  file << "\n]\n";
  // Close file
  file.flush();
  file.close();
}
bool CBuild::ccj_out::init() {
  CBuild::disable_system();
  return false;
}
