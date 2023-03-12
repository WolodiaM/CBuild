/**
 * @file register.hpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Register any things
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
// Project headers
#include "task/Task.hpp"
#include "build/Build.hpp"
#ifndef __CBUILD_REGISTER_HPP__
#define __CBUILD_REGISTER_HPP__
namespace CBuild
{
    namespace Registry
    {
        /**
         * @brief Internal init
         */
        void init();
        /**
         * @brief Register new task
         * 
         * @param task => CBuild::Task* -> pointer to staticly allocated task
         */
        void RegistryTask(CBuild::Task* task);
        /**
         * @brief Run tasks
         * 
         * @param name => std::string -> Name of task
         * @param args => std::vector<std::string> -> Arguments for task
         */
        void CallTask(std::string name, std::vector<std::string> args);
        /**
         * @brief Register new target for build
         * 
         * @param target => CBuild::Toolchain* -> Pointer to staticly allocated class that extends CBuild::Toolchain
         */
        void RegisterTarget(CBuild::Toolchain* target);
        /**
         * @brief Get the registered toolchain
         * 
         * @param name => std::string -> target id
         * @param force => bool -> default = false, if true does not look in list of executed toolchains
         * @return CBuild::Toolchain* -> pointer to toolchain or null if target with given id does not exists
         */
        CBuild::Toolchain* GetToolchain(std::string name, bool force = false);
        /**
         * @brief Get list of all targets
         * 
         * @return lib::map<std::string, CBuild::Toolchain*> 
         */
        lib::map<std::string, CBuild::Toolchain*> GetTargets();
        /**
         * @brief Register new keyord for parsing,
         * ! Dangerous, owerwriting existing keywords can lead to undefined behaviours
         * 
         * @param key => std::string -> new keyword
         * @param func => Cbuild::Task -> task, associated with keyword
         */
        void RegisterKeyword(std::string key, CBuild::Task* func);
        /**
         * @brief Get list of all user registered keywords
         * 
         * @returns lib::map<std::string, std::string> where key - keyword, value - task id to call
         */
        lib::map<std::string, std::string> GetKeywordsList();
    } // namespace Registry
} // namespace CBuild
#endif // __CBUILD_REGISTER_HPP__