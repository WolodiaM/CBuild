/**
 * @file g++.hpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief g++ toolchain implementation
 * @date 2023-02-03
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
#ifndef CBUILD_GXX_TOOLCHAIN
#define CBUILD_GXX_TOOLCHAIN
// Project files
#include "../CBuild_defs.hpp"
#include "../hasher/cbuild_hash.hpp"
#include "Build.hpp"
// Code
namespace CBuild {
template <CBuild::HashImpl hash = CBuild::CBuildHashV2> class GXX : public CBuild::Toolchain {
  public:
    /**
     * @brief Construct a new GXX object
     *
     * @param id Id
     */
    GXX(std::string id) {
        // Set id of toolchain and assign executables constants
        this->id = id;
        this->name = "";
        this->linker = "g++";
        this->compiler = "g++";
        this->packer = "ar cr";
        this->add_link_arg("-Wl,-z,origin");
        this->add_link_arg(" -Wl,-rpath,\"\\$ORIGIN\"");
        this->add_link_arg("-lstdc++");
        this->hasher = new hash(this->id);
    }
    /**
     * @brief Construct a new GXX object
     *
     * @param id Id
     * @param name Name
     */
    GXX(std::string id, std::string name) {
        // Set id and name of toolchain and assign executables constants
        this->id = id;
        this->name = name;
        this->linker = "g++";
        this->compiler = "g++";
        this->packer = "ar cr";
        this->add_link_arg("-Wl,-z,origin");
        this->add_link_arg(" -Wl,-rpath,\"\\$ORIGIN\"");
        this->add_link_arg("-lstdc++");
        this->hasher = new hash(this->id);
    }

  protected:
    void build() override {
        // Get all args
        std::string args;
        for (auto elem : this->compiler_args) {
            args += elem;
            args += " ";
        }
        // Get all files
        auto files = this->gen_file_list(this->force);
        if (files.size() > 0) {
            // Compile file by file
            for (unsigned int i = 0; i < files.size(); i++) {
                // Construct command
                std::string cmd = this->compiler + " -c ";
                cmd += files.at(i).key;
                cmd += " ";
                cmd += args;
                cmd += " -o ";
                cmd += files.at(i).data;
                // Execute command
                this->compile(cmd);
            }
        }
    }
    void link() override {
        // Get args
        std::string args;
        for (auto elem : this->link_args) {
            args += elem;
            args += " ";
        }
        // Get files
        this->gen_file_list_for_linking = true;
        auto files = this->gen_file_list(true);
        this->gen_file_list_for_linking = false;
        std::string flist;
        for (unsigned int i = 0; i < files.size(); i++) {
            flist += files.at(i).data;
            flist += " ";
        }
        if (files.size() > 0) {
            // Construct command
            std::string cmd = this->linker + " ";
            cmd += flist;
            cmd += " ";
            cmd += args;
            cmd += " ";
            cmd += " -o ";
            cmd += this->gen_out_name();
            // Call command
            // CBuild::print(cmd.c_str(), CBuild::color::BLUE);
            this->compile(cmd);
        }
    }
    void link_pack() override {
        // Get args
        std::string args;
        for (auto elem : this->link_args) {
            args += elem;
            args += " ";
        }
        // Get all files
        this->gen_file_list_for_linking = true;
        auto files = this->gen_file_list(true);
        this->gen_file_list_for_linking = false;
        std::string flist;
        for (unsigned int i = 0; i < files.size(); i++) {
            flist += files.at(i).data;
            flist += " ";
        }
        if (files.size() > 0) {
            // Construct command
            std::string cmd = this->packer + " ";
            cmd += this->gen_out_name();
            cmd += " ";
            cmd += flist;
            cmd += " ";
            // Call command
            // CBuild::print(cmd.c_str(), CBuild::color::BLUE);
            this->compile(cmd);
        }
    }
};
} // namespace CBuild
#endif // _CBUILD_GXX_TOOLCHAIN
