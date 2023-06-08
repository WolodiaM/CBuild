/**
 * @file CBuild_help_task.hpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Help print routine for CBuild
 * @date 2023-06-08
 *
 * @copyright (C) 2023 WolodiaM
 * @license GPL v3.0 or later
 *
 * Copyright (C) 2022  WolodiaM
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
// Include c++ libraries
#include "string"
// Include CBuild headers
#include "../print.hpp"
#include "Task.hpp"
namespace CBuild {
class Help : public CBuild::Task {
private:
  std::string data = "@HELP_MSG@";

public:
  Help() : CBuild::Task("CBuild_help", {}){};
  void call(std::vector<std::string> args) { CBuild::print(data); }
};
} // namespace CBuild