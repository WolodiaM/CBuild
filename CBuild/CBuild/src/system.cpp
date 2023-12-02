/**
 * @file system.cpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief custom system() wrapper
 * @date 2023-03-11
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
// C++ libraries
#include "map"
#include "stdlib.h"
#include "string"
#include "vector"
// Project headers
#include "../../headers/environment.hpp"
#include "../../headers/print.hpp"
#include "../../headers/system.hpp"
/* namespace CBuild */
namespace CBuild {
std::vector<std::string> log;
std::map<std::string, std::string> env_vars;
bool enabled = true;
std::string get_env_vars() {
    std::string ret = "";
    for (auto elem : CBuild::env_vars) {
        ret += elem.first;
        ret += std::string("=");
        ret += elem.second;
        ret += std::string("; ");
    }
    return ret;
}
} // namespace CBuild
/* system.hpp */
int CBuild::system(std::string cmd) {
    CBuild::log.push_back(cmd);
    int ret = -1;
    if (CBuild::enabled) {
        CBuild::print(CBuild::get_env_vars() + cmd, CBuild::color::BLUE);
        ret = std::system((CBuild::get_env_vars() + cmd).c_str());
    } else {
        ret = 0;
    }
    return ret;
}
std::string CBuild::system_piped(std::string cmd, unsigned int buffsize) {
    CBuild::log.push_back(cmd);
    if (CBuild::enabled) {
        CBuild::print(CBuild::get_env_vars() + cmd, CBuild::color::BLUE);
        std::string ret = "";
        char* buffer = (char*)malloc(buffsize);
        FILE* pipe = popen((CBuild::get_env_vars() + cmd).c_str(), "r");
        size_t bytesread;
        while ((bytesread = fread(buffer, sizeof(buffer[0]), sizeof(buffer), pipe)) != 0) {
            ret += std::string(buffer, bytesread);
        }
        pclose(pipe);
        return ret;
    }
    return "";
}
std::vector<std::string>* CBuild::get_log() {
    return &CBuild::log;
}
void CBuild::disable_system() {
    CBuild::enabled = false;
}
void CBuild::enable_system() {
    CBuild::enabled = true;
}
bool CBuild::is_system_enabled() {
    return CBuild::enabled;
}
/* environment.hpp */
void CBuild::add_env_var(std::string var, std::string value) {
    CBuild::env_vars.insert(std::make_pair(var, value));
}
std::string CBuild::remove_env_var(std::string var) {
    std::string ret = CBuild::env_vars.at(var);
    CBuild::env_vars.erase(var);
    return ret;
}
void CBuild::change_env_var(std::string var, std::string new_value) {
    CBuild::env_vars.at(var) = new_value;
}
