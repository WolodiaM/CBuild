/**
 * @file cbuild_dep.hpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief CBuild project dependency
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
#include "vector"
// Project includes
#include "../CBuild_defs.hpp"
#include "../build_data.hpp"
#include "../filesystem++.hpp"
#include "../print.hpp"
#include "../system.hpp"
#include "dependency.hpp"
// Code
#ifndef __CBUILD_DEP_HPP__
#define __CBUILD_DEP_HPP__
namespace CBuild {
class CBuildDependency : public CBuild::Dependency {
  protected:
    std::string path;
    std::string target_id;
    std::string headers_dir;

  public:
    CBuildDependency(std::string path, std::string target_id, std::string headers_dir) {
        this->path = path;
        this->target_id = target_id;
        this->headers_dir = headers_dir;
    }
    bool need_prepare() override {
        CBuild::target_metadata_file meta;
        CBuild::read_target_metadata_direct(this->path + "/" + CBUILD_BUILD_DIR + "/" +
                                                this->target_id + "/" + this->target_id +
                                                CBUILD_METADATA_FILE_EXTENSION,
                                            &meta);
        return !CBuild::fs::exists(CBUILD_CACHE_DIR + "/" + CBUILD_PROJECT_DEPS_DIR + "/" +
                                   meta.out.substr(meta.out.find_last_of("/") + 1));
    }
    std::string cargs() override {
        return ("-I" + CBUILD_CACHE_DIR + "/" + CBUILD_PROJECT_DEPS_HEADERS + "/extproject_" +
                this->target_id);
    }
    std::string largs() override {
        CBuild::target_metadata_file meta;
        CBuild::read_target_metadata_direct(this->path + "/" + CBUILD_BUILD_DIR + "/" +
                                                this->target_id + "/" + this->target_id +
                                                CBUILD_METADATA_FILE_EXTENSION,
                                            &meta);
        return ("-L" + CBUILD_CACHE_DIR + "/" + CBUILD_PROJECT_DEPS_DIR + " -l" +
                meta.out.substr(meta.out.find_last_of("/") + 4,
                                (meta.out.length() - (meta.out.find_last_of("/") + 4) -
                                 (meta.out.length() - meta.out.find_last_of(".")))));
    }
    void prepare() override {
        CBuild::print("Running prepare() for dependency \"" + this->target_id + "\"", CBuild::RED);
        CBuild::target_metadata_file meta;
        CBuild::read_target_metadata_direct(this->path + "/" + CBUILD_BUILD_DIR + "/" +
                                                this->target_id + "/" + this->target_id +
                                                CBUILD_METADATA_FILE_EXTENSION,
                                            &meta);
        std::string ccmd = meta.compiler + " ";
        for (auto carg : meta.cargs) {
            ccmd += carg;
            ccmd += " ";
        }
        std::string lcmd = meta.linker + " ";
        for (auto larg : meta.largs) {
            lcmd += larg;
            lcmd += " ";
        }
        CBuild::print_full("Search metadata path: \"" + this->path + "/" + CBUILD_BUILD_DIR + "/" +
                               this->target_id + "/" + CBUILD_METADATA_FOLDER + "\"",
                           CBuild::MAGENTA);
        std::vector files = CBuild::fs::dir(this->path + "/" + CBUILD_BUILD_DIR + "/" +
                                                this->target_id + "/" + CBUILD_METADATA_FOLDER,
                                            ".*\\.meta");
        std::string object_list = "";
        CBuild::fs::create({CBUILD_BUILD_DIR + "/extproject_" + this->target_id}, CBuild::fs::DIR);
        CBuild::fs::create(
            {CBUILD_BUILD_DIR + "/extproject_" + this->target_id + "/" + CBUILD_BUILD_OUT_DIR},
            CBuild::fs::DIR);
        CBuild::fs::create(
            {CBUILD_BUILD_DIR + "/extproject_" + this->target_id + "/" + CBUILD_BUILD_CACHE_DIR},
            CBuild::fs::DIR);
        for (auto file : files) {
            CBuild::source_metadata_file smeta;
            CBuild::read_file_metadata_direct(this->target_id, file, &smeta);
            if (smeta.object != std::string("header")) {
                CBuild::print_full("Compiling file \"" + file + "\"", CBuild::GREEN);
                object_list = object_list + " " + CBUILD_BUILD_DIR + "/extproject_" +
                              this->target_id + "/" + CBUILD_BUILD_CACHE_DIR + "/" +
                              smeta.object.substr(smeta.object.find_last_of("/") + 1);
                this->compile(ccmd + " -c " + this->path + "/" + smeta.source + " -o " +
                              CBUILD_BUILD_DIR + "/extproject_" + this->target_id + "/" +
                              CBUILD_BUILD_CACHE_DIR + "/" +
                              smeta.object.substr(smeta.object.find_last_of("/") + 1));
            }
        }
        this->compile(lcmd + object_list + " -o " + CBUILD_BUILD_DIR + "/extproject_" +
                      this->target_id + "/" + CBUILD_BUILD_OUT_DIR + "/" +
                      meta.out.substr(meta.out.find_last_of("/")));
        CBuild::fs::copy(CBUILD_BUILD_DIR + "/extproject_" + this->target_id + "/" +
                             CBUILD_BUILD_OUT_DIR + "/" +
                             meta.out.substr(meta.out.find_last_of("/") + 1),
                         CBUILD_CACHE_DIR + "/" + CBUILD_PROJECT_DEPS_DIR + "/" +
                             meta.out.substr(meta.out.find_last_of("/") + 1));
        CBuild::fs::create({CBUILD_CACHE_DIR + "/" + CBUILD_PROJECT_DEPS_HEADERS + "/" +
                            this->headers_dir.substr(this->headers_dir.find_last_of("/") + 1)},
                           CBuild::fs::DIR);
        CBuild::fs::copy(this->headers_dir, CBUILD_CACHE_DIR + "/" + CBUILD_PROJECT_DEPS_HEADERS +
                                                "/extproject_" + this->target_id);
    }
    void compile(std::string str) {
        int ret = CBuild::system(str);
        if (ret != 0) {
            CBuild::print("Error in compilation! Stopping compilation...", CBuild::RED);
            exit(0xFF);
        }
    }
};
} // namespace CBuild
#endif // __CBUILD_DEP_HPP__
