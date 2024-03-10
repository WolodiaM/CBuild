/**
 * @file manual_hash.hpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @date 2023-12-15
 * @brief Old manual hasher
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
#ifndef __MANUAL_HASH_HPP__
#define __MANUAL_HASH_HPP__
// C++ libs
#include "string"
#include "vector"
// Project headers
#include "../build_data.hpp"
#include "../hash.hpp"
#include "../map.hpp"
#include "hasher.hpp"
// Code
namespace CBuild {
class CBuildHash : public CBuild::Hash {
  protected:
    std::string gen_out_file(std::string file) {
        // path to base diretory for out files
        std::string base =
            CBUILD_BUILD_DIR + "/" + this->target_id + "/" + CBUILD_BUILD_CACHE_DIR + "/";
        // Find dot in file name
        unsigned long pos = file.find_last_of(".");
        // Cut it out
        file = file.substr(0, pos);
        // Replace all slashes by dots
        while (file.find("/") != std::string::npos) {
            file.replace(file.find("/"), std::string("/").size(), ".");
        }
        // Add .o extension
        file += ".o";
        // Return full path
        base += file;
        return base;
    }

  public:
    CBuildHash(std::string target_id) : Hash(target_id) {}
    virtual lib::map<std::string, std::string>
    get_files_for_recompilation(std::vector<std::string> file_list,
                                std::vector<std::string> objects_list) override {
        if (file_list.size() != objects_list.size()) {
            return {};
        }
        auto files = CBuild::get_files(file_list, objects_list, this->target_id);
        lib::map<std::string, std::string> ret;
        for (size_t i = 0; i < files.size(); i++) {
            ret.push_back(files.at(i), this->gen_out_file(files.at(i)));
        }
        return ret;
    }
    virtual int compare_and_set_cargs(std::vector<std::string> cargs) override {
        CBuild::target_metadata_file meta;
        if (CBuild::read_target_metadata(this->target_id, &meta) != 0) {
            return -1;
        }
        if (cargs != meta.cargs) {
            meta.cargs = cargs;
            CBuild::write_target_metadata(this->target_id, &meta);
            return 1;
        }
        return 0;
    }
    virtual int compare_and_set_largs(std::vector<std::string> largs) override {
        CBuild::target_metadata_file meta;
        if (CBuild::read_target_metadata(this->target_id, &meta) != 0) {
            return -1;
        }
        if (largs != meta.largs) {
            meta.largs = largs;
            CBuild::write_target_metadata(this->target_id, &meta);
            return 1;
        }
        return 0;
    }
    virtual int compare_and_set_commands(std::string compiler, std::string linker,
                                         std::string packer) override {
        CBuild::target_metadata_file meta;
        if (CBuild::read_target_metadata(this->target_id, &meta) != 0) {
            return -1;
        }
        if (compiler != meta.compiler || linker != meta.linker || packer != meta.packer) {
            meta.compiler = compiler;
            meta.linker = linker;
            meta.packer = packer;
            CBuild::write_target_metadata(this->target_id, &meta);
            return 1;
        }
        return 0;
    }
    virtual int compare_and_set_output_file(std::string out) override {
        CBuild::target_metadata_file meta;
        if (CBuild::read_target_metadata(this->target_id, &meta) != 0) {
            return -1;
        }
        if (out != meta.out) {
            meta.out = out;
            CBuild::write_target_metadata(this->target_id, &meta);
            return 1;
        }
        return 0;
    }
    virtual void set_target_meta(std::vector<std::string> cargs, std::vector<std::string> largs,
                                 std::string out, std::string compiler, std::string linker,
                                 std::string packer) override {
        CBuild::target_metadata_file meta;
        meta.cargs = cargs;
        meta.largs = largs;
        meta.out = out;
        meta.compiler = compiler;
        meta.linker = linker;
        meta.packer = packer;
        CBuild::write_target_metadata(this->target_id, &meta);
    }
};
} // namespace CBuild
#endif // __MANUAL_HASH_HPP__
