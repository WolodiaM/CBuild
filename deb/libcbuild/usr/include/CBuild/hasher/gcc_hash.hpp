/**
 * @file gcc_hash.hpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Metadata preprocessor based on gcc -MD subcommand
 * @date 2024-01-06
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
#include "../map.hpp"
#include "string"
#include "vector"
// Project includes
#include "../build_data.hpp"
#include "gcc_hash_impl.hpp"
#include "hasher.hpp"
// Code
#ifndef __GCC_HASH_HPP__
#define __GCC_HASH_HPP__
namespace CBuild {
class GCCHash : public CBuild::Hash {
  protected:
  public:
    GCCHash(std::string target_id) : Hash(target_id) {}
    virtual lib::map<std::string, std::string>
    get_files_for_recompilation(std::vector<std::string> file_list,
                                std::vector<std::string> objects_list) override {
        if (file_list.size() != objects_list.size()) {
            return {};
        }
        return CBuild::gcc_hash_impl(file_list, objects_list, this->target_id);
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
#endif // __GCC_HASH_HPP__
