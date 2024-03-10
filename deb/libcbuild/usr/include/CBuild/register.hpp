/**
 * @file register.hpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Register any things
 * @date 2023-01-28
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
#ifndef __CBUILD_REGISTER_HPP__
#define __CBUILD_REGISTER_HPP__
// C++ libraries
#include "string"
#include "vector"
// Project headers
#include "build/Build.hpp"
#include "generator/generator.hpp"
#include "task/Task.hpp"
// Code
namespace CBuild {
namespace Registry {
/**
 * @brief Internal init
 */
void init();
/**
 * @brief Register new task
 *
 * @param task => CBuild::Task* -> pointer to staticly allocated task
 */
void RegisterTask(CBuild::Task* task);
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
 * @param target => CBuild::Toolchain* -> Pointer to staticly allocated class
 * that extends CBuild::Toolchain
 */
void RegisterTarget(CBuild::Toolchain* target);
/**
 * @brief Get the registered toolchain
 *
 * @param name => std::string -> target id
 * @param force => bool -> default = false, if true does not look in list of
 * executed toolchains
 * @return CBuild::Toolchain* -> pointer to toolchain or null if target with
 * given id does not exists
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
 * @returns lib::map<std::string, std::string> where key - keyword, value - task
 * id to call
 */
lib::map<std::string, std::string> GetKeywordsList();
/**
 * @brief Set the rebuilt CBuild executable name
 * @param _name => std::string -> new CBuild user executable name
 */
void SetRebuildName(std::string _name);
/**
 * @brief Add linker arg for CBuild rebuild
 * @param arg => std::string -> raw argument
 */
void AddLinkArg(std::string arg);
/**
 * @brief Add compiler argument for CBuild rebuild
 * @param arg => std::string -> raw argument
 */
void AddCompileArg(std::string arg);
/**
 * @brief Set handler to add output to `--version`
 *
 * @param handler => void (*)() -> Handler
 */
void SetVersionHandler(void (*handler)());
/**
 * @brief Call and execute all toolchains
 *
 * @param force => bool -> Compilation type
 * @param path => std::string -> Base path
 * @param args => std::vector<std::string>* -> PArgs
 */
void ToolchainAll(bool force, std::string path, std::vector<std::string>* args);
/**
 * @brief Get generator for specified id
 *
 * @param id => std::string -> Generator id
 * @return CBuild::generator_base* -> Pointer to generator instance
 */
CBuild::generator_base* GetGenerator(std::string id);
/**
 * @brief Register generator for id
 *
 * @param gen => CBuild::generator_base* -> Generator instance
 * @param id => std::string -> Geneator id
 */
void RegisterGenerator(CBuild::generator_base* gen, std::string id);
/**
 * @brief Get list of registered generators
 *
 * @return std::vector<std::string> -> List of generators
 */
std::vector<std::string> GetGeneratorsList();
/**
 * @brief Get list of registered toolchains
 *
 * @return std::vector<std::string> -> List of toolchains
 */
std::vector<std::string> GetToolchainsList();
/**
 * @brief Get list of registered tasks
 *
 * @return std::vector<std::string> -> List of tasks
 */
std::vector<std::string> GetTasksList();
/**
 * @brief Get target used for rebuild of CBuild script
 *
 * @return CBuild::Toolcgain* -> Rebuild target
 */
CBuild::Toolchain* GetRebuildTarget();
} // namespace Registry
} // namespace CBuild
#endif // __CBUILD_REGISTER_HPP__
