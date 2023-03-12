/**
 * @file Task.hpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Task main class
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
// C++ libraries
#include "string"
#include "vector"
#ifndef __CBUILD_TASK_HPP__
#define __CBUILD_TASK_HPP__
namespace CBuild
{
    /**
     * @brief Task, can be runned from shell or code
     */
    class Task
    {
    private:
        std::string name;
        std::vector<std::string> required;
    public:
        /**
         * @brief Construct a new Task object
         * 
         * @param name Task name
         * @param required Reguired tasks
         */
        Task(std::string name, std::vector<std::string> required);
        /**
         * @brief Executed on task call
         * @param args=> std::vector<std::string> -> used internally
         */
        virtual void call(std::vector<std::string> args) = 0;
        /**
         * @brief Return task name
         * 
         * @return std::string -> Task name
         */
        std::string self_name();
        /**
         * @brief Return required tasks
         * 
         * @return std::vector<std::string> -> Required tasks's id list
         */
        std::vector<std::string> self_required();
    };
} // namespace CBuild
#endif // __CBUILD_TASK_HPP__