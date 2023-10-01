/**
 * @file generator.cpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief CBuild internal generators implementation
 * @date 2023-07-24
 *
 * @copyright (C) 2023 WolodiaM
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
// C++ libraries
#include "array"
#include "fstream"
#include "iostream"
#include "sstream"
#include "string"
#include "string_view"
#include "vector"
// Project includes
#include "../../headers/CBuild.hpp"
#include "../../headers/filebuff.hpp"
#include "../../headers/filesystem++.hpp"
#include "../../headers/generator/ccj.hpp"
#include "../../headers/generator/makefile.hpp"
#include "../../headers/map.hpp"
#include "../../headers/print.hpp"
#include "../../headers/register.hpp"
#include "../../headers/system.hpp"
/* makefile.hpp */
void CBuild::makefile_out::generate(CBuild::RType                       mode,
                                    lib::map<std::string, std::string> *args) {
    CBuild::print("Generating Makefile ...", CBuild::color::MAGENTA);
    // Get command log
    auto log = CBuild::get_log();
    // Create Makefile if needed
    if (!CBuild::fs::exists(this->OUT)) {
        CBuild::fs::create({this->OUT}, CBuild::fs::FILE);
    }
    // Open file
    std::ofstream makefile;
    makefile.open(this->OUT, std::ios::app);
    std::string name = "";
    // Create name of make task, first part - CBuild run type or
    // cbuild_ placeholder
    switch (mode) {
    case CBuild::BUILD:
        name += "build_";
        break;
    case CBuild::BUILD_RUN:
        name += "build_run_";
        break;
    case CBuild::RUN:
        name += "run_";
        break;
    case CBuild::CLEAR:
        name += "clear_";
        break;
    case CBuild::DEBUG:
        name += "debug_";
        break;
    case CBuild::TASK:
        name += "task_";
        break;
    default:
        name += "cbuild_";
        break;
    }
    // Second part of make task - target or task id or cbuild
    // placeholder
    if (mode == CBuild::BUILD || mode == CBuild::BUILD_RUN ||
        mode == CBuild::RUN || mode == CBuild::DEBUG || mode == CBuild::CLEAR) {
        name += *(args->get("toolchain_id"));
    } else if (mode == CBuild::TASK) {
        name += *(args->get("task_id"));
    } else {
        name += CBuild::get_random_string(6);
    }
    // Write all executed command to Makefile
    makefile << "\n";
    makefile << name << ":\n";
    for (auto elem : *log) {
        makefile << "\t" << elem << "\n";
    }
    // End Makefile
    makefile << "\n";
    makefile.close();
}
bool CBuild::makefile_out::init() { return true; }
/* ccj.hpp - helper */
namespace CBuild {
/**
 * @brief Evaluate compilation cmd
 *
 * @param data => std::array<std::string, 4> -> Data:
 * 		- 0: File path
 * 		- 1: Object file
 * 		- 2: Compiler command
 * 		- 3: Args
 * @return std::string -> Command
 */
std::string eval_cmd(std::array<std::string, 4> data) {
    std::string cmd = data[2] + " -c ";
    cmd += data[0];
    cmd += " ";
    cmd += data[3];
    cmd += " -o ";
    cmd += data[1];
    return cmd;
}
/**
 * @brief Command for compile_commands.json
 */
typedef struct {
    /**
     * @brief "directory":
     */
    std::string base_path;
    /**
     * @brief "command":
     */
    std::string cmd;
    /**
     * @brief "file":
     */
    std::string file;
    /**
     * @brief Does this entry is already in file
     */
    bool in_file;
} cmd;
/**
 * @brief Preprocess string for json
 *
 * @param str => std::string_view -> Input string
 * @return std::string -> Output string
 */
std::string preprocess_json_str(std::string_view str) {
    std::string ret;
    for (char c : str) {
        switch (c) {
        case '\"':
            ret += "\\\"";
            break;
        case '\\':
            ret += "\\\\";
            break;
        case '/':
            ret += "\\/";
            break;
        case '\b':
            ret += "\\b";
            break;
        case '\f':
            ret += "\\f";
            break;
        case '\n':
            ret += "\\n";
            break;
        case '\r':
            ret += "\\r";
            break;
        case '\t':
            ret += "\\t";
            break;
        default:
            ret += c;
            break;
        }
    }
    return ret;
}
} // namespace CBuild
//{
//	"directory": "/home/wolodiam/dev/c++/CBuild",
//	"command": "g++ -c file.cpp -std=c++20 -o file.o",
// 	"file": "/home/wolodiam/dev/c++/CBuild/CBuild/CBuild/src/system.cpp"
// }
/* ccj.hpp */
void CBuild::ccj_out::generate(CBuild::RType                       mode,
                               lib::map<std::string, std::string> *args) {
    // Fail fast if we not building
    if (mode != CBuild::BUILD) {
        CBuild::print("Not in build mode, ERROR!", CBuild::RED);
        return;
    }
    // Cannot generate compile_commands.json for meta-toolchain 'all'
    if (*(args->get("toolchain_id")) == std::string("all")) {
        CBuild::print(
            "Cannot generate compile commands for meta-toolchain \"all\"",
            CBuild::RED);
        return;
    }
    // Get commands
    // Vars
    lib::map<std::string, CBuild::cmd> data;
    // Get data from target
    auto toolchain =
        CBuild::Registry::GetToolchain(*(args->get("toolchain_id")));
    std::vector<std::string> dummy_vec = {};
    toolchain->call(&dummy_vec, true, false, true);
    auto files = toolchain->gen_file_list_force();
    auto cmds  = toolchain->get_cmds();
    auto targs = toolchain->get_compile_args();
    auto dir   = args->get_ptr("curr_path")->data;
    for (auto file : files) {
        data.push_back({file.key,
                        {.base_path = dir,
                         .cmd     = CBuild::eval_cmd(std::array<std::string, 4>{
                                 file.key, file.data, cmds[0], targs}),
                         .file    = file.key,
                         .in_file = false}});
    }
    // Store in file
    CBuild::print("Generating compile_commands.json ...",
                  CBuild::color::MAGENTA);
    CBuild::print("This can breaks if compile_commands.json already exists and "
                  "this file was not created by CBuild!",
                  CBuild::color::RED);
    if (CBuild::fs::exists(this->OUT) == false) {
        CBuild::print_full("File not found, creating file...");
        CBuild::fs::create({this->OUT}, CBuild::fs::FILE);
        std::fstream ccj(this->OUT);
        ccj << "[\n";
        for (unsigned int j = 0; j < data.size(); j++) {
            auto elem = data.at(j);
            if (j == 0) {
                ccj << "{";
            } else {
                ccj << ",\n{\n";
            }
            ccj << (std::string("\t\"directory\": \"") + dir +
                    std::string("\",\n"));
            ccj << (std::string("\t\"command\": \"") +
                    CBuild::preprocess_json_str(elem.data.cmd) +
                    std::string("\",\n"));
            ccj << (std::string("\t\"file\": \"") + elem.data.file +
                    std::string("\"\n"));
            ccj << "}";
        }
        ccj << "\n]";
    } else {
        CBuild::print_full("File found, appending...");
        CBuild::line_filebuff ccj(this->OUT);
        ccj.update();
        int i = 0;
        try {
            while (true) {
                std::string str = ccj.get_line(i);
                size_t      pos = str.find("\"file\":");
                if (pos != std::string::npos) {
                    // We find file
                    size_t      st    = str.find_first_of('"', pos + 7);
                    size_t      ed    = str.find_last_of('"');
                    std::string fname = str.substr(st + 1, ed - st - 1);
                    auto        elem  = data.get_ptr(fname);
                    if (elem != NULL) {
                        // File is recompiled, so replace "command"
                        CBuild::print_full(
                            std::string("Found file \"") + fname +
                            std::string("\" that is recompiled, replacing "
                                        "\"command\" entry"));
                        ccj.del_line(i - 1);
                        ccj.set_line(
                            std::string("\t\"command\": \"") +
                                CBuild::preprocess_json_str(elem->data.cmd) +
                                std::string("\","),
                            i - 1);
                        // Mark file as processed
                        elem->data.in_file = true;
                    }
                }
                i++;
            }
        } catch (std::exception &e) {
            // Add coma at the end, no worry about this later
            ccj.del_line(i - 2);
            ccj.set_line("},", i - 2);
            // Check if all files are processed
            for (unsigned int j = 0; j < data.size(); j++) {
                auto elem = data.at(j);
                if (elem.data.in_file == false) {
                    CBuild::print_full(std::string("Found new file: \"") +
                                       elem.data.file +
                                       std::string("\", inserting new entry"));
                    // If file is not in database - insert entry, we dont need
                    // to sert in_file tag, because this is last part, where
                    // data map is valid
                    ccj.set_line("{", i - 1);
                    i++;
                    ccj.set_line(std::string("\t\"directory\": \"") + dir +
                                     std::string("\","),
                                 i - 1);
                    i++;
                    ccj.set_line(
                        std::string("\t\"command\": \"") +
                            CBuild::preprocess_json_str(elem.data.cmd) +
                            std::string("\","),
                        i - 1);
                    i++;
                    ccj.set_line(std::string("\t\"file\": \"") +
                                     elem.data.file + std::string("\""),
                                 i - 1);
                    i++;
                    ccj.set_line("},", i - 1);
                    i++;
                }
            }
            // Remove comma after last element as per json specs
            ccj.del_line(i - 2);
            ccj.set_line("}", i - 2);
            ccj.update();
        }
    }
}
bool CBuild::ccj_out::init() {
    CBuild::disable_system();
    return false;
}
