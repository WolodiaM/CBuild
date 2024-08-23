/**
 * @file simple_toolchain.hpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Simple retargetable toolchain
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
#include "../hasher/cbuild_hash.hpp"
#include "Build.hpp"
// Code
#ifndef __SIMPLE_TOOLCHAIN_HPP__
#define __SIMPLE_TOOLCHAIN_HPP__
namespace CBuild {
/**
 * @brief Very simple generic toolchain, rely on compiler to support '-c' flat to specify files to
 * compile and '-o' flag to specify output object file, linker need to support '-o' flag to specify
 * output binary. This should work with all gcc and clang variants.
 */
template <CBuild::HashImpl hash = CBuild::CBuildHashV2>
class SimpleToolchain : public CBuild::Toolchain {
  public:
    /**
     * @brief Construct a new GXX object
     *
     * @param id Id
     */
    SimpleToolchain(std::string id) {
        this->id = id;
        this->name = "";
        this->compiler = "";
        this->linker = "";
        this->packer = "";
        this->debuuger = "";
        this->hasher = new hash(this->id);
    }
    /**
     * @brief Construct a new GXX object
     *
     * @param id Id
     * @param name Name
     */
    SimpleToolchain(std::string id, std::string name) {
        // Set id and name of toolchain and assign executables constants
        this->id = id;
        this->name = name;
        this->compiler = "";
        this->linker = "";
        this->packer = "";
        this->debuuger = "";
        this->hasher = new hash(this->id);
    }
    /**
     * @brief Set used compiler
     *
     * @param compiler => std::string -> Compiler shell command (you could specify cli arguments
     * here if needed, eg. to select specific architecture)
     */
    void set_compiler(std::string compiler) {
        this->compiler = compiler;
    }
    /**
     * @brief Set used linker
     *
     * @param linker => std::string -> Linker shell command (you could specify cli arguments
     * here if needed, eg. to select specific architecture)
     */
    void set_linker(std::string linker) {
        this->linker = linker;
    }
    /**
     * @brief Set used packer
     *
     * @param packer => std::string -> Packer (does static lib packing) shell command (you could
     * specify cli arguments here if needed, eg. to select specific architecture)
     */
    void set_packer(std::string packer) {
        this->packer = packer;
    }
    /**
     * @brief Set used debugger
     *
     * @param debugger => std::string -> Debugger shell command (you could specify cli arguments
     * here if needed, eg. to select specific architecture)
     */
    void set_debugger(std::string debugger) {
        this->debugger = debugger;
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
#endif // __SIMPLE_TOOLCHAIN_HPP__
