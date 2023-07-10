/**
 * @file Build.cpp
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Toolchain class implementation
 * @version 1.0
 * @date 2023-02-01
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
// C++ librarries
#include "../../headers/map.hpp"
#include "fstream"
#include "iostream"
#include "stdlib.h"
#include "string"
#include "vector"
// Project headers
#include "../../headers/CBuild_defs.hpp"
#include "../../headers/build/Build.hpp"
#include "../../headers/filesystem++.hpp"
#include "../../headers/hash.hpp"
#include "../../headers/print.hpp"
#include "../../headers/register.hpp"
#include "../../headers/system.hpp"
/* Build.hpp - arguments */
void CBuild::Toolchain::add_compile_arg(std::string arg) {
  // Push compiler arg if it is not an empty string
  if (arg != std::string("")) {
    this->compiler_args.push_back(arg);
  }
}
void CBuild::Toolchain::add_link_arg(std::string arg) {
  // Push linker arg if it is not an empty string
  if (arg != std::string("")) {
    this->link_args.push_back(arg);
  }
}
void CBuild::Toolchain::add_define(std::string def, std::string val) {
  // Create buffer with base of define
  std::string tmp = "-D";
  // Add define only if it is not empty
  if (def != std::string("")) {
    tmp += def;
    // If define value is something, add it
    if (val != std::string("")) {
      tmp += '=';
      tmp += val;
    }
    this->add_compile_arg(tmp);
  }
}
void CBuild::Toolchain::add_undef(std::string def) {
  // Create buffer with base of undefine
  std::string tmp = "-U";
  // Add undefine only if it is not empty
  if (def != std::string("")) {
    tmp += def;
    this->add_compile_arg(tmp);
  }
}
void CBuild::Toolchain::add_include(std::string inc) {
  // Create buffer with base of include
  std::string tmp = "-include \"";
  // Add only if incude is not empty
  if (inc != std::string("")) {
    tmp += inc;
    tmp += "\"";
    this->add_compile_arg(tmp);
  }
}
void CBuild::Toolchain::add_library_include(std::string lib) {
  // Create buffer with base of lib link
  std::string tmp = "-l";
  // Add only if lib name is not emoty
  if (lib != std::string("")) {
    tmp += lib;
    this->add_compile_arg(tmp);
    this->add_link_arg(tmp);
  }
}
void CBuild::Toolchain::add_library_dir(std::string inc, std::string lib) {
  // Create buffers with base of include and link pathes
  std::string itmp = "-I";
  std::string ltmp = "-L";
  // Only if two pathes not empty
  if ((inc != std::string("")) && (lib != std::string(""))) {
    itmp += inc;
    ltmp += lib;
    this->add_compile_arg(itmp);
    this->add_compile_arg(ltmp);
    this->add_link_arg(itmp);
    this->add_link_arg(ltmp);
  }
}
void CBuild::Toolchain::add_requirment(std::string   task_,
				       CBuild::stage run_stage) {
  // Add required task only if it's id is non-empty
  if (task_ != std::string("")) {
    this->required.push_back_check(task_, run_stage);
  }
}
void CBuild::Toolchain::warn() {
  // Push all arguments for normal waning (-Wall, -Wextra but
  // -Wno-comments)
  this->add_compile_arg("-Wall");
  this->add_compile_arg("-Wextra");
  this->add_compile_arg("-Wno-comments");
}
void CBuild::Toolchain::static_analizer() {
  // Push all aruments needed for static anlization of code
  this->add_compile_arg("-fanalyzer");
  this->add_compile_arg("-Wanalyzer-too-complex");
}
void CBuild::Toolchain::error() {
  // Push argument for treating warning as errors
  this->add_compile_arg("-Werror");
}
void CBuild::Toolchain::set_standart(std::string std_) {
  // Only if non empty
  if (std_ != std::string("")) {
    // Push corectly formated stadart arg
    this->add_compile_arg(std::string("-std=") + std_);
  }
}
void CBuild::Toolchain::set_type(CBuild::build_type type) {
  this->build_type = type;
}
void CBuild::Toolchain::depends_on(std::string target) {
  // Push target in list of dependencies
  this->depends.push_back(target);
}
void CBuild::Toolchain::depends_on_project(std::string path, std::string name,
					   std::string id,
					   std::string headers_path) {
  CBuild::Project_dependency dep;
  dep.headers_path = headers_path;
  dep.id	   = id;
  dep.name	   = name;
  dep.path	   = path;
  this->project_deps.push_back(dep);
}
/* Build.hpp - files */
void CBuild::Toolchain::add_file(std::string path) {
  // If path is non-empty
  if (path != std::string("")) {
    // Create new path to file
    CBuild::Path p;
    p.folder = false;
    p.path   = path;
    // Push it to list of targets for compilation
    this->targets.push_back(p);
  }
}
void CBuild::Toolchain::add_folder(std::string path) {
  // Only if path is non-empty
  if (path != std::string("")) {
    // Create new path to folder
    CBuild::Path p;
    p.folder = true;
    p.path   = path;
    // Push it to list of targets for compilation
    this->targets.push_back(p);
  }
}
/* Build.hpp - helper */
std::string CBuild::Toolchain::gen_out_file(std::string file) {
  // path to base diretory for out files
  std::string base =
      CBUILD_BUILD_DIR + "/" + this->id + "/" + CBUILD_BUILD_CACHE_DIR + "/";
  // Find dot in file name
  unsigned long pos = file.find_last_of(".");
  // Cut it out
  file = file.substr(0, pos);
  // Replace all slashes by dots
  while (file.find("/") != std::string::npos) {
    file.replace(file.find("/"), std::string("/").size(), ".");
  }
  // Add .o extension
  file += ".o";
  // Return full path
  base += file;
  return base;
}
lib::map<std::string, std::string>
CBuild::Toolchain::gen_file_list(bool force_) {
  // File - object
  lib::map<std::string, std::string> ret;
  std::vector<std::string>	     for_recomp, filelist;
  for (auto elem : this->targets) {
    if (elem.folder) {
      auto files = CBuild::fs::dir(elem.path);
      for (auto file : files) {
	filelist.push_back(CBuild::fs::normalize_path(file));
      }
    } else {
      filelist.push_back(CBuild::fs::normalize_path(elem.path));
    }
  }
  for_recomp = CBuild::get_files(filelist, this->id);
  if (this->force) {
    for_recomp = filelist;
  }
  // Not force
  // if (!force_) {
  // 	// For every target path
  // 	for (auto elem : this->targets) {
  // 		// If folder
  // 		if (elem.folder) {
  // 			// For every file in folder
  // 			std::vector<std::string> files =
  // 			    CBuild::fs::dir(elem.path);
  // 			for (auto file : files) {
  // 				// Check if hash file exists
  // 				if (!CBuild::fs::exists(
  // 					this->gen_hash_file(file))) {
  // 					CBuild::fs::create(
  // 					    {this->gen_hash_file(file)},
  // 					    CBuild::fs::FILE);
  // 					std::ofstream f(
  // 					    this->gen_hash_file(file));
  // 					f << "-";
  // 					f.close();
  // 				}
  // 				// Push file only if hash is different
  // 				if (!CBuild::load_hash(
  // 					this->gen_hash_file(file),
  // 					file)) {
  // 					ret.push_back(
  // 					    this->cmd_str(file),
  // 					    this->cmd_str(
  // 						this->gen_out_file(
  // 						    file)));
  // 				}
  // 			}
  // 		}
  // 		// If file
  // 		else {
  // 			std::string file = elem.path;
  // 			// Check if hash file exists
  // 			if (!CBuild::fs::exists(
  // 				this->gen_hash_file(file))) {
  // 				CBuild::fs::create(
  // 				    {this->gen_hash_file(file)},
  // 				    CBuild::fs::FILE);
  // 				std::ofstream f(
  // 				    this->gen_hash_file(file));
  // 				f << "-";
  // 				f.close();
  // 			}
  // 			// Push file only if hash is different
  // 			if (!CBuild::load_hash(
  // 				this->gen_hash_file(file), file)) {
  // 				ret.push_back(
  // 				    this->cmd_str(file),
  // 				    this->cmd_str(
  // 					this->gen_out_file(file)));
  // 			}
  // 		}
  // 	}
  // }
  // // Force
  // else {
  // 	// For every target path
  // 	for (auto elem : this->targets) {
  // 		// If folder
  // 		if (elem.folder) {
  // 			// For every file in folder
  // 			std::vector<std::string> files =
  // 			    CBuild::fs::dir(elem.path);
  // 			for (auto f : files) {
  // 				// Push file
  // 				ret.push_back(
  // 				    this->cmd_str(f),
  // 				    this->cmd_str(
  // 					this->gen_out_file(f)));
  // 				// Generate hash file
  // 				if (!CBuild::fs::exists(
  // 					this->gen_hash_file(f))) {
  // 					CBuild::fs::create(
  // 					    {this->gen_hash_file(f)},
  // 					    CBuild::fs::FILE);
  // 					std::ofstream fl(
  // 					    this->gen_hash_file(f));
  // 					fl << "-";
  // 					fl.close();
  // 				}
  // 			}
  // 		}
  // 		// If file
  // 		else {
  // 			// Push file
  // 			ret.push_back(this->cmd_str(elem.path),
  // 				      this->cmd_str(this->gen_out_file(
  // 					  elem.path)));
  // 			// Generate hash file
  // 			if (!CBuild::fs::exists(
  // 				this->gen_hash_file(elem.path))) {
  // 				CBuild::fs::create(
  // 				    {this->gen_hash_file(elem.path)},
  // 				    CBuild::fs::FILE);
  // 				std::ofstream f(
  // 				    this->gen_hash_file(elem.path));
  // 				f << "-";
  // 				f.close();
  // 			}
  // 		}
  // 	}
  // }
  for (auto elem : for_recomp) {
    try {
      ret.push_back(this->cmd_str(elem),
		    this->cmd_str(this->gen_out_file(elem)));
    } catch (std::exception &e) {
    }
  }
  CBuild::print_full("CBuild::Build::gen_file_list() - dbg: ");
  for (unsigned int i = 0; i < ret.size(); i++) {
    CBuild::print_full(ret.at(i).key);
  }
  return ret;
}
std::string CBuild::Toolchain::gen_out_name(std::string executable,
					    std::string dyn_lib,
					    std::string stat_lib) {
  // Base path
  std::string base =
      CBUILD_BUILD_DIR + "/" + this->id + "/" + CBUILD_BUILD_OUT_DIR + "/";
  // If name defined
  if (this->name != std::string("")) {
    // If this is executable add file excutable extension
    if (this->build_type == CBuild::EXECUTABLE) {
      base += this->name;
      base += executable;
    }
    // We build lib
    else {
      // Push "lib" and file name
      base += "lib";
      base += this->name;
      // Based on type push different file extensions
      if (this->build_type == CBuild::DYNAMIC_LIBRARY) {
	base += dyn_lib;
      } else {
	base += stat_lib;
      }
    }
  }
  // Else use id
  else {
    // If this is executable add file excutable extension
    if (this->build_type == CBuild::EXECUTABLE) {
      base += this->name;
      base += executable;
    }
    // We build lib
    else {
      // Push "lib" and file name
      base += "lib";
      base += this->id;
      // Based on type push different file extensions
      if (this->build_type == CBuild::DYNAMIC_LIBRARY) {
	base += dyn_lib;
      } else {
	base += stat_lib;
      }
    }
  }
  return this->cmd_str(base);
}
std::string CBuild::Toolchain::cmd_str(std::string in) {
  // Replace all spaces with backslash and placeholder
  while (in.find(" ") != std::string::npos) {
    in.replace(in.find(" "), std::string(" ").size(), "\\^");
  }
  // Replace placeholder by real space
  while (in.find("\\^") != std::string::npos) {
    in.replace(in.find("\\^"), std::string("\\^").size(), "\\ ");
  }
  return in;
}
void CBuild::Toolchain::init() {
  // Init toolchain
  if (this->id == std::string(""))
    this->id = "null";
  // Check all neeaded dirs and create it's if it doesnot exists
  if (!CBuild::fs::exists(CBUILD_BUILD_DIR)) {
    CBuild::fs::create({CBUILD_BUILD_DIR}, CBuild::fs::DIR);
  }
  if (!CBuild::fs::exists(CBUILD_BUILD_DIR + "/" + this->id)) {
    CBuild::fs::create({CBUILD_BUILD_DIR + "/" + this->id}, CBuild::fs::DIR);
  }
  if (!CBuild::fs::exists(CBUILD_BUILD_DIR + "/" + this->id + "/" +
			  CBUILD_BUILD_CACHE_DIR)) {
    CBuild::fs::create(
	{CBUILD_BUILD_DIR + "/" + this->id + "/" + CBUILD_BUILD_CACHE_DIR},
	CBuild::fs::DIR);
  }
  if (!CBuild::fs::exists(CBUILD_BUILD_DIR + "/" + this->id + "/" +
			  CBUILD_BUILD_OUT_DIR)) {
    CBuild::fs::create(
	{CBUILD_BUILD_DIR + "/" + this->id + "/" + CBUILD_BUILD_OUT_DIR},
	CBuild::fs::DIR);
  }
  if (!CBuild::fs::exists(CBUILD_BUILD_DIR + "/" + this->id + "/" +
			  CBUILD_HASH_DIR)) {
    CBuild::fs::create(
	{CBUILD_BUILD_DIR + "/" + this->id + "/" + CBUILD_HASH_DIR},
	CBuild::fs::DIR);
  }
}
void CBuild::Toolchain::load_project_deps(std::string curr_path) {
  for (auto elem : this->project_deps) {
    // Generate and execute command for build dependencies
    std::string cmd = "cd " + elem.path + " && ./CBuild.run -b " + elem.id;
    CBuild::system(cmd);
    // Copy files
    CBuild::system("cp -r " + elem.path + "/" + CBUILD_BUILD_DIR + "/" +
		   elem.id + "/" + CBUILD_BUILD_OUT_DIR + "/*.* " +
		   CBUILD_CACHE_DIR + "/" + CBUILD_PROJECT_DEPS_DIR);
    CBuild::system("cp -r " + elem.headers_path + "/ " + CBUILD_CACHE_DIR +
		   "/" + CBUILD_PROJECT_DEPS_HEADERS + "/" + elem.id);
    // Add lib in dependencies
    this->add_library_dir(CBUILD_CACHE_DIR + "/" + CBUILD_PROJECT_DEPS_HEADERS,
			  CBUILD_CACHE_DIR + "/" + CBUILD_PROJECT_DEPS_DIR);
    this->add_library_include(elem.name);
    this->add_compile_arg(" -Wl,-rpath,\"\\$ORIGIN/../../../" +
			  CBUILD_CACHE_DIR + "/" + CBUILD_PROJECT_DEPS_DIR +
			  "\"");
    this->add_link_arg(" -Wl,-rpath,\"\\$ORIGIN/../../../" + CBUILD_CACHE_DIR +
		       "/" + CBUILD_PROJECT_DEPS_DIR + "\"");
  }
}
/* Build.hpp - main */
void CBuild::Toolchain::call(std::vector<std::string> *args, bool force,
			     bool debug) {
  CBuild::print("Starting " + this->id + " toolchain in build mode ",
		CBuild::color::RED);
  // Save scratch variables
  this->args  = args;
  this->force = force;
  // Special link tag for dynamick libraries
  if (this->build_type == CBuild::DYNAMIC_LIBRARY)
    this->link_args.push_back("-shared");
  // Debug mode of compilation
  if (debug)
    this->compiler_args.push_back("-g");
  // Some standart compile and link args
  this->compiler_args.push_back("-fPIC");
  this->add_link_arg("-Wl,-z,origin");
  this->add_compile_arg("-Wl,-z,origin");
  this->add_link_arg(" -Wl,-rpath,\"\\$ORIGIN\"");
  this->add_compile_arg(" -Wl,-rpath,\"\\$ORIGIN\"");
  this->add_compile_arg("-I" + CBUILD_CACHE_DIR + "/" +
			CBUILD_PROJECT_DEPS_HEADERS);
  this->add_link_arg("-I" + CBUILD_CACHE_DIR + "/" +
		     CBUILD_PROJECT_DEPS_HEADERS);
  // For packing in deb fomat
  if (this->build_type == CBuild::DYNAMIC_LIBRARY) {
    std::string lib = this->gen_out_name();
    int		pos = lib.find_last_of('/');
    lib		    = lib.substr(pos + 1);
    this->add_link_arg(" -Wl,-soname," + lib);
    this->add_compile_arg(" -Wl,-soname," + lib);
  }
  // Init
  this->init();
  CBuild::print("Calling tasks marked as PRE ", CBuild::color::GREEN);
  // Call all dependencies (tasks) marked as PRE
  for (unsigned int i = 0; i < this->required.size(); i++) {
    auto elem = this->required.at(i);
    if (elem.data == CBuild::PRE) {
      CBuild::Registry::CallTask(elem.key, {});
    }
  }
  CBuild::print("Calling all required toolchains ", CBuild::color::GREEN);
  // Call all dependencies (toolchains)
  for (std::string id : this->depends) {
    // Call target
    auto target = CBuild::Registry::GetToolchain(id);
    if (target != NULL) {
      target->call(args, force, debug);
      // get lib name
      auto	   out_path  = target->gen_out_name();
      unsigned int end_slash = out_path.find_last_of('/');
      unsigned int end_dot   = out_path.find_last_of('.');
      std::string out = out_path.substr(end_slash + 4, end_dot - end_slash - 4);
      // Add lib include
      this->add_library_include(out);
      // Add all needed for linking to that library
      this->add_library_dir(".", CBUILD_BUILD_DIR + "/" + target->get_id() +
				     "/" + CBUILD_BUILD_OUT_DIR + "/");
      this->add_compile_arg("-Wl,-rpath,\"\\$ORIGIN/../../" + target->get_id() +
			    "/" + CBUILD_BUILD_OUT_DIR + "\"");
      this->add_link_arg("-Wl,-rpath,\"\\$ORIGIN/../../" + target->get_id() +
			 "/" + CBUILD_BUILD_OUT_DIR + "\"");
    }
  }
  // 5 steps
  if (!force)
    CBuild::print("Using precompiled object were possible ",
		  CBuild::color::MAGENTA);
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
  // Special linking for static libraries
  if (this->build_type == CBuild::STATIC_LIBRARY) {
    this->link_pack();
  } else {
    this->link();
  }
  // Last, 6-th step
  CBuild::print("Running post link tasks ", CBuild::GREEN);
  this->post_link();
  CBuild::print("Calling tasks marked as POST ", CBuild::GREEN);
  // Call all dependencies (tasks) marked as POSt
  for (unsigned int i = 0; i < this->required.size(); i++) {
    auto elem = this->required.at(i);
    if (elem.data == CBuild::POST) {
      CBuild::Registry::CallTask(elem.key, {});
    }
  }
  CBuild::print("End of execution of toolchain " + this->id + " ", CBuild::RED);
}
void CBuild::Toolchain::run(std::vector<std::string> *args) {
  CBuild::print("Starting \"" + this->name + "\" ", CBuild::RED);
  // Parse args
  std::string pargs = "";
  if (args != NULL) {
    for (auto elem : *args) {
      pargs += elem;
      pargs += " ";
    }
  }
  // Construct command
  std::string cmd;
  cmd = this->gen_out_name();
  cmd += " ";
  cmd += pargs;
  // Call app
  CBuild::print("App output (if any):", CBuild::MAGENTA);
  CBuild::system(cmd);
  CBuild::print("End of app execution", CBuild::RED);
}
void CBuild::Toolchain::debug(std::vector<std::string> *args,
			      std::vector<std::string> *pargs) {
  CBuild::print("Starting \"" + this->id + "\" toolchain in debug mode ",
		CBuild::RED);
  // Build in debug mode
  this->call(args, true, true);
  CBuild::print("Running builded app with gdb ", CBuild::GREEN);
  std::string ppargs = "";
  if (pargs != NULL) {
    for (auto elem : *pargs) {
      ppargs += elem;
      ppargs += " ";
    }
  }
  // Construct command
  std::string cmd;
  cmd = "gdb ";
  cmd += this->gen_out_name();
  cmd += " ";
  cmd += ppargs;
  // Call gdb on app
  CBuild::print("Now you can see gdb shell ", CBuild::MAGENTA);
  CBuild::system(cmd);
  CBuild::print("End of app execution", CBuild::RED);
}
void CBuild::Toolchain::clear() {
  // Symple clear though shell ;)
  CBuild::system(std::string("rm -r ") + CBUILD_BUILD_DIR + "/" + this->id);
}
