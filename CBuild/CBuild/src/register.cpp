/**
 * @file register.cpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Register implementation
 * @version 1.0
 * @date 2023-01-28
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
#include "../../headers/filesystem++.hpp"
#include "../../headers/map.hpp"
#include "string"
#include "vector"
// Project headers
#include "../../headers/CBuild_defs.hpp"
#include "../../headers/build/g++.hpp"
#include "../../headers/print.hpp"
#include "../../headers/register.hpp"
#include "../../headers/task/CBuild_help_task.hpp"
/* Custom tasks */
namespace CBuild {
/**
 * @brief Init task
 */
class Init : public CBuild::Task {
public:
  Init() : Task("CBuild_init", {}) {}
  void call(std::vector<std::string> __attribute_maybe_unused__) {
    // Check all directories, and create it if needed
    if (!CBuild::fs::exists(CBUILD_BUILD_DIR))
      CBuild::fs::create({CBUILD_BUILD_DIR}, CBuild::fs::DIRECTORY);
    if (!CBuild::fs::exists(CBUILD_SCRIPTS_DIR))
      CBuild::fs::create({CBUILD_SCRIPTS_DIR}, CBuild::fs::DIRECTORY);
    if (!CBuild::fs::exists(CBUILD_CACHE_DIR))
      CBuild::fs::create({CBUILD_CACHE_DIR}, CBuild::fs::DIRECTORY);
    if (!CBuild::fs::exists(CBUILD_CACHE_DIR + "/" + CBUILD_COPY_CACHE_DIR))
      CBuild::fs::create({CBUILD_CACHE_DIR + "/" + CBUILD_COPY_CACHE_DIR},
                         CBuild::fs::DIRECTORY);
  }
};
} // namespace CBuild
/* namesapce Registry */
namespace CBuild {
namespace Registry {
// System tasks
CBuild::Help help;
CBuild::Init initw;
// Registred data
// Tasks pointers ;)
lib::map<std::string, CBuild::Task *> tasks;
// Executed tasks
lib::map<std::string, bool> task_executed;
// Toolchains pointers ;)
lib::map<std::string, CBuild::Toolchain *> targets;
// Executed toolchains
lib::map<std::string, bool> target_executed;
// Custom cmd line arguments to tasks mapping
lib::map<std::string, std::string> keywords;
// Custom rebuild name
std::string name = "CBuild.run";
// Custom rebuild args
std::vector<std::string> cargs;
std::vector<std::string> largs;
// TODO other types ?
} // namespace Registry
} // namespace CBuild
/* register.hpp */
void CBuild::Registry::init() {
  // Register help task
  Registry::RegistryTask(&CBuild::Registry::help);
  // Register init task
  Registry::RegistryTask(&CBuild::Registry::initw);
}
void CBuild::Registry::RegistryTask(CBuild::Task *task) {
  // Save task ptr connected to it's name
  try {
    Registry::tasks.push_back_check(task->self_name(), task);
  } catch (std::exception &e) {
    CBuild::print_full("Error: trying to register task with the same id: " +
                           task->self_name(),
                       CBuild::RED);
  }
  // Save task excution state connected to it's name
  Registry::task_executed.push_back(task->self_name(), false);
}
void CBuild::Registry::CallTask(std::string name,
                                std::vector<std::string> args) {
  // Load executed state of task
  auto check = Registry::task_executed.get_ptr(name);
  // Check if task exixtst and wasn't executed
  if (check != NULL && check->data == false) {
    // Load task
    auto mod = Registry::tasks.get_ptr(name);
    // Call all required tasks, recursion ;)
    for (std::string s : mod->data->self_required()) {
      Registry::CallTask(s, args);
    }
    // Call task
    mod->data->call(args);
    // Mark this task es exuted, very easy because of pointers ;)
    check->data = true;
  }
  // Print error if task not found
  if (check == NULL)
    CBuild::print("Task" + name + "not found. Exiting ...", CBuild::RED);
}
void CBuild::Registry::RegisterTarget(CBuild::Toolchain *target) {
  // Save toolchain ptr connected to it's name
  try {
    Registry::targets.push_back_check(target->get_id(), target);
  } catch (std::exception &e) {
    CBuild::print_full(
        "Error: trying to register toolchain with the same id: " +
            target->get_id(),
        CBuild::RED);
  }
  // Save toolchain excution state connected to it's name
  Registry::task_executed.push_back(target->get_id(), false);
}
CBuild::Toolchain *CBuild::Registry::GetToolchain(std::string name,
                                                  bool force) {
  // If we not in force build
  if (!force) {
    // Check if toolchain was executed before
    auto check = Registry::task_executed.get_ptr(name);
    if (check != NULL && check->data == false) {
      // Return toolchain pointer if not
      auto elem = Registry::targets.get_ptr(name);
      // Mark toolcian as executed
      check->data = true;
      return elem->data;
    }
  }
  // We in force build
  else {
    // Return toolchain ptr
    auto elem = Registry::targets.get_ptr(name);
    if (elem != NULL)
      return elem->data;
  }
  // Some error occured
  return NULL;
}
lib::map<std::string, CBuild::Toolchain *> CBuild::Registry::GetTargets() {
  // Return raw map of toolchains, return copy
  return Registry::targets;
}
void CBuild::Registry::RegisterKeyword(std::string key, CBuild::Task *func) {
  // Register task
  Registry::RegistryTask(func);
  // Connect argument to this task
  try {
    Registry::keywords.push_back_check(key, func->self_name());
  } catch (std::exception &e) {
    CBuild::print_full("Error: trying to register simmilar keyword: " + key,
                       CBuild::RED);
  }
}
lib::map<std::string, std::string> CBuild::Registry::GetKeywordsList() {
  // Return list of arguments
  return Registry::keywords;
}

void CBuild::Registry::SetRebuildName(std::string _name) {
  // Save name
  Registry::name = _name;
}
void CBuild::Registry::AddLinkArg(std::string arg) {
  // Add argument
  Registry::largs.push_back(arg);
}
void CBuild::Registry::AddCompileArg(std::string arg) {
  // Add argument
  Registry::cargs.push_back(arg);
}
std::string CBuild::Registry::GetRebuildArgs() {
  // Return string
  std::string ret = " \"";
  // Get compile args and pack it to workable string
  for (auto elem : Registry::cargs) {
    ret += elem;
    ret += " ";
  }
  ret += " \" \" ";
  // Get link args and pack it to workable string
  for (auto elem : Registry::largs) {
    ret += elem;
    ret += " ";
  }
  ret += " \" ";
  // Get name and pack it to workable string
  ret += Registry::name;
  // Return string
  return ret;
}
