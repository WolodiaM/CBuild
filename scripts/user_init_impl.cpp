/**
 * @file user_init.cpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Main implementation file for userspace scripts
 * @version 1.0
 * @date <date>
 *
 *
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
// C++ libraries
#include "fstream"
#include "iostream"
#include "stdio.h"
// Userspace headers
#include "clean_build.hpp"
#include "user_init.hpp"
// CBuild headers
#include "../CBuild/headers/build/g++mt.hpp"
#include "../CBuild/headers/print.hpp"
#include "../CBuild/headers/register.hpp"
#include "../CBuild/headers/task/Task.hpp" // Custom Tasks
// Code
class copyLib : public CBuild::Task {
  public:
    copyLib(std::string name) : CBuild::Task(name, {}) {}
    void call(std::vector<std::string> args __attribute_maybe_unused__) {
        CBuild::system("cp -r build/cbuild_clean/out/* CBuild/CBuild/");
    }
};
CBuild::GXXMT<> libCBuild("cbuild", "CBuild");
clean_gxx cbuild_clean("cbuild_clean", "CBuild");
copyLib cpy("copyLib");

void ver() {
    CBuild::print("CBuild buildscript version 3.0");
}

void init() {
    libCBuild.set_standart("c++20");
    libCBuild.add_folder("CBuild/CBuild/src/");
    libCBuild.warn();
    libCBuild.set_type(CBuild::DYNAMIC_LIBRARY);
    libCBuild.add_link_arg("-pthread");
    libCBuild.add_compile_arg("-pthread");
    libCBuild.add_requirment("proccessVersion", CBuild::PRE);
    libCBuild.add_requirment("proccessHelp", CBuild::PRE);
    cbuild_clean.set_standart("c++20");
    cbuild_clean.add_folder("CBuild/CBuild/src/");
    cbuild_clean.warn();
    cbuild_clean.set_type(CBuild::DYNAMIC_LIBRARY);
    cbuild_clean.add_link_arg("-pthread");
    cbuild_clean.add_compile_arg("-pthread");
    cbuild_clean.add_requirment("proccessVersion", CBuild::PRE);
    cbuild_clean.add_requirment("proccessHelp", CBuild::PRE);
    // libCBuild.add_compile_arg("-g");
    // libCBuild.add_link_arg("-g");
    // Version
    std::ifstream version("./packages/ppa/ubuntu/version");
    char str[10];
    version.getline(str, 10);
    version.close();
    std::string vstr = std::string(str).substr(0, std::string(str).find('v'));
    libCBuild.set_version_major(std::stoi(vstr.substr(0, vstr.find('.'))));
    CBuild::Registry::RegisterTarget(&libCBuild);
    CBuild::Registry::RegisterTarget(&cbuild_clean);
    CBuild::Registry::RegisterTask(&cpy);
    CBuild::Registry::RegisterKeyword("-cp", &cpy);
    load_tasks();
    // Version handler
    CBuild::Registry::SetVersionHandler(&ver);
}
