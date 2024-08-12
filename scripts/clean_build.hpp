/**
 * @file clean_build.hpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Clean build using gcc under docker
 * @date 2024-05-05
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
#include "../CBuild/headers/CBuild_defs.hpp"
#include "../CBuild/headers/build/g++.hpp"
#include "../CBuild/headers/filesystem++.hpp"
// Code
#ifndef __CLEAN_BUILD_HPP__
#define __CLEAN_BUILD_HPP__
class clean_gxx : public CBuild::GXX<CBuild::CBuildHashV2> {
  public:
    clean_gxx(std::string id) : CBuild::GXX<>(id) {
        this->compiler =
            "sudo docker run --rm -v \"$PWD\":/usr/src/myapp -w /usr/src/myapp gcc:10.5.0 g++";
        this->linker =
            "sudo docker run --rm -v \"$PWD\":/usr/src/myapp -w /usr/src/myapp gcc:10.5.0 g++";
    }
    clean_gxx(std::string id, std::string name) : CBuild::GXX<>(id, name) {
        this->compiler =
            "sudo docker run --rm -v \"$PWD\":/usr/src/myapp -w /usr/src/myapp gcc:10.5.0 g++";
        this->linker =
            "sudo docker run --rm -v \"$PWD\":/usr/src/myapp -w /usr/src/myapp gcc:10.5.0 g++";
    }
    void post_link() override {
        CBuild::fs::remove("packages/libCBuild.so", true);
        CBuild::fs::copy(CBUILD_BUILD_DIR + "/" + this->id + "/" + CBUILD_BUILD_OUT_DIR + "/" +
                             "libCBuild.so",
                         "packages/libCBuild.so");
    }
};
#endif // __CLEAN_BUILD_HPP__
