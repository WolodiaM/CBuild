/**
 * @file build_hash.cpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Implementation of CBuildHashV2 hashing algorithm
 * @date 2024-01-08
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
#include "inttypes.h"
#include "regex"
#include "string"
#include "sys/resource.h"
#include "vector"
// Project includes
#include "../../headers/build_data.hpp"
#include "../../headers/filesystem++.hpp"
#include "../../headers/hasher/cbuild_hash.hpp"
#include "../../headers/print.hpp"
/* Some helper code */
struct extended_metadata {
    CBuild::source_metadata_file new_meta;
    CBuild::source_metadata_file old_meta;
    bool old_meta_available;
};
struct internal_data {
    lib::map<std::string, extended_metadata> sources_metadata;
    lib::map<std::string, extended_metadata> headers_metadata;
    lib::map<std::string, bool> headers_to_parse;
};
std::regex cpp_comment("[^\'\"]\\/\\/.*\n",
                       std::regex_constants::optimize | std::regex_constants::ECMAScript);
std::regex c_comment("\\/\\*([\\s\\S]*?)\\*\\/",
                     std::regex_constants::optimize | std::regex_constants::ECMAScript);
std::regex include_detector("#include [\"<]([^\">]+)[\">]",
                            std::regex_constants::optimize | std::regex_constants::ECMAScript);
void parse_file(internal_data* data, std::string target_id, std::string file, std::string object) {
    CBuild::print_full(std::string("Start, file: ") + file);
    extended_metadata m;
    // Get old metadata
    int err = CBuild::read_file_metadata(target_id, file, &(m.old_meta));
    m.old_meta_available = (err == 0);
    CBuild::print_full(std::string("Metadata read"));
    // Get new metadata: file and object
    m.new_meta.source = file;
    m.new_meta.object = object;
    // Read file content
    std::ifstream f(file);
    std::string file_content;
    std::getline(f, file_content, '\0');
    // Strip comments from file
    file_content = std::regex_replace(file_content, cpp_comment, "");
    file_content = std::regex_replace(file_content, c_comment, "");
    CBuild::print_full(std::string("Comments removed"));
    // Get new metadata: hash
    m.new_meta.hash = CBuild::hash_fnv_1a(file_content);
    // Get new metadata: get dependencies and add new headers for parsing
    m.new_meta.deps = {};
    std::smatch match;
    std::string::const_iterator start(file_content.cbegin());
    CBuild::print_full(std::string("Include search start"));
    while (std::regex_search(start, file_content.cend(), match, include_detector)) {
        CBuild::print_full(std::string("Include found, include: ") + match[1].str());
        std::string tmp = "";
        try {
            tmp = CBuild::fs::normalize_relative_path(match[1], CBuild::fs::base(file));
        } catch (std::exception& e) {
        }
        if (tmp != std::string("") && CBuild::fs::exists(tmp)) {
            CBuild::print_full(std::string("Include added, include: ") + tmp);
            m.new_meta.deps.push_back(tmp);
            try {
                data->headers_to_parse.push_back_check(tmp, false);
            } catch (std::exception& e) {
            }
        }
        start = match.suffix().first;
    }
    CBuild::print_full("Metadata store");
    // Store metadata into a shared data
    if (object == std::string("header")) {
        try {
            data->headers_metadata.push_back_check(file, m);
        } catch (std::exception& e) {
        }
    } else {
        try {
            data->sources_metadata.push_back_check(file, m);
        } catch (std::exception& e) {
        }
    }
    // Rewrite metadata with new data
    CBuild::write_file_metadata(target_id, file, &(m.new_meta));
    CBuild::print_full(std::string("End"));
}
void print_metadata(extended_metadata* m) {
    CBuild::printf_full(CBuild::WHITE, "File: '%s'\nObject: '%s'\nDeps [new]:\n",
                        m->new_meta.source.c_str(), m->new_meta.object.c_str());
    for (auto dep : m->new_meta.deps) {
        CBuild::printf_full(CBuild::WHITE, "\t%s\n", dep.c_str());
    }
    CBuild::printf_full(CBuild::WHITE, "Deps [old]: %s\n",
                        (m->old_meta_available == true) ? "" : "null");
    if (m->old_meta_available) {
        for (auto dep : m->old_meta.deps) {
            CBuild::printf_full(CBuild::WHITE, "\t%s\n", dep.c_str());
        }
    }
    CBuild::printf_full(CBuild::WHITE,
                        "Hash [new]: '0x%" PRIx64 "'\nHash [old]: ", m->new_meta.hash);
    if (m->old_meta_available) {
        CBuild::printf_full(CBuild::WHITE, "'0x%" PRIx64 "'\n", m->old_meta.hash);
    } else {
        CBuild::printf_full(CBuild::WHITE, "'null'\n");
    }
}

bool check_mismatch_recursive(const extended_metadata* file, internal_data* data) {
    // std::printf("Checking file '%s'\n", file->new_meta.source.c_str());
    if (file->old_meta_available == false) {
        // std::printf("File: '%s' -> no_old_meta\n", file->new_meta.source.c_str());
        return true;
    }
    if (file->new_meta.object != file->old_meta.object) {
        // std::printf("File: '%s' -> mismatch object\n", file->new_meta.source.c_str());
        return true;
    }
    if (file->new_meta.hash != file->old_meta.hash) {
        // std::printf("File: '%s' -> mismatch hash\n", file->new_meta.source.c_str());
        return true;
    }
    if (file->new_meta.deps != file->old_meta.deps) {
        // std::printf("File: '%s' -> mismatch deps\n", file->new_meta.source.c_str());
        return true;
    }
    for (auto dep : file->new_meta.deps) {
        auto header = data->headers_metadata.get(dep);
        if (header != NULL) {
            if (check_mismatch_recursive(header, data)) {
                return true;
            }
        }
        auto source = data->sources_metadata.get(dep);
        if (source != NULL) {
            if (check_mismatch_recursive(source, data)) {
                return true;
            }
        }
    }
    return false;
}
/* build_hash.hpp */
lib::map<std::string, std::string>
CBuild::CBuildHashV2::get_files_for_recompilation(std::vector<std::string> sources,
                                                  std::vector<std::string> objects) {
    if (sources.size() != objects.size()) {
        return {};
    }
    internal_data data;
    CBuild::print_full("Parsing files");
    // Get sources metadata
    for (size_t i = 0; i < sources.size(); i++) {
        parse_file(&data, target_id, sources.at(i), objects.at(i));
    }
    CBuild::print_full("Parsing headers");
    // Get headers metadata
    for (size_t i = 0; i < data.headers_to_parse.size(); i++) {
        auto elem = data.headers_to_parse.ptr_at(i);
        parse_file(&data, target_id, elem->key, "header");
        elem->data = true;
    }
    // Fancy prints
    CBuild::print_full("Source files:");
    for (auto elem : data.sources_metadata) {
        print_metadata(&(elem.data));
    }
    CBuild::print_full("Header files:");
    for (auto elem : data.headers_metadata) {
        print_metadata(&(elem.data));
    }
    lib::map<std::string, std::string> ret;
    // Get files for recompilation
    for (auto file : data.sources_metadata) {
        if (check_mismatch_recursive(&(file.data), &data)) {
            ret.push_back(file.data.new_meta.source, file.data.new_meta.object);
        }
    }
    return ret;
}
