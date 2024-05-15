/**
 * @file net_dep_helper.hpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Few helper functions for external net dependencies
 * @date 2024-05-12
 *
 * @copyright (C) 2024 WolodiaM
 * @license GPL v3.0 or later
 *
 * Copyright (C) 2024 WolodiaM
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
// Project includes
#include "../CBuild_defs.hpp"
#include "../filesystem++.hpp"
#include "../print.hpp"
#include "../system.hpp"
#include "dependency.hpp"
// Code
#ifndef __NET_DEP_HELPER_HPP__
#define __NET_DEP_HELPER_HPP__
namespace CBuild {
class NetDependency : public CBuild::Dependency {
  protected:
    void git_fetch(std::string server, std::string repo) {
        std::string repo_path = repo;
        while (repo_path.find("/") != std::string::npos) {
            repo_path.replace(repo_path.find("/"), std::string("/").size(), "_");
        }
        CBuild::fs::remove(CBUILD_CACHE_DIR + "/" + repo_path, true);
        CBuild::fs::create({CBUILD_CACHE_DIR + "/" + repo_path}, CBuild::fs::DIR);
        CBuild::system("cd " + CBUILD_CACHE_DIR + "/" + repo_path +
                       " && git clone --depth 1 --recursive " + server + "/" + repo + " .");
    }
    void git_fetch_branch(std::string server, std::string repo, std::string branch) {
        std::string repo_path = repo;
        while (repo_path.find("/") != std::string::npos) {
            repo_path.replace(repo_path.find("/"), std::string("/").size(), "_");
        }
        CBuild::fs::remove(CBUILD_CACHE_DIR + "/" + repo_path, true);
        CBuild::fs::create({CBUILD_CACHE_DIR + "/" + repo_path}, CBuild::fs::DIR);
        CBuild::system("cd " + CBUILD_CACHE_DIR + "/" + repo_path +
                       " && git clone --depth 1 --recursive --branch " + branch + " " + server +
                       "/" + repo + " .");
    }
    void file_fetch(std::string url, std::string file, std::string args = "") {
        CBuild::system("curl " + args + " -o " + file + " " + url);
    }
};
} // namespace CBuild
#endif // __NET_DEP_HELPER_HPP__
