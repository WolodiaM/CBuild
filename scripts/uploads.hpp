/**
 * @file uploads.hpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Upload scripts for SourceForge
 * @date 2023-10-09
 *
 * @copyright (C) 2023 WolodiaM
 * @license GPL v3.0 or later
 *
 * Copyright (C) 2023 WolodiaM
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
// Project includes
#include "../CBuild/headers/files.hpp"
#include "../CBuild/headers/filesystem++.hpp"
#include "../CBuild/headers/system.hpp"
#include "../CBuild/headers/task/Task.hpp"
// Code
#ifndef __UPLOADS_HPP__
#define __UPLOADS_HPP__
class upload : public CBuild::Task {
  protected:
    std::string script;

  public:
    upload(std::string id, std::string sc) : CBuild::Task(id, {}) {
        this->script = sc;
    }
    void call(std::vector<std::string> args) {
        CBuild::system(this->script + std::string(" ") +
                       CBuild::fs::normalize_path("."));
    }
};
#endif // __UPLOADS_HPP__
