/**
 * @file Task.cpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Task implementation
 * @version 1.0
 * @date 2023-01-28
 *
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
#include "../../headers/task/Task.hpp"
/* Task.hpp */
CBuild::Task::Task(std::string name, std::vector<std::string> required) {
	// Save name and required tasks
	this->name = name;
	this->required = required;
}
std::string CBuild::Task::self_name() {
	// Return name
	return this->name;
}
std::vector<std::string> CBuild::Task::self_required() {
	// Return required tasks
	return this->required;
}
