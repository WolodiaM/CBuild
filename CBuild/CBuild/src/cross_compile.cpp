/**
 * @file cross_compile.cpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Implementation of cross-compilation front-end
 * @date 2024-05-06
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
#include "../../headers/map.hpp"
#include "string"
// Project includes
#include "../../headers/build/Build.hpp"
#include "../../headers/build/cross_compile.hpp"
#include "../../headers/build/simple_toolchain.hpp"
#include "../../headers/hasher/cbuild_hash.hpp"
#include "../../headers/print.hpp"
#include "../../headers/register.hpp"
/* Config toolchain */
namespace CBuild {
class ConfigToolchain : public CBuild::Toolchain {
  protected:
    bool type_set;
    bool vmajor_set;

  public:
    ConfigToolchain() {
        this->hasher = NULL;
        type_set = false;
        vmajor_set = false;
    }
    void call(std::vector<std::string>* args [[maybe_unused]], bool force = false,
              bool debug = false, bool dummy = false) override {
        CBuild::print("You cannot call \"call\" on config toolchain!", CBuild::RED);
        exit(1);
    }
    void debug(std::vector<std::string>* args, std::vector<std::string>* pargs) override {
        CBuild::print("You cannot call \"debug\" on config toolchain!", CBuild::RED);
        exit(1);
    }
    void clear() override {
        CBuild::print("You cannot call \"clear\" on config toolchain!", CBuild::RED);
        exit(1);
    }
    void init() override {
        CBuild::print("You cannot call \"init\" on config toolchain!", CBuild::RED);
        exit(1);
    }
    void run(std::vector<std::string>* args) override {
        CBuild::print("You cannot call \"run\" on config toolchain!", CBuild::RED);
        exit(1);
    }
    void pre_build() override {
        CBuild::print("You cannot call \"pre_build\" on config toolchain!", CBuild::RED);
        exit(1);
    }
    void build() override {
        CBuild::print("You cannot call \"build\" on config toolchain!", CBuild::RED);
        exit(1);
    }
    void post_build() override {
        CBuild::print("You cannot call \"post_build\" on config toolchain!", CBuild::RED);
        exit(1);
    }
    void pre_link() override {
        CBuild::print("You cannot call \"pre_link\" on config toolchain!", CBuild::RED);
        exit(1);
    }
    void link_pack() override {
        CBuild::print("You cannot call \"link_pack\" on config toolchain!", CBuild::RED);
        exit(1);
    }
    void link() override {
        CBuild::print("You cannot call \"link\" on config toolchain!", CBuild::RED);
        exit(1);
    }
    void post_link() override {
        CBuild::print("You cannot call \"post_link\" on config toolchain!", CBuild::RED);
        exit(1);
    }
    void stdargs() override {
        CBuild::print("You cannot call \"stdargs\" on config toolchain!", CBuild::RED);
        exit(1);
    }
    void compile(std::string cmd) override {
        CBuild::print("You cannot call \"compile\" on config toolchain!", CBuild::RED);
        exit(1);
    }
    void load_project_deps(std::string curr_path) override {
        CBuild::print("You cannot call \"load_project_deps\" on config toolchain!", CBuild::RED);
        exit(1);
    }
    std::array<std::string, 3> get_cmds() override {
        CBuild::print("You cannot call \"get_cmds\" on config toolchain!", CBuild::RED);
        exit(1);
    }
    void set_id(std::string id) override {
        CBuild::print("You cannot call \"set_id\" on config toolchain! Use constructor of "
                      "cross-compilation wrapper for this!",
                      CBuild::RED);
        exit(1);
    }
    void set_name(std::string name) override {
        CBuild::print("You cannot call \"set_name\" on config toolchain! Use constructor of "
                      "cross-compilation wrapper for this!",
                      CBuild::RED);
        exit(1);
    }
    void set_version_major(int version) override {
        this->vmajor_set = true;
        this->version_major = version;
    }
    void set_type(CBuild::build_type type) override {
        this->type_set = true;
        this->build_type = type;
    }
    void clone_config(CBuild::Toolchain* tool) {
        for (auto carg : this->compiler_args) {
            tool->add_compile_arg(carg);
        }
        for (auto larg : this->link_args) {
            tool->add_link_arg(larg);
        }
        for (auto pdep : this->project_deps) {
            tool->depends_on_project(pdep.path, pdep.name, pdep.id, pdep.headers_path);
        }
        for (auto cdep : this->depends) {
            tool->depends_on(cdep);
        }
        for (auto tdep : this->required) {
            tool->add_requirment(tdep.key, tdep.data);
        }
        for (auto pkgcdep : this->pkg_config_include_entries) {
            tool->add_pkgconfig_entry(pkgcdep);
        }
        for (auto edep : this->dependency_list) {
            tool->add_external_dependency(edep);
        }
        for (auto comp : this->targets) {
            if (comp.folder) {
                tool->add_folder(comp.path);
            } else {
                tool->add_file(comp.path);
            }
        }
        if (this->hasher != NULL) {
            tool->set_hasher(this->hasher);
        }
        if (this->type_set) {
            tool->set_type(this->build_type);
        }
        if (this->vmajor_set) {
            tool->set_version_major(this->version_major);
        }
    }
};
class MetaToolchain : public CBuild::Toolchain {
  private:
    CBuild::CrossCompiler* parent;
    std::string default_tool;
    bool default_tool_set;

  public:
    MetaToolchain(CBuild::CrossCompiler* parent) {
        this->parent = parent;
        this->default_tool = "";
        this->default_tool_set = false;
    }
    void build() override {}
    void link() override {}
    void link_pack() override {}
    void load_project_deps(std::string curr_path) override {
        CBuild::print(std::string("Showing toolchain selector:"), CBuild::MAGENTA);
        auto toollist = this->parent->get_toolchain_list();
        size_t i = 0;
        for (; i < toollist.size(); i++) {
            CBuild::printf(CBuild::WHITE, "\t%lu => %s\n", i, toollist.at(i).c_str());
        }
        CBuild::printf(CBuild::MAGENTA, "Enter desired toolchain id [0-%lu]: ", (i - 1));
        size_t val;
        scanf("%lu", &val);
        if (val >= i) {
            CBuild::print("Error, toolchain id overflow. Exiting...", CBuild::RED);
        }
        this->parent->get_arch_tool(toollist.at(val))->load_project_deps(curr_path);
    }
    void run(std::vector<std::string>* args) override {
        CBuild::print(std::string("Showing toolchain selector:"), CBuild::MAGENTA);
        auto toollist = this->parent->get_toolchain_list();
        size_t i = 0;
        for (; i < toollist.size(); i++) {
            CBuild::printf(CBuild::WHITE, "\t%lu => %s\n", i, toollist.at(i).c_str());
        }
        CBuild::printf(CBuild::MAGENTA, "Enter desired toolchain id [0-%lu]: ", (i - 1));
        size_t val;
        scanf("%lu", &val);
        if (val >= i) {
            CBuild::print("Error, toolchain id overflow. Exiting...", CBuild::RED);
        }
        this->parent->get_arch_tool(toollist.at(val))->run(args);
    }
    void call(std::vector<std::string>* args, bool force = false, bool debug = false,
              bool dummy = false) override {
        std::vector<std::string> new_args;
        bool select = false;
        for (auto arg : *args) {
            if (arg == std::string("select")) {
                select = true;
            } else {
                new_args.push_back(arg);
            }
        }
        if (dummy == false) {
            if (this->default_tool_set && !select) {
                CBuild::print(std::string("Redirecting to default toolchain \"") +
                                  this->default_tool + std::string("\"."),
                              CBuild::MAGENTA);
                this->parent->get_arch_tool(this->default_tool)
                    ->call(&new_args, force, debug, false);
            } else {
                CBuild::print(std::string("Showing toolchain selector:"), CBuild::MAGENTA);
                auto toollist = this->parent->get_toolchain_list();
                size_t i = 0;
                for (; i < toollist.size(); i++) {
                    CBuild::printf(CBuild::WHITE, "\t%lu => %s\n", i, toollist.at(i).c_str());
                }
                CBuild::printf(CBuild::MAGENTA, "Enter desired toolchain id [0-%lu]: ", (i - 1));
                size_t val;
                scanf("%lu", &val);
                if (val >= i) {
                    CBuild::print("Error, toolchain id overflow. Exiting...", CBuild::RED);
                }
                this->parent->get_arch_tool(toollist.at(val))->call(&new_args, force, debug, false);
            }
        }
    }
    void debug(std::vector<std::string>* args, std::vector<std::string>* pargs) override {
        std::vector<std::string> new_args;
        bool select = false;
        for (auto arg : *args) {
            if (arg == std::string("select")) {
                select = true;
            } else {
                new_args.push_back(arg);
            }
        }
        if (this->default_tool_set && !select) {
            CBuild::print(std::string("Redirecting to default toolchain \"") + this->default_tool +
                              std::string("\"."),
                          CBuild::MAGENTA);
            this->parent->get_arch_tool(this->default_tool)->debug(&new_args, pargs);
        } else {
        select:
            CBuild::print(std::string("Showing toolchain selector:"), CBuild::MAGENTA);
            auto toollist = this->parent->get_toolchain_list();
            size_t i = 0;
            for (; i < toollist.size(); i++) {
                CBuild::printf(CBuild::WHITE, "\t%lu => %s\n", i, toollist.at(i).c_str());
            }
            CBuild::printf(CBuild::MAGENTA, "Enter desired toolchain id [0-%lu]: ", (i - 1));
            size_t val;
            scanf("%lu", &val);
            if (val >= i) {
                CBuild::print("Error, toolchain id overflow. Exiting...", CBuild::RED);
            }
            this->parent->get_arch_tool(toollist.at(val))->debug(&new_args, pargs);
        }
    }
    void clear() override {
        CBuild::print(std::string("Showing toolchain selector:"), CBuild::MAGENTA);
        auto toollist = this->parent->get_toolchain_list();
        size_t i = 0;
        for (; i < toollist.size(); i++) {
            CBuild::printf(CBuild::WHITE, "\t%lu => %s\n", i, toollist.at(i).c_str());
        }
        CBuild::printf(CBuild::MAGENTA, "Enter desired toolchain id [0-%lu]: ", (i - 1));
        size_t val;
        scanf("%lu", &val);
        if (val >= i) {
            CBuild::print("Error, toolchain id overflow. Exiting...", CBuild::RED);
        }
        this->parent->get_arch_tool(toollist.at(val))->clear();
    }
    std::array<std::string, 3> get_cmds() override {
        return std::array<std::string, 3>{"META", "META", "META"};
    }
    void set_default_tool(std::string arch) {
        this->default_tool = arch;
        this->default_tool_set = true;
    }
};
} // namespace CBuild
/* cross_compile.hpp */
CBuild::CrossCompiler::CrossCompiler(std::string id, std::string name) {
    this->meta_id = id;
    this->meta_name = name;
    this->meta_config = new CBuild::ConfigToolchain();
    this->meta_toolchain = new CBuild::MetaToolchain(this);
}
std::vector<std::string> CBuild::CrossCompiler::get_toolchain_list() {
    std::vector<std::string> ret;
    for (auto arch : this->toolchain_list) {
        ret.push_back(arch.int_id);
    }
    return ret;
}
CBuild::Toolchain* CBuild::CrossCompiler::get_arch_tool(std::string id) {
    for (auto arch : this->toolchain_list) {
        if (arch.int_id == id) {
            return arch.tool;
        }
    }
    return NULL;
}
CBuild::Toolchain* CBuild::CrossCompiler::get_global_config() {
    return this->meta_config;
}
void CBuild::CrossCompiler::set_default_tool(std::string arch) {
    ((CBuild::MetaToolchain*)this->meta_toolchain)->set_default_tool(arch);
}
void CBuild::CrossCompiler::add_arch_tool(std::string id, CBuild::Toolchain* tool) {
    this->toolchain_list.push_back(
        (CBuild::CrossCompiler::ToolData){.int_id = id, .tool = tool, .internally_managed = false});
    tool->set_id(this->meta_id + "-" + id);
    tool->set_name(this->meta_name);
}
void CBuild::CrossCompiler::add_arch_tool(std::string id) {
    this->toolchain_list.push_back((CBuild::CrossCompiler::ToolData){
        .int_id = id,
        .tool = new CBuild::SimpleToolchain(this->meta_id + "-" + id, this->meta_name),
        .internally_managed = true});
}
void CBuild::CrossCompiler::perform_registry() {
    for (auto t : this->toolchain_list) {
        CBuild::Registry::RegisterTarget(t.tool);
    }
    CBuild::Registry::RegisterTarget(this->meta_toolchain);
}
void CBuild::CrossCompiler::apply_global_config() {
    for (auto [id, tool, flag] : this->toolchain_list) {
        ((CBuild::ConfigToolchain*)this->meta_config)->clone_config(tool);
    }
}
