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
#include "../../headers/generator/ccj.hpp"
#include "../../headers/generator/generator.hpp"
#include "../../headers/generator/makefile.hpp"
#include "../../headers/print.hpp"
#include "../../headers/register.hpp"
#include "../../headers/task/CBuild_help_task.hpp"
// Bugfix
namespace CBuild {
namespace Registry {
// Version handler
void (*handler)();
} // namespace Registry
} // namespace CBuild
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
/**
 * @brief Version task
 */
class Version : public CBuild::Task {
  public:
    Version() : CBuild::Task("CBuild_version", {}) {}
    void call(std::vector<std::string> __attribute_maybe_unused__) {
        CBuild::print(std::string("CBuild version ") + CBUILD_VERSION_STR +
                      std::string(" by WolodiaM"));
        CBuild::print(std::string("Compiled by GCC ") +
                      CBUILD_COMPILER_VERSION);
        CBuild::print(std::string("Compiled on ") + CBUILD_COMPILATION_DATE +
                      std::string(" at ") + CBUILD_COMPILATION_TIME);
        CBuild::print("");
        CBuild::print("Licensed under GPL v3 or later");
        CBuild::print("Copyright (C) 2023 WolodiaM");
        CBuild::print("");
        CBuild::Registry::handler();
    }
};
class TaskList : public CBuild::Task {
  public:
    TaskList() : CBuild::Task("CBuild_tasklist", {}) {}
    void call(std::vector<std::string> __attribute_maybe_unused__) {
        CBuild::print("Registered tasks:");
        for (auto task : CBuild::Registry::GetTasksList()) {
            CBuild::print(std::string("\t") + task);
        }
    }
};
class ToolchainList : public CBuild::Task {
  public:
    ToolchainList() : CBuild::Task("CBuild_toolchainlist", {}) {}
    void call(std::vector<std::string> __attribute_maybe_unused__) {
        CBuild::print("Registered targets:");
        for (auto toolchain : CBuild::Registry::GetToolchainsList()) {
            CBuild::print(std::string("\t") + toolchain);
        }
    }
};
class GeneratorList : public CBuild::Task {
  public:
    GeneratorList() : CBuild::Task("CBuild_generatorlist", {}) {}
    void call(std::vector<std::string> __attribute_maybe_unused__) {
        CBuild::print("Registered generators:");
        for (auto gen : CBuild::Registry::GetGeneratorsList()) {
            CBuild::print(std::string("\t") + gen);
        }
    }
};
class CmdList : public CBuild::Task {
  public:
    CmdList() : CBuild::Task("CBuild_cmdlist", {}) {}
    void call(std::vector<std::string> __attribute_maybe_unused__) {
        CBuild::print("Registered user commands:");
        for (auto cmd : CBuild::Registry::GetKeywordsList().keys()) {
            CBuild::print(std::string("\t") + cmd);
        }
    }
};
} // namespace CBuild
/* namesapce Registry */
namespace CBuild {
namespace Registry {
// System tasks
CBuild::Help          help;
CBuild::Init          initw;
CBuild::Version       ver;
CBuild::TaskList      tasklist;
CBuild::ToolchainList toolchainlist;
CBuild::GeneratorList generatorlist;
CBuild::CmdList       cmdlist;
// System generators
CBuild::ccj_out      ccjo;
CBuild::makefile_out makefileo;
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
// Output generators
lib::map<std::string, CBuild::generator_base *> generators;
} // namespace Registry
} // namespace CBuild
/* register.hpp */
void CBuild::Registry::init() {
    // Register help task
    Registry::RegisterKeyword("--help", &CBuild::Registry::help);
    // Register init task
    Registry::RegisterKeyword("--init", &CBuild::Registry::initw);
    // Register version task
    Registry::RegisterKeyword("--version", &CBuild::Registry::ver);
    // Task list task
    Registry::RegisterKeyword("--task-list", &CBuild::Registry::tasklist);
    // Target list task
    Registry::RegisterKeyword("--target-list",
                              &CBuild::Registry::toolchainlist);
    // User commands list
    Registry::RegisterKeyword("--commands-list", &CBuild::Registry::cmdlist);
    // Generator list task
    Registry::RegisterKeyword("--generator-list",
                              &CBuild::Registry::generatorlist);
    // Register system generators
    Registry::RegisterGenerator(&CBuild::Registry::makefileo, "make");
    Registry::RegisterGenerator(&CBuild::Registry::ccjo, "ccj");
}
void CBuild::Registry::RegisterTask(CBuild::Task *task) {
    // Save task ptr connected to it's name
    try {
        Registry::tasks.push_back_check(task->self_name(), task);
    } catch (std::exception &e) {
        CBuild::print_full("Error: trying to register task with the same id: " +
                               task->self_name(),
                           CBuild::RED);
        return;
    }
    // Save task excution state connected to it's name
    Registry::task_executed.push_back(task->self_name(), false);
}
void CBuild::Registry::CallTask(std::string              name,
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
        CBuild::print("Task " + name + " not found. Exiting ...", CBuild::RED);
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
        return;
    }
    // Save toolchain excution state connected to it's name
    Registry::target_executed.push_back(target->get_id(), false);
}
CBuild::Toolchain *CBuild::Registry::GetToolchain(std::string name,
                                                  bool        force) {
    // If we not in force build
    if (!force) {
        // Check if toolchain was executed before
        auto check = Registry::target_executed.get_ptr(name);
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
    Registry::RegisterTask(func);
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
void CBuild::Registry::ToolchainAll(bool force, std::string path,
                                    std::vector<std::string> *args) {
    for (unsigned int i = 0; i < Registry::targets.size(); i++) {
        auto elem  = Registry::targets.at(i);
        auto check = Registry::target_executed.get_ptr(elem.key);
        if (check != NULL && check->data == false) {
            check->data = true;
            auto ptr    = elem.data;
            if (ptr != NULL) {
                ptr->load_project_deps(path);
                ptr->call(args, force);
            }
        }
    }
    // auto check = Registry::target_executed.get_ptr(name);
    // if (check != NULL && check->data == false) {
    //   // Return toolchain pointer if not
    //   auto elem = Registry::targets.get_ptr(name);
    //   // Mark toolcian as executed
    //   check->data = true;
    //   return elem->data;
    // }
}
CBuild::generator_base *CBuild::Registry::GetGenerator(std::string id) {
    auto elem = Registry::generators.get_ptr(id);
    if (elem != NULL) {
        return elem->data;
    }
    return NULL;
}
void CBuild::Registry::RegisterGenerator(CBuild::generator_base *gen,
                                         std::string             id) {
    try {
        Registry::generators.push_back_check(id, gen);
    } catch (std::exception &e) {
        CBuild::print_full(
            "Error: trying to register generator with simmilar id: " + id, RED);
    }
}
std::vector<std::string> CBuild::Registry::GetGeneratorsList() {
    std::vector<std::string> ret;
    for (auto gen : CBuild::Registry::generators) {
        ret.push_back(gen.key);
    }
    return ret;
}
std::vector<std::string> CBuild::Registry::GetToolchainsList() {
    std::vector<std::string> ret;
    for (auto tg : CBuild::Registry::targets) {
        ret.push_back(tg.key);
    }
    return ret;
}
std::vector<std::string> CBuild::Registry::GetTasksList() {
    std::vector<std::string> ret;
    for (auto ts : CBuild::Registry::tasks) {
        ret.push_back(ts.key);
    }
    return ret;
}
void CBuild::Registry::SetVersionHandler(void (*handler)()) {
    CBuild::Registry::handler = handler;
}
