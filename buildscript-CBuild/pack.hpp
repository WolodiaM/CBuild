/**
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Tasks for packing diffrent OS packets
 * @date Wed May  3 12:50:35 PM EEST 2023
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
// C++ libraries
#include "fstream"
#include "iomanip"
#include "iostream"
#include "sstream"
#include "string"
#include "time.h"
// CBuild headers
#include "../CBuild/headers/filebuff.hpp"
#include "../CBuild/headers/files.hpp"
#include "../CBuild/headers/filesystem++.hpp"
#include "../CBuild/headers/print.hpp"
#include "../CBuild/headers/system.hpp"
#include "../CBuild/headers/task/Task.hpp"
// Coe
#ifndef __PACK_HPP__
#define __PACK_HPP__
// Functions
std::string get_day(int day) {
  switch (day) {
  case 0:
    return std::string("Sun");
  case 1:
    return std::string("Mon");
  case 2:
    return std::string("Tue");
  case 3:
    return std::string("Wed");
  case 4:
    return std::string("Thu");
  case 5:
    return std::string("Fri");
  case 6:
    return std::string("Sat");
  }
  return std::string("");
}
std::string get_mon(int mon) {
  switch (mon) {
  case 0:
    return std::string("Jan");
  case 1:
    return std::string("Feb");
  case 2:
    return std::string("Mar");
  case 3:
    return std::string("Apr");
  case 4:
    return std::string("May");
  case 5:
    return std::string("Jun");
  case 6:
    return std::string("Jul");
  case 7:
    return std::string("Aug");
  case 8:
    return std::string("Sep");
  case 9:
    return std::string("Oct");
  case 10:
    return std::string("Nov");
  case 11:
    return std::string("Dec");
  }
  return std::string("");
}
// Classes
class pack_base : public CBuild::Task {
protected:
  std::string work_folder;
  std::string version_str() {
    std::ifstream version("./ppa/ubuntu/version");
    char str[10];
    version.getline(str, 10);
    version.close();
    return std::string(str).substr(0, 3);
  }
  int version_major() {
    std::string vstr = this->version_str();
    return std::stoi(vstr.substr(0, 1));
  }
  int version_minor() {
    std::string vstr = this->version_str();
    return std::stoi(vstr.substr(2));
  }

public:
  pack_base(std::string name, std::string folder) : CBuild::Task(name, {}) {
    this->work_folder = folder;
  }
};
class pack_deb : public pack_base {
private:
  std::string changelog = "changelog.Debian";
  void editChangelog() {
    CBuild::line_filebuff chf(this->work_folder + this->changelog);
    std::stringstream buff;
    buff << "cbuild (";
    buff << this->version_str();
    buff << ") stable; urgency=high";
    chf.set_line(buff.str(), 0);
    chf.set_line("", 1);
    chf.set_line("  [WolodiaM]", 2);
    buff.str("");
    buff << " -- WolodiaM <w_melnyk@outlook.com>  ";
    time_t t = time(NULL);
    tm time_struct = *localtime(&t);
    buff << get_day(time_struct.tm_wday);
    buff << ", ";
    buff << std::setfill('0') << std::setw(2)
         << std::to_string(time_struct.tm_mday);
    buff << " ";
    buff << get_mon(time_struct.tm_mon);
    buff << " ";
    buff << std::to_string((1900 + time_struct.tm_year));
    buff << " ";
    buff << std::setfill('0') << std::setw(2)
         << std::to_string(time_struct.tm_hour);
    buff << ":";
    buff << std::setfill('0') << std::setw(2)
         << std::to_string(time_struct.tm_min);
    buff << ":";
    buff << std::setfill('0') << std::setw(2)
         << std::to_string(time_struct.tm_sec);
    buff << " +0200";
    std::string input = "";
    std::vector<std::string> changes;
    std::cout << "Write change log here, blank line end input:" << std::endl;
    while (true) {
      std::getline(std::cin, input);
      if (input.empty()) {
        break;
      }
      changes.push_back(input);
    }
    unsigned int i = 3;
    for (; i < changes.size() + 3; i++) {
      chf.set_line(std::string("  * ") + changes.at(i - 3), i);
    }
    chf.set_line("", i);
    chf.set_line(buff.str(), i + 1);
    chf.set_line("", i + 2);
    chf.update();
  }

public:
  pack_deb() : pack_base("pack_deb", "./deb/") {}
  void call(std::vector<std::string> args) {
    // Changelog
    if (args.size() < 1 || args.at(0) != std::string("no-changelog")) {
      this->editChangelog();
    }
    // Dirs
    CBuild::fs::create({this->work_folder + "/libcbuild"}, CBuild::fs::DIR);
    CBuild::fs::create({this->work_folder + "/libcbuild/usr"}, CBuild::fs::DIR);
    CBuild::fs::create({this->work_folder + "/libcbuild/usr/bin"},
                       CBuild::fs::DIR);
    CBuild::fs::create({this->work_folder + "/libcbuild/usr/lib"},
                       CBuild::fs::DIR);
    CBuild::fs::create({this->work_folder + "/libcbuild/usr/include"},
                       CBuild::fs::type::DIR);
    CBuild::fs::create({this->work_folder + "/libcbuild/usr/include/CBuild"},
                       CBuild::fs::DIR);
    CBuild::fs::create({this->work_folder + "/libcbuild/usr/share"},
                       CBuild::fs::DIR);
    CBuild::fs::create({this->work_folder + "/libcbuild/usr/share/doc"},
                       CBuild::fs::DIR);
    CBuild::fs::create(
        {this->work_folder + "/libcbuild/usr/share/doc/libcbuild"},
        CBuild::fs::DIR);
    CBuild::fs::create({this->work_folder + "/libcbuild/DEBIAN"},
                       CBuild::fs::DIR);

    // Rebuild script
    CBuild::fs::remove(this->work_folder + "/libcbuild/usr/bin/CBuild_rebuild");
    CBuild::fs::copy("rebuild.sh",
                     this->work_folder + "/libcbuild/usr/bin/CBuild_rebuild");
    // Lib
    CBuild::fs::remove(this->work_folder + "/libcbuild/usr/lib/libCBuild.so");
    CBuild::fs::copy("CBuild/CBuild/libCBuild.so",
                     this->work_folder + "/libcbuild/usr/lib");
    // Headers
    CBuild::system("rm -r " + this->work_folder +
                   "/libcbuild/usr/include/CBuild/*");
    CBuild::system("cp -r CBuild/headers/* " + this->work_folder +
                   "/libcbuild/usr/include/CBuild/");
    // Changelog
    CBuild::fs::copy(this->work_folder + "/changelog.Debian",
                     this->work_folder + "/changelog");
    CBuild::system("cd " + this->work_folder + " && gzip --best -n changelog");
    CBuild::fs::remove(this->work_folder +
                       "/libcbuild/usr/share/doc/libcbuild/changelog.gz");
    CBuild::fs::move(this->work_folder + "/changelog.gz",
                     this->work_folder + "/libcbuild/usr/share/doc/libcbuild/");
    // DEBIAN control
    CBuild::fs::remove(this->work_folder + "/libcbuild/DEBIAN/control");
    CBuild::fs::copy(this->work_folder + "/control",
                     this->work_folder + "/libcbuild/DEBIAN/");
    CBuild::fs::replace(this->work_folder + "/libcbuild/DEBIAN/control",
                        "$VERSION", this->version_str());
    // Some cleaning
    CBuild::system("strip " + this->work_folder +
                   "/libcbuild/usr/lib/libCBuild.so");
    CBuild::system("chmod 0644 " + this->work_folder +
                   "/libcbuild/usr/lib/libCBuild.so");
    CBuild::system("chmod 0644 " + this->work_folder +
                   "/libcbuild/usr/include/CBuild/CBuild_defs.hpp");
    CBuild::system("chmod 0644 " + this->work_folder +
                   "/libcbuild/usr/include/CBuild/task/CBuild_help_task.hpp");
    CBuild::system("chmod +x " + this->work_folder +
                   "/libcbuild/usr/bin/CBuild_rebuild");
    // Pack package
    CBuild::system("cd " + this->work_folder +
                   " && dpkg-deb --root-owner-group --build libcbuild");
    // Run lintian
    CBuild::system("lintian ./" + this->work_folder + "/libcbuild.deb");
  }
};
#endif // __PACK_HPP__
