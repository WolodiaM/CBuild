/**
 * @file mingw-gcc.hpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief mingw-gcc toolchain realization
 * @version 1.0
 * @date 2023-02-03
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
// Project files
#include "Build.hpp"
#include "CBuild/CBuild_defs.hpp"
#include "CBuild/print.hpp"
#include "CBuild/register.hpp"
#include "CBuild/filesystem++.hpp"
#include "CBuild/hash.hpp"
#include "CBuild/system.hpp"
// Code
#ifndef _CBUILD_MINGW_GCC_TOOLCHAIN
#define _CBUILD_MINGW_GCC_TOOLCHAIN
namespace CBuild
{
    class MINGW_GCC : public CBuild::Toolchain
    {
    public:
        /**
         * @brief Construct a new MINGW_GCC object
         *
         * @param id Id
         */
        MINGW_GCC(std::string id)
        {
            this->id = id;
            this->linker = "x86_64-w64-mingw32-gcc";
            this->compiler = "x86_64-w64-mingw32-gcc";
            this->packer = "x86_64-w64-mingw32-ar cr";
        }
        /**
         * @brief Construct a new MINGW_GCC object
         *
         * @param id Id
         * @param name Name
         */
        MINGW_GCC(std::string id, std::string name)
        {
            this->id = id;
            this->name = name;
            this->linker = "x86_64-w64-mingw32-gcc";
            this->compiler = "x86_64-w64-mingw32-gcc";
            this->packer = "x86_64-w64-mingw32-ar cr";
        }

    protected:
        // For docs see mingw-g++.hpp
        void build()
        {
            std::string args;
            for (auto elem : this->compiler_args)
            {
                args += elem;
                args += " ";
            }
            auto files = this->gen_file_list(this->force);
            std::vector<std::string> hash_files;
            if (files.size() > 0)
            {
                for (unsigned int i = 0; i < files.size(); i++)
                {
                    std::string cmd = this->compiler + " -c ";
                    cmd += files.at(i).key;
                    cmd += " ";
                    cmd += args;
                    cmd += " -o ";
                    cmd += files.at(i).data;
                    // CBuild::print(cmd, CBuild::BLUE);
                    CBuild::system(cmd);
                    hash_files.push_back(this->gen_hash_file(files.at(i).key));
                }
            }
            CBuild::save_hashes(hash_files, files.keys());
        }
        void link()
        {
            std::string args;
            for (auto elem : this->link_args)
            {
                args += elem;
                args += " ";
            }
            auto files = this->gen_file_list(true);
            std::string flist;
            for (unsigned int i = 0; i < files.size(); i++)
            {
                flist += files.at(i).data;
                flist += " ";
            }
            if (files.size() > 0)
            {
                std::string cmd = this->linker + " ";
                cmd += flist;
                cmd += " ";
                cmd += args;
                cmd += " ";
                cmd += " -o ";
                cmd += this->gen_out_name();
                // CBuild::print(cmd, CBuild::BLUE);
                CBuild::system(cmd);
            }
        }
        void link_pack()
        {
            std::string args;
            for (auto elem : this->link_args)
            {
                args += elem;
                args += " ";
            }
            auto files = this->gen_file_list(true);
            std::string flist;
            for (unsigned int i = 0; i < files.size(); i++)
            {
                flist += files.at(i).data;
                flist += " ";
            }
            if (files.size() > 0)
            {
                std::string cmd = this->packer + " ";
                cmd += this->gen_out_name();
                cmd += " ";
                cmd += flist;
                cmd += " ";
                // CBuild::print(cmd, CBuild::BLUE);
                CBuild::system(cmd.c_str());
            }
        }
        void post_link()
        {
            for (std::string id : this->depends)
            {
                auto target = CBuild::Registry::GetToolchain(id, true);
                if (target != NULL)
                {
                    auto out_path = target->gen_out_name(".exe", ".dll");
                    unsigned int end_slash = out_path.find_last_of('/');
                    std::string out = CBUILD_BUILD_DIR + "/" + this->id + "/" + CBUILD_BUILD_OUT_DIR + "/" + out_path.substr(end_slash + 1);
                    CBuild::fs::copy(out_path, out);
                }
                
            }
        }
    public:
        std::string gen_out_name(std::string executable = ".exe", std::string dyn_lib = ".dll", std::string stat_lib = ".a")
        {
            std::string base = CBUILD_BUILD_DIR + "/" + this->id + "/" + CBUILD_BUILD_OUT_DIR + "/";
            if (this->name != std::string(""))
            {
                if (this->build_type == CBuild::EXECUTABLE)
                {
                    base += this->name;
                    base += executable;
                }
                else
                {
                    base += "lib";
                    base += this->name;
                    if (this->build_type == CBuild::DYNAMIC_LIBRARY)
                    {
                        base += dyn_lib;
                    }
                    else
                    {
                        base += stat_lib;
                    }
                }
            }
            else
            {
                if (this->build_type == CBuild::EXECUTABLE)
                {
                    base += this->name;
                    base += executable;
                }
                else
                {
                    base += "lib";
                    base += this->id;
                    if (this->build_type == CBuild::DYNAMIC_LIBRARY)
                    {
                        base += dyn_lib;
                    }
                    else
                    {
                        base += stat_lib;
                    }
                }
            }
            return this->cmd_str(base);
        }
        void call(std::vector<std::string> *args, bool force = false, bool debug = false)
        {
            CBuild::print("Starting " + this->id + " toolchain in build mode ", CBuild::color::RED);
            this->args = args;
            this->force = force;
            if (this->build_type == CBuild::DYNAMIC_LIBRARY)
                this->link_args.push_back("-shared");
            if (debug)
                this->compiler_args.push_back("-g");
            this->compiler_args.push_back("-fPIC");
            this->add_link_arg("-static-libgcc");
            this->add_link_arg("-static-libstdc++");
            this->init();
            CBuild::print("Calling tasks marked as PRE ", CBuild::color::GREEN);
            for (unsigned int i = 0; i < this->required.size(); i++)
            {
                auto elem = this->required.at(i);
                if (elem.data == CBuild::PRE)
                {
                    CBuild::Registry::CallTask(elem.key, {});
                }
            }
            CBuild::print("Calling all required toolchains ", CBuild::color::GREEN);
            for (std::string id : this->depends)
            {

                auto target = CBuild::Registry::GetToolchain(id);
                if (target != NULL)
                {
                    target->call(args, force, debug);
                    auto out_path = target->gen_out_name();
                    unsigned int end_slash = out_path.find_last_of('/');
                    unsigned int end_dot = out_path.find_last_of('.');
                    std::string out = out_path.substr(end_slash + 4, end_dot - end_slash - 4);
                    this->add_library_include(out);
                    this->add_library_dir(".", CBUILD_BUILD_DIR + "/" + target->get_id() + "/" + CBUILD_BUILD_OUT_DIR + "/");
                }
            }
            if (!force)
                CBuild::print("Using precompiled object were possible ", CBuild::color::MAGENTA);
            CBuild::print("Running pre build tasks ", CBuild::GREEN);
            this->pre_build();
            CBuild::print("Running build tasks ", CBuild::GREEN);
            CBuild::print("Now you can see compiler output", CBuild::MAGENTA);
            this->build();
            CBuild::print("Running post build tasks ", CBuild::GREEN);
            this->post_build();
            CBuild::print("Running pre link tasks ", CBuild::GREEN);
            this->pre_link();
            CBuild::print("Running link tasks ", CBuild::GREEN);
            CBuild::print("Now you can see linker output", CBuild::MAGENTA);
            if (this->build_type == CBuild::STATIC_LIBRARY)
            {
                this->link_pack();
            }
            else
            {
                this->link();
            }
            CBuild::print("Running post link tasks ", CBuild::GREEN);
            this->post_link();
            CBuild::print("Calling tasks marked as POST ", CBuild::GREEN);
            for (unsigned int i = 0; i < this->required.size(); i++)
            {
                auto elem = this->required.at(i);
                if (elem.data == CBuild::POST)
                {
                    CBuild::Registry::CallTask(elem.key, {});
                }
            }
            CBuild::print("End of execution of toolchain " + this->id + " ", CBuild::RED);
        }
        void run(std::vector<std::string> *args)
        {
            CBuild::print("Starting \"" + this->name + "\" ", CBuild::RED);
            std::string pargs = "";
            if (args != NULL)
            {
                for (auto elem : *args)
                {
                    pargs += elem;
                    pargs += " ";
                }
            }
            std::string cmd;
            cmd = "wine64 ";
            cmd += this->gen_out_name();
            cmd += " ";
            cmd += pargs;
            CBuild::print("App output (if any):", CBuild::MAGENTA);
            CBuild::system(cmd);
            CBuild::print("End of app execution", CBuild::RED);
        }
        void debug(std::vector<std::string> *args, std::vector<std::string> *pargs)
        {
            CBuild::print("It is unimplemented for now!", CBuild::RED);
            // CBuild::print("Starting \"" + this->id + "\" toolchain in debug mode ", CBuild::RED);
            // this->call(args, true, true);
            // CBuild::print("Running builded app with gdb ", CBuild::GREEN);
            // std::string ppargs = "";
            // if (pargs != NULL)
            // {
            //     for (auto elem : *pargs)
            //     {
            //         ppargs += elem;
            //         ppargs += " ";
            //     }
            // }
            // std::string cmd;
            // cmd = "x86_64-w64-mingw32-gdb ";
            // cmd += this->gen_out_name();
            // cmd += " ";
            // cmd += ppargs;
            // // CBuild::print("Now you can see gdb shell ", CBuild::MAGENTA);
            // CBuild::system(cmd);
            // CBuild::print("End of app execution", CBuild::RED);
        }
    };
} // namespace CBuild
#endif // _CBUILD_MINGW_GCC_TOOLCHAIN