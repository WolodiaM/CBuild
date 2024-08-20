/**
 * @file cross_compile.hpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief CrossCompilation front-end for CBuild
 * @date 2024-05-05
 *
 * @copyright (C) 2024 WolodiaM
 * @license GPL v3.0 or later
 *
 * Copyright (C) 2024 WolodiaM
 *
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
// Project includes
#include "Build.hpp"
// Code
#ifndef __CROSS_COMPILE_HPP__
#define __CROSS_COMPILE_HPP__
namespace CBuild {
/**
 * @class CrossCompiler
 * @brief Toolchain that wraps multiple other toolchain to support cross compilation
 * @deprecated Not implement for now
 *
 */
class CrossCompiler {
  protected:
    /**
     * @class ToolData
     * @brief Internal toolchain data
     */
    struct ToolData {
        std::string int_id;
        CBuild::Toolchain* tool;
        bool internally_managed;
    };
    /**
     * @brief List of internal toolchains
     */
    std::vector<CBuild::CrossCompiler::ToolData> toolchain_list;
    /**
     * @brief Global configuration
     */
    CBuild::Toolchain* meta_config;
    /**
     * @brief Meta-toolchain fot this group
     */
    CBuild::Toolchain* meta_toolchain;
    /**
     * @brief Id of this group
     */
    std::string meta_id;
    /**
     * @brief Output name for this group
     */
    std::string meta_name;

  public:
    /**
     * @brief Create new cross-compilation collection
     *
     * @param id => std::string -> Collection id
     * @param name => std::string -> Collection output name
     */
    CrossCompiler(std::string id, std::string name);
    /**
     * @brief Create new architecture dependant tool
     *
     * @param id => std::string -> Id of tool
     * @param tool => CBuild::Toolchain* -> Preconfigured toolchain
     */
    void add_arch_tool(std::string id, CBuild::Toolchain* tool);
    /**
     * @brief Create new architecture dependant tool, toolchain instance will be created
     *
     * @param id => std::string -> Id of tool
     */
    void add_arch_tool(std::string id);
    /**
     * @brief Get toolchain for specific arch
     *
     * @param id => std::string -> Id of needed toolchain
     * @return CBuild::Toolchain* -> Toolchain pointer
     */
    CBuild::Toolchain* get_arch_tool(std::string id);
    /**
     * @brief Register all internal toolchains
     */
    void perform_registry();
    /**
     * @brief Get global configuration (for compatibility reasons returns CBuild::Toolchain* to
     * config)
     *
     * @return CBuild::Toolchain* -> Configuration class
     */
    CBuild::Toolchain* get_global_config();
    /**
     * @brief Load global configuration data into local, per-toolchain, configuration
     */
    void apply_global_config();
    /**
     * @brief Set default toolchain
     *
     * @param arch => std::string -> Id of default toolchain
     */
    void set_default_tool(std::string arch);
    /**
     * @brief Get list of sub-toolchains in this collection (skips config and meta-toolchain)
     *
     * @returns std::vector<std::string> -> List of intel toolchain id's
     */
    std::vector<std::string> get_toolchain_list();
    /**
     * @brief Destructor for this class
     */
    ~CrossCompiler() {
        for (auto tool : this->toolchain_list) {
            if (tool.internally_managed) {
                delete tool.tool;
            }
        }
        delete this->meta_config;
        delete this->meta_toolchain;
    }
};
} // namespace CBuild
#endif // __CROSS_COMPILE_HPP__
