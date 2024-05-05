/**
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Task initializer
 * @date Tue May  2 08:40:12 PM EEST 2023
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
// CBuild headers
#include "../CBuild/headers/filebuff.hpp"
#include "../CBuild/headers/files.hpp"
#include "../CBuild/headers/filesystem++.hpp"
#include "../CBuild/headers/register.hpp"
#include "../CBuild/headers/task/Task.hpp"
// C++ headers
#include "fstream"
#include "iostream"
#include "string"
// Scripts headers
#include "pack.hpp"
#include "test.hpp"
#include "uploads.hpp"
#include "user_init.hpp"
// Task classes
class procces_version : public pack_base {
  private:
    std::string input = "CBuild/tmp/CBuild_defs.hpp";
    std::string output = "CBuild/headers/CBuild_defs.hpp";

  public:
    procces_version() : pack_base("proccessVersion", "") {}
    void call(std::vector<std::string> args __attribute_maybe_unused__) {
        CBuild::print_full(std::string("Delete: ") +
                           std::to_string(CBuild::fs::remove(this->output)));
        CBuild::print_full(std::string("Copy: ") +
                           std::to_string(CBuild::fs::copy(this->input, this->output)));
        CBuild::fs::replace(this->output, "$VERSION_MINOR", std::to_string(this->version_minor()));
        CBuild::fs::replace(this->output, "$VERSION_MAJOR", std::to_string(this->version_major()));
        CBuild::fs::replace(this->output, "$VERSION", "\"" + this->version_str() + "\"");
    }
};
class modify_version : public pack_base {
  private:
    void print_help() {
        printf("  [help] page 0\n\t- `h` : Display this menu\n\t- `help` "
               ": Display this menu\n\t- `inc_maj` : Increment major "
               "version\n\t- `inc_major` : Increment major version\n\t- "
               "`inc_min` : Increment minor version\n\t- `inc_minor` : "
               "Increment minor version\n\t- `dec_maj` : Decrement major "
               "version\n\t- `dec_major` : Decrement major version\n\t- "
               "`dec_min` : Decrement minor version\n\t- `dec_minor` : "
               "Decrement minor version\n\t- `print` : Print version\n\t- "
               "`p` : Print version\n\t- `s` : Quit and save\n\t- `q` : Quit "
               "without saving\n");
    }
    int vmn, vmj;

  public:
    modify_version() : pack_base("ver", "") {}
    void call(std::vector<std::string> args __attribute_maybe_unused__) {
        this->vmj = this->version_major();
        this->vmn = this->version_minor();
        printf("Interactive version manipulator of CBuild\n\tVersion "
               "1.0\tMade by WolodiaM\n");
        this->print_help();
        while (true) {
            std::string cmd;
            std::cin >> cmd;
            if (cmd == std::string("s")) {
                std::ofstream v("packages/ppa/ubuntu/version");
                v << std::to_string(this->vmj) << "." << std::to_string(this->vmn) << "v";
                return;
            } else if (cmd == std::string("q")) {
                return;
            } else if (cmd == std::string("p") || cmd == std::string("print")) {
                printf("Version: %d.%d\n", this->vmj, this->vmn);
            } else if (cmd == std::string("inc_maj") || cmd == std::string("inc_major")) {
                this->vmj += 1;
                this->vmn = 0;
            } else if (cmd == std::string("inc_min") || cmd == std::string("inc_minor")) {
                this->vmn += 1;
            } else if (cmd == std::string("dec_maj") || cmd == std::string("dec_major")) {
                this->vmj -= 1;
                this->vmn = 0;
            } else if (cmd == std::string("dec_min") || cmd == std::string("dec_minor")) {
                this->vmn -= 1;
            } else if (cmd == std::string("help") || cmd == std::string("h")) {
                this->print_help();
            } else {
                printf("Illegal command\n");
            }
        }
    }
};
class mkppa : public CBuild::Task {
  public:
    mkppa() : CBuild::Task("mkppa", {}) {}
    void call(std::vector<std::string> args __attribute_maybe_unused__) {
        std::ifstream v("./packages/ppa/ubuntu/version");
        char str[10];
        v.getline(str, 10);
        v.close();
        auto version = std::string(str).substr(0, std::string(str).find('v'));
        CBuild::system("gpg --import private.pgp");
        CBuild::fs::copy("packages/deb/libcbuild.deb",
                         "packages/ppa/ubuntu/libcbuild-" + version + ".deb");
        CBuild::system("cd packages/ppa/ubuntu && dpkg-scanpackages --multiversion . > "
                       "Packages");
        CBuild::system("cd packages/ppa/ubuntu && gzip -k -f Packages");
        CBuild::system("cd packages/ppa/ubuntu && apt-ftparchive release . > Release");
        CBuild::system("cd packages/ppa/ubuntu && gpg --default-key "
                       "\"w_melnyk@outlook.com\" -abs -o - Release > Release.gpg");
        CBuild::system("cd packages/ppa/ubuntu && gpg --default-key "
                       "\"w_melnyk@outlook.com\" --clearsign -o - Release > "
                       "InRelease");
    }
};
class procces_help : public pack_base {
  private:
    std::string input = "CBuild/tmp/CBuild_help_task.hpp";
    std::string output = "CBuild/headers/task/CBuild_help_task.hpp";

  public:
    procces_help() : pack_base("proccessHelp", "") {};
    void call(std::vector<std::string> args __attribute_maybe_unused__) {
        CBuild::line_filebuff md("usage.md");
        std::string replace =
            "CBuild help - v" + this->version_str() + "\\nCommand line options:\\n";
        bool in_while = true;
        int i = 2;
        std::string line;
        while (in_while) {
            line = md.get_line(i);
            if (line.at(0) == '#') {
                in_while = false;
                break;
            }
            line = line.substr(2);
            line = "\\t" + line + "\\n";
            replace += line;
            i++;
        }
        i += 2;
        in_while = true;
        replace += "CBuild folder structure: \\n";
        while (in_while) {
            line = md.get_line(i);
            if (line.at(0) == '`') {
                in_while = false;
                break;
            }
            line += "\\n";
            replace += line;
            i++;
        }
        i += 2;
        in_while = true;
        replace += "Some useful information: \\n";
        while (in_while) {
            try {
                line = md.get_line(i);
            } catch (std::exception& e) {
                in_while = false;
                break;
            }
            if (line.at(0) == '#') {
                replace += "    " + line.substr(3) + ": \\n";
            } else {
                replace += "\\t - " + line + "\\n";
            }
            i++;
        }
        // CBuild::print(replace);
        CBuild::fs::remove(this->output);
        CBuild::fs::copy(this->input, this->output);
        CBuild::fs::replace(this->output, "@HELP_MSG@", replace);
    }
};
class bhelp : public CBuild::Task {
  public:
    bhelp() : CBuild::Task("help", {}) {};
    void call(std::vector<std::string> args __attribute_maybe_unused__) {
        CBuild::print("CBuild build help");
        CBuild::print("\t`-t pack_deb` - pack a .deb");
        CBuild::print("\t`-t pack_deb -a no-changelog` - pack deb but not "
                      "update changelog");
        CBuild::print("\t`-t mkppa` - add deb to ppa");
        CBuild::print("\t`-t ver` - interactive version editor");
        CBuild::print("\t`-t proccessVersion` - preprocess version fields in "
                      "CBuild_defs.hpp");
        CBuild::print("\t`-t proccessHelp` - preprocess help massage in "
                      "CBuild_help_task.hpp");
        CBuild::print("\t`-t test` - not used test task");
        CBuild::print("\t`-t copyLib` - Copy .so file to proper dir from output dir");
        CBuild::print("\t`-cp` - call `-t copyLib`");
        CBuild::print("\t`--build-help` - this message");
        CBuild::print("\t`-t upload-doxygen` - upload doxygen-generated html to web");
        CBuild::print("\t`-t upload-wiki` - upload CBuild wiki to web");
        CBuild::print("\t`-t upload-ppa` - upload ubuntu ppa repo to web");
        CBuild::print("\t`-t create_init_script` - preprocess project init script");
        CBuild::print("libCBuild toolchain id - `cbuild`");
    }
};
class create_temlate_init : public CBuild::Task {
  public:
    create_temlate_init() : CBuild::Task("create_init_script", {}) {};
    void call(std::vector<std::string> args __attribute_maybe_unused__) {
        CBuild::fs::remove("sh/project_init.sh");
        CBuild::fs::copy("sh/project_init.sh.temlate", "sh/project_init.sh");
        std::fstream f("template/scripts/main.cpp");
        f.seekg(0, std::ios::end);
        std::streampos fileSize = f.tellg();
        f.seekg(0, std::ios::beg);
        std::string fileContents;
        fileContents.resize(static_cast<std::size_t>(fileSize));
        f.read(&fileContents[0], fileSize);
        f.close();
        std::string replacement = "\\n";
        size_t found = fileContents.find('\n');
        while (found != std::string::npos) {
            fileContents.replace(found, 1, replacement);
            found = fileContents.find('\n', found + replacement.length());
        }
        replacement = "\\\"";
        found = fileContents.find('"');
        while (found != std::string::npos) {
            fileContents.replace(found, 1, replacement);
            found = fileContents.find('\"', found + replacement.length());
        }
        CBuild::fs::replace("sh/project_init.sh", "#MAIN.CPP", fileContents);
    }
};
class create_arch : public CBuild::Task {
  public:
    create_arch() : CBuild::Task("create-arch", {}) {};
    void call(std::vector<std::string> args __attribute_maybe_unused__) {
        CBuild::fs::remove("./doxygen/latest.tar.gz");
        std::string cmd = "tar -czvf ";
        cmd += "./doxygen/latest.tar.gz";
        cmd += " -C ";
        cmd += "./packages/deb/libcbuild/usr/";
        cmd += " -P bin include lib share";
        CBuild::system(cmd);
    }
};
// Tasks
pack_deb packd;
procces_version pv;
procces_help ph;
modify_version mv;
mkppa ppa;
test t;
bhelp help;
create_temlate_init init_template;
create_arch ca;
upload upl_dox("upload-doxygen", "doxygen/upload-doxygen.sh");
upload upl_wiki("upload-wiki", "doxygen/upload-wiki.sh");
upload upl_ppa("upload-ppa", "doxygen/upload-ppa.sh");
upload upl_arch("upload-arch", "doxygen/upload-latest.sh");
// Init
void load_tasks() {
    CBuild::Registry::RegisterTask(&packd);
    CBuild::Registry::RegisterTask(&pv);
    CBuild::Registry::RegisterTask(&ph);
    CBuild::Registry::RegisterTask(&mv);
    CBuild::Registry::RegisterTask(&ppa);
    CBuild::Registry::RegisterTask(&help);
    CBuild::Registry::RegisterTask(&t);
    CBuild::Registry::RegisterTask(&upl_dox);
    CBuild::Registry::RegisterTask(&upl_wiki);
    CBuild::Registry::RegisterTask(&upl_ppa);
    CBuild::Registry::RegisterTask(&upl_arch);
    CBuild::Registry::RegisterTask(&init_template);
    CBuild::Registry::RegisterTask(&ca);
    CBuild::Registry::RegisterKeyword("--build-help", &help);
}
