/**
 * @file metadata_file.cpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @date 2023-12-06
 * @brief Implementation for build_data.hpp
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
// C++ libs
#include "cstdint"
#include "fstream"
#include "sstream"
#include "string"
#include "vector"
// Project headers
#include "../../headers/CBuild_defs.hpp"
#include "../../headers/build_data.hpp"
#include "../../headers/filesystem++.hpp"
/* Internal funcs */
int read_file_metadata_internal(std::string path, CBuild::source_metadata_file* metadata) {
    if (!CBuild::fs::exists(path)) {
        metadata = NULL;
        return -1;
    }
    std::fstream f;
    std::string FILE_DATA;
    f.open(path);
    std::getline(f, FILE_DATA, '\0');
    f.close();
    std::string line;
    std::stringstream FILE_DATA_STREAM(FILE_DATA);
    std::getline(FILE_DATA_STREAM, line, '\n');
    metadata->source = line;
    std::getline(FILE_DATA_STREAM, line, '\n');
    metadata->object = line;
    std::getline(FILE_DATA_STREAM, line, '\n');
    if (line != std::string("DEPS")) {
        return -3;
    }
    std::getline(FILE_DATA_STREAM, line, '\n');
    while (line != std::string("ENDDEPS")) {
        metadata->deps.push_back(line);
        std::getline(FILE_DATA_STREAM, line, '\n');
    }
    std::getline(FILE_DATA_STREAM, line, '\n');
    if (!std::isdigit(line.front()) || !std::isdigit(line.back())) {
        return -3;
    }
    metadata->hash = std::strtoull(line.c_str(), NULL, 10);
    return 0;
}
/* build_data.hpp */
int CBuild::read_file_metadata(std::string target_id, std::string src_file,
                               CBuild::source_metadata_file* metadata) {
    std::string path = CBuild::get_file_metadata_path(target_id, src_file);
    return read_file_metadata_internal(path, metadata);
}
int CBuild::read_file_metadata_direct(std::string target_id __attribute_maybe_unused__,
                                      std::string file, CBuild::source_metadata_file* metadata) {
    std::string path = file;
    return read_file_metadata_internal(path, metadata);
}
int CBuild::read_target_metadata(std::string target_id, CBuild::target_metadata_file* metadata) {
    std::string path = CBuild::get_target_metadata_path(target_id);
    if (!CBuild::fs::exists(path)) {
        metadata = NULL;
        return -1;
    }
    std::fstream f;
    std::string FILE_DATA;
    f.open(path);
    std::getline(f, FILE_DATA, '\0');
    f.close();
    std::string line;
    std::stringstream FILE_DATA_STREAM(FILE_DATA);
    std::getline(FILE_DATA_STREAM, line, '\n');
    metadata->out = line;
    std::getline(FILE_DATA_STREAM, line, '\n');
    metadata->compiler = line;
    std::getline(FILE_DATA_STREAM, line, '\n');
    metadata->linker = line;
    std::getline(FILE_DATA_STREAM, line, '\n');
    metadata->packer = line;
    std::getline(FILE_DATA_STREAM, line, '\n');
    if (line != std::string("CARGS")) {
        return -3;
    }
    std::getline(FILE_DATA_STREAM, line, '\n');
    while (line != std::string("ENDCARGS")) {
        metadata->cargs.push_back(line);
        std::getline(FILE_DATA_STREAM, line, '\n');
    }
    std::getline(FILE_DATA_STREAM, line, '\n');
    if (line != std::string("LARGS")) {
        return -3;
    }
    std::getline(FILE_DATA_STREAM, line, '\n');
    while (line != std::string("ENDLARGS")) {
        metadata->largs.push_back(line);
        std::getline(FILE_DATA_STREAM, line, '\n');
    }
    return 0;
}
int CBuild::read_file_hash(std::string target_id, std::string file, uint64_t* hash) {
    CBuild::source_metadata_file m;
    int ret = CBuild::read_file_metadata(target_id, file, &m);
    *hash = m.hash;
    return ret;
}
int CBuild::write_file_metadata(std::string target_id, std::string src_file,
                                CBuild::source_metadata_file* metadata) {
    std::string path = CBuild::get_file_metadata_path(target_id, src_file);
    std::ofstream f(path);
    f << metadata->source << "\n";
    f << metadata->object << "\n";
    f << "DEPS\n";
    for (auto dep : metadata->deps) {
        f << dep << "\n";
    }
    f << "ENDDEPS\n";
    f << metadata->hash << "\n";
    f.close();
    return 0;
}
int CBuild::write_target_metadata(std::string target_id, CBuild::target_metadata_file* metadata) {
    std::string path = CBuild::get_target_metadata_path(target_id);
    std::ofstream f(path);
    f << metadata->out << "\n";
    f << metadata->compiler << "\n";
    f << metadata->linker << "\n";
    f << metadata->packer << "\n";
    f << "CARGS\n";
    for (auto dep : metadata->cargs) {
        f << dep << "\n";
    }
    f << "ENDCARGS\n";
    f << "LARGS\n";
    for (auto dep : metadata->largs) {
        f << dep << "\n";
    }
    f << "ENDLARGS\n";
    f.close();
    return 0;
}
int CBuild::write_file_hash(std::string target_id, std::string file, uint64_t* hash) {
    CBuild::source_metadata_file m;
    int ret = CBuild::read_file_metadata(target_id, file, &m);
    if (ret != 0) {
        return ret;
    }
    m.hash = *hash;
    return CBuild::write_file_metadata(target_id, file, &m);
}
std::string CBuild::get_file_metadata_path(std::string target_id, std::string file) {
    // Remove file extension
    unsigned long pos = file.find_last_of(".");
    file = file.substr(0, pos);
    // Replace '/' with '.'
    while (file.find("/") != std::string::npos) {
        file.replace(file.find("/"), std::string("/").size(), ".");
    }
    return CBUILD_BUILD_DIR + "/" + target_id + "/" + CBUILD_METADATA_FOLDER + "/" + file +
           CBUILD_METADATA_FILE_EXTENSION;
}
std::string CBuild::get_target_metadata_path(std::string target_id) {
    return CBUILD_BUILD_DIR + "/" + target_id + "/" + target_id + CBUILD_METADATA_FILE_EXTENSION;
}
