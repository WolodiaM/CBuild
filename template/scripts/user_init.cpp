/**
 * @file user_init.cpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Main implementation file for userspace scripts
 * @version 1.0
 * @date <date>
 *
 *
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
// C++ libraries
#include "stdio.h"
// Userspace headers
#include "user_init.hpp"
// CBuild headers
#include "CBuild/build/g++.hpp"
#include "CBuild/build/mingw-g++.hpp"
#include "CBuild/print.hpp"
#include "CBuild/register.hpp"
#include "CBuild/task/Task.hpp"

CBuild::GXX multiplication("mull", "Multiplication test");
CBuild::GXX multiplication_lib("mull_lib", "mull");
CBuild::MINGW_GXX multiplication_win("mullw", "Multiplication test");
CBuild::MINGW_GXX multiplication_lib_win("mullw_lib", "mull");

void init() {
  multiplication.set_standart("c++20");
  multiplication.add_file("src/main.cpp");
  multiplication.warn();
  multiplication.set_type(CBuild::EXECUTABLE);
  multiplication.depends_on("mull_lib");
  CBuild::Registry::RegisterTarget(&multiplication);
  multiplication_lib.set_standart("c++20");
  multiplication_lib.add_file("src/test.cpp");
  multiplication_lib.warn();
  multiplication_lib.set_type(CBuild::DYNAMIC_LIBRARY);
  CBuild::Registry::RegisterTarget(&multiplication_lib);
  multiplication_win.set_standart("c++20");
  multiplication_win.add_file("src/main.cpp");
  multiplication_win.warn();
  multiplication_win.set_type(CBuild::EXECUTABLE);
  multiplication_win.depends_on("mullw_lib");
  CBuild::Registry::RegisterTarget(&multiplication_win);
  multiplication_lib_win.set_standart("c++20");
  multiplication_lib_win.add_file("src/test.cpp");
  multiplication_lib_win.warn();
  multiplication_lib_win.set_type(CBuild::DYNAMIC_LIBRARY);
  CBuild::Registry::RegisterTarget(&multiplication_lib_win);
}
