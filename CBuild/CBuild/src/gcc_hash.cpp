/**
 * @file gcc_hash.cpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief GCC hash impementation's implemetation
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
#include "../../headers/map.hpp"
#include "fstream"
#include "sstream"
#include "string"
#include "vector"
// Project includes
#include "../../headers/build_data.hpp"
#include "../../headers/filesystem++.hpp"
#include "../../headers/hasher/gcc_hash_impl.hpp"
#include "../../headers/print.hpp"
#include "../../headers/register.hpp"
#include "../../headers/system.hpp"
/* Helper code */
struct extended_metadata {
    CBuild::source_metadata_file meta;
    CBuild::source_metadata_file old_meta;
    bool old_metadata_available;
};
bool check_mismatch_recursive(const extended_metadata* file,
                              std::vector<extended_metadata>* source_metadata,
                              lib::map<std::string, extended_metadata>* header_metadata) {
    if (file->old_metadata_available == false) {
        // printf("File: '%s' -> no_old_meta\n", file->meta.source.c_str());
        return true;
    }
    if (file->meta.object != file->old_meta.object) {
        // printf("File: '%s' -> mismatch object\n", file->meta.source.c_str());
        return true;
    }
    if (file->meta.hash != file->old_meta.hash) {
        // printf("File: '%s' -> mismatch hash\n", file->meta.source.c_str());
        return true;
    }
    if (file->meta.deps != file->old_meta.deps) {
        // printf("File: '%s' -> mismatch deps\n", file->meta.source.c_str());
        return true;
    }
    for (auto dep : file->meta.deps) {
        auto header = header_metadata->get(dep);
        if (header != NULL && check_mismatch_recursive(header, source_metadata, header_metadata)) {
            return true;
        }
        for (auto s : *source_metadata) {
            if (dep == s.meta.source) {
                if (check_mismatch_recursive(&s, source_metadata, header_metadata)) {
                    return true;
                }
            }
        }
    }
    return false;
}
/* gcc_hash.hpp */
lib::map<std::string, std::string> CBuild::gcc_hash_impl(std::vector<std::string> file_list,
                                                         std::vector<std::string> objects_list,
                                                         std::string target_id) {
    std::vector<extended_metadata> metadatas;
    lib::map<std::string, extended_metadata> header_metadatas;
    lib::map<std::string, std::string> ret; // source->object
    CBuild::print_toggle();
    // Get some basic metadata structs
    for (size_t i = 0; i < file_list.size(); i++) {
        metadatas.push_back({.meta = {.source = file_list.at(i),
                                      .object = objects_list.at(i),
                                      .deps = {},
                                      .hash = 0},
                             .old_meta = {},
                             .old_metadata_available = false});
    }
    // Get metadata from files
    for (size_t i = 0; i < metadatas.size(); i++) {
        auto meta = &(metadatas.at(i));
        int err = CBuild::read_file_metadata(target_id, meta->meta.source, &(meta->old_meta));
        if (err == 0) {
            meta->old_metadata_available = true;
        }
    }
    // Get toolchain data
    auto toolchain = CBuild::Registry::GetTarget(target_id, true);
    if (toolchain != NULL) {
        auto cc = toolchain->get_cmds()[0];
        toolchain->stdargs();
        auto cargs = toolchain->get_compile_args();
        for (size_t i = 0; i < metadatas.size(); i++) {
            auto meta = &metadatas.at(i);
            // Get dependencies
            auto file = meta->meta.source;
            auto cmd = cc + " " + cargs + " " + file + " -MM";
            auto ret = CBuild::system_piped(cmd, 64 * 1024);
            std::stringstream ss(ret);
            std::string tmp;
            ss >> tmp;
            ss >> tmp;
            std::vector<std::string> deps;
            while (std::getline(ss, tmp, ' ')) {
                if (tmp != std::string("") && tmp != std::string("\\\n") &&
                    tmp != std::string("\n")) {
                    deps.push_back(tmp);
                }
            }
            lib::map<std::string, bool> deps_selector;
            for (auto dep : deps) {
                try {
                    deps_selector.push_back_check(CBuild::fs::normalize_relative_path(dep), true);
                } catch (std::exception& e) {
                }
            }
            for (auto elem : deps_selector) {
                meta->meta.deps.push_back(elem.key);
            }
            if (!(meta->meta.deps.empty())) {
                meta->meta.deps.back().pop_back();
            }
            // Get file hash
            std::ifstream f(file);
            std::string content;
            std::getline(f, content, '\0');
            meta->meta.hash = CBuild::hash_fnv_1a(content);
        }
        // Reset print to normal
        CBuild::print_toggle();
        // Some fancy debug prints and metadat file updates
        for (auto meta : metadatas) {
            CBuild::write_file_metadata(target_id, meta.meta.source, &(meta.meta));
            CBuild::print_full(std::string("File: ") + meta.meta.source +
                               std::string("\nObject: ") + meta.meta.object +
                               std::string("\nDeps [new]:"));
            for (auto dep : meta.meta.deps) {
                CBuild::print_full(dep);
                // Abuse printing code to save headers for processing
                try {
                    header_metadatas.push_back_check(dep, {});
                } catch (std::exception& e) {
                }
            }
            CBuild::print_full("Deps [old]:");
            if (meta.old_metadata_available) {
                for (auto dep : meta.old_meta.deps) {
                    CBuild::print_full(dep);
                }
            } else {
                CBuild::print_full(" null");
            }
            CBuild::print_full(std::string("Hash [new]: ") + std::to_string(meta.meta.hash) +
                               std::string(", Hash [old]: ") +
                               (meta.old_metadata_available ? std::to_string(meta.old_meta.hash)
                                                            : std::string("null")));
        }
        // Disable printing, again
        CBuild::print_toggle();
        // Get metadata for headers
        for (size_t i = 0; i < header_metadatas.size(); i++) {
            auto meta = &(header_metadatas.ptr_at(i)->data);
            // Get dependencies
            auto file = header_metadatas.at(i).key;
            auto cmd = cc + " " + cargs + " " + file + " -MM";
            auto ret = CBuild::system_piped(cmd, 64 * 1024);
            std::stringstream ss(ret);
            std::string tmp;
            ss >> tmp;
            ss >> tmp;
            std::vector<std::string> deps;
            while (std::getline(ss, tmp, ' ')) {
                if (tmp != std::string("") && tmp != std::string("\\\n") &&
                    tmp != std::string("\n")) {
                    deps.push_back(tmp);
                }
            }
            lib::map<std::string, bool> deps_selector;
            for (auto dep : deps) {
                try {
                    deps_selector.push_back_check(CBuild::fs::normalize_relative_path(dep), true);
                } catch (std::exception& e) {
                }
            }
            for (auto elem : deps_selector) {
                meta->meta.deps.push_back(elem.key);
            }
            if (!(meta->meta.deps.empty())) {
                meta->meta.deps.back().pop_back();
            }
            for (auto dep : meta->meta.deps) {
                try {
                    header_metadatas.push_back_check(dep, {});
                } catch (std::exception& e) {
                }
            }
            // Get file hash
            std::ifstream f(file);
            std::string content;
            std::getline(f, content, '\0');
            meta->meta.hash = CBuild::hash_fnv_1a(content);
            // Get old metadata
            int err = CBuild::read_file_metadata(target_id, file, &(meta->old_meta));
            meta->old_metadata_available = (err == 0);
            // Files
            meta->meta.source = file;
            meta->meta.object = "header";
        }
        // Reset print to normal, again
        CBuild::print_toggle();
        // Some fancy debug prints and metadat file updates
        for (auto meta_container : header_metadatas) {
            auto meta = meta_container.data;
            CBuild::write_file_metadata(target_id, meta.meta.source, &(meta.meta));
            CBuild::print_full(std::string("File: ") + meta.meta.source +
                               std::string("\nObject: ") + meta.meta.object +
                               std::string("\nDeps [new]:"));
            for (auto dep : meta.meta.deps) {
                CBuild::print_full(dep);
            }
            CBuild::print_full("Deps [old]:");
            if (meta.old_metadata_available) {
                for (auto dep : meta.old_meta.deps) {
                    CBuild::print_full(dep);
                }
            } else {
                CBuild::print_full(" null");
            }
            CBuild::print_full(std::string("Hash [new]: ") + std::to_string(meta.meta.hash) +
                               std::string(", Hash [old]: ") +
                               (meta.old_metadata_available ? std::to_string(meta.old_meta.hash)
                                                            : std::string("null")));
        }
    }
    // Get files for recompilation
    for (auto file : metadatas) {
        if (check_mismatch_recursive(&file, &metadatas, &header_metadatas)) {
            ret.push_back(file.meta.source, file.meta.object);
        }
    }
    // Generate dependencie
    return ret;
}
