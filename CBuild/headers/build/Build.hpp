/**
 * @file Build.hpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Build toolchain headers
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
#ifndef __CBUILD_TOOLCHAIN_HPP__
#define __CBUILD_TOOLCHAIN_HPP__
// C++ libraries
#include "../map.hpp"
#include "array"
#include "string"
#include "vector"
// Project headers
#include "../generator/generator.hpp"
#include "../hasher/hasher.hpp"
#include "../hasher/manual_hash.hpp"
// Code
namespace CBuild {
/**
 * @brief Path datatype
 */
typedef struct {
    std::string path;
    bool folder;
} Path;
/**
 * @brief Project dependency data
 */
typedef struct {
    std::string path;
    std::string id;
    std::string name;
    std::string headers_path;
} Project_dependency;
/**
 * @brief Type of depencesies for toolchain
 */
typedef enum { PRE, POST } stage;
typedef enum { EXECUTABLE, STATIC_LIBRARY, DYNAMIC_LIBRARY } build_type;
template <class T>
concept HashImpl = std::is_base_of<CBuild::Hash, T>::value;
/**
 * @brief Toolchain class
 */
class Toolchain {
  protected:
    /**
     * @brief Name of output, if undefined (""), for output name used id
     */
    std::string name = "";
    /**
     * @brief Must be initialized in derived classes
     *
     */
    std::string id;
    /**
     * @brief Save task id with run stage sprecified for this task
     */
    lib::map<std::string, CBuild::stage> required;
    /**
     * @brief All other targets, that is needed for this target
     */
    std::vector<std::string> depends;
    /**
     * @brief Target files or folders
     */
    std::vector<CBuild::Path> targets;
    /**
     * @brief Compiler args (specified and generated)
     */
    std::vector<std::string> compiler_args;
    /**
     * @brief Linker args (specified and generated)
     */
    std::vector<std::string> link_args;
    /**
     * @brief Compiler command
     */
    std::string compiler;
    /**
     * @brief Linker command
     */
    std::string linker;
    /**
     * @brief ar command for packing static libraries
     */
    std::string packer;
    /**
     * @brief Type of build
     */
    CBuild::build_type build_type;
    /**
     * @brief Project dependencies
     */
    std::vector<CBuild::Project_dependency> project_deps;
    /**
     * @brief We do dummy compilation
     */
    bool dummy = false;
    /**
     * @brief Major version, can be necesary for shared libraries
     */
    int version_major = -1;
    /**
     * @brief gen_out_name() dont prepend base path
     */
    bool gen_out_name_without_base_path = false;
    /**
     * @brief Generate file list for linking - force and no changes to hashes
     */
    bool gen_file_list_for_linking = false;
    /**
     * @brief Libs that need to be searched in pkg-config
     */
    std::vector<std::string> pkg_config_include_entries;

  protected:
    // Scratch variables
    /**
     * @brief Argument pointer (scratch variable)
     */
    std::vector<std::string>* args;
    /**
     * @brief Force argument (scratch variable)
     */
    bool force;
    /**
     * @brief Hasher used here
     */
    Hash* hasher;
    /**
     * @brief Lock that specifies that stdargs() function need to do nothing
     */
    bool stdargs_lock = false;

  protected:
    /* Build stages */
    /**
     * @brief Before build
     */
    virtual void pre_build(){};
    /**
     * @brief After build
     */
    virtual void post_build(){};
    /**
     * @brief Before linking
     */
    virtual void pre_link(){};
    /**
     * @brief After linking
     */
    virtual void post_link(){};
    /**
     * @brief Build
     */
    virtual void build() = 0;
    /**
     * @brief Linking
     */
    virtual void link() = 0;
    /**
     * @brief Linking for static libraries
     */
    virtual void link_pack() = 0;
    /* Helper */
    /**
     * @brief Generate list of files that need to be compiles
     *
     * @param force => bool -> Do we do force compilation
     * @return lib::map<std::string, std::string> -> list of files, key - code,
     * value - object
     */
    virtual lib::map<std::string, std::string> gen_file_list(bool force);
    /**
     * @brief Generate output object file from input file
     *
     * @param file => std::string -> input file
     * @return std::string -> file path
     */
    virtual std::string gen_out_file(std::string file);

  public:
    /**
     * @brief Generate output file name (after linking)
     *
     * @return std::string -> File name
     */
    virtual std::string gen_out_name(std::string executable = ".run", std::string dyn_lib = ".so",
                                     std::string stat_lib = ".a");
    /**
     * @brief Load all project dependencies
     *
     * @param curr_path => std::string -> current path
     */
    virtual void load_project_deps(std::string curr_path);

  protected:
    /**
     * @brief Replace spaces by "\ "
     *
     * @param in => std::string -> input string
     * @return std::string Returned string
     */
    virtual std::string cmd_str(std::string in);
    /**
     * @brief Initialize folder structure
     */
    virtual void init();

  public:
    /**
     * @brief Add standard compile and link arguments into compilation
     * Need to be publick for some hashers to work
     */
    virtual void stdargs();

  public:
    Toolchain() {}
    virtual ~Toolchain() {}
    std::string get_id() {
        return this->id;
    }
    /**
     * @brief Call tollchain to execute
     *
     * @param args => std::vector<std::string>* -> argument for call,
     * pointer, nonull
     * @param force => bool -> force argument, used internally
     * @param debug => bool -> does we run in build mode
     * @param dummy => bool -> we dont build - we run this to evalueate all
     * things
     */
    virtual void call(std::vector<std::string>* args, bool force = false, bool debug = false,
                      bool dummy = false);
    /**
     * @brief Run builded app
     *
     * @param args => std::vector<std::string>* -> program arguments
     */
    virtual void run(std::vector<std::string>* args);
    /**
     * @brief Build program in debug mode and after run gdb on it
     *
     * @param args => std::vector<std::string>* -> arguments for build
     * @param pargs => std::vector<std::string>* -> arguments for run
     */
    virtual void debug(std::vector<std::string>* args, std::vector<std::string>* pargs);
    /**
     * @brief Clear all caches and builded app
     */
    virtual void clear();
    /**
     * @brief Add new define
     *
     * @param define => std:string -> Define name
     * @param val => std:string -> Value for define, default - ""
     */
    virtual void add_define(std::string define, std::string val = "");
    /**
     * @brief Add new undefine (undefine previvious define)
     *
     * @param define => std::string -> Define name
     */
    virtual void add_undef(std::string define);
    /**
     * @brief Add compile-time include
     *
     * @param include_path => std:string -> path for include
     */
    virtual void add_include(std::string include_path);
    /**
     * @brief Directly add compiler argument
     *
     * @param arg => std:string -> argument, directly passed to compiler
     */
    virtual void add_compile_arg(std::string arg);
    /**
     * @brief Add linker arguments
     *
     * @param arg => std:string -> argument, directly passed to compiler
     */
    virtual void add_link_arg(std::string arg);
    /**
     * @brief Add system library include, library include folder need to be
     * in PATH
     *
     * @param library => std:string -> library name, will be used as
     * -l<library>
     */
    virtual void add_library_include(std::string library);
    /**
     * @brief Add library include directory (with *.h files) and directory
     * with library *.a and *.so/*.dll files
     *
     * @param include => std:string -> include dir, will be used as
     * -I<include>
     * @param lib => std:string -> library dir, will be used as -L<lib>
     */
    virtual void add_library_dir(std::string include, std::string lib);
    /**
     * @brief Add required task, that task will be executed before
     * toolchain, it can have dependecies, but toolchain can depend on
     * multiple tasks also
     *
     * @param task_ => std:string -> task id
     * @param run_stage => CBuild::stage -> run stage of task
     */
    virtual void add_requirment(std::string task_, CBuild::stage run_stage = CBuild::PRE);
    /**
     * @brief This target depends on other target
     *
     * @param target => std::string -> Other target id
     */
    virtual void depends_on(std::string target);
    /**
     * @brief Enable more warnings
     */
    virtual void warn();
    /**
     * @brief Enable static anlizer (very slow)
     */
    virtual void static_analizer();
    /**
     * @brief Add -Werror
     */
    virtual void error();
    /**
     * @brief Add `-pthread` argument
     */
    virtual void multithreaded_target();
    /**
     * @brief Set standart for code (-std=std_)
     *
     * @param std_ => std::string -> Stadart name
     */
    virtual void set_standart(std::string std_);
    /**
     * @brief Add file for compilation
     *
     * @param path => std::string -> Path to file
     */
    virtual void add_file(std::string path);
    /**
     * @brief Add folder to compile path, scan is not recursive
     *
     * @param path => std::string -> Path to folder
     */
    virtual void add_folder(std::string path);
    /**
     * @brief Set the type of build
     *
     * @param type Build type
     */
    virtual void set_type(CBuild::build_type type);
    /**
     * @brief Add another CBuild project as dependency
     *
     * @param path => std::string -> Relative (from CBuild.run) path to
     * needed toolchain
     * @param name => std::string -> name of build target (for lib linking)
     * @param id => std::string -> Id of needed toolchain for calling target
     * build
     * @param headers_path => std::string -> path to headers dir
     */
    virtual void depends_on_project(std::string path, std::string name, std::string id,
                                    std::string headers_path);
    /**
     * @brief Enable `-pg` for gprof
     */
    virtual void set_profiling_mode();
    /**
     * @brief Set major version component, used in `-Wl,-soname`
     *
     * @param version => int -> Major version
     */
    virtual void set_version_major(int version);
    /**
     * @brief Get all three commands from class - compile, linker and packer
     * @return std::array<std::string, 3> -> Compiler,
     * linker, packer
     */
    virtual std::array<std::string, 3> get_cmds();
    /**
     * @brief Generate list of all files, that is tracked by this target
     *
     * @return lib::map<std::string, std::string> -> List of files
     */
    virtual lib::map<std::string, std::string> gen_file_list_force();
    /**
     * @brief Get preprocessed compile args
     */
    virtual std::string get_compile_args();
    /**
     * @brief Crash compilation, needed when gcc throws error
     */
    virtual void crash() __attribute__((__noreturn__));
    /**
     * @brief Call compiler
     *
     * @param cmd => std::string -> Shell command
     */
    virtual void compile(std::string cmd);
    /**
     * @brief Add a lib, data for what need to be searched using pkg-config tool
     *
     * @param id => std::string -> Package id in a pkg-config database
     */
    virtual void add_pkgconfig_entry(std::string id);
    /**
     * @brief Set used hasher for this target
     *
     * @param hasher => Hash* -> Hasher that need to be used
     */
    virtual void set_hasher(Hash* hasher);
    /**
     * @brief Change out file name
     *
     * @param name => std::string -> Out name
     */
    virtual void set_name(std::string name);
};
} // namespace CBuild
#endif // __CBUILD_TOOLCHAIN_HPP__
