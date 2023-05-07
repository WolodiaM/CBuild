/**
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Task initializer
 * @date Tue May  2 08:40:12 PM EEST 2023
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
// CBuild headers
#include "../CBuild/headers/register.hpp"
#include "../CBuild/headers/task/Task.hpp"
#include "string"
// Scripts headers
#include "pack.hpp"
#include "user_init.hpp"
// Tasks
pack_deb packd;
// Init
void load_tasks() { CBuild::Registry::RegistryTask(&packd); }
