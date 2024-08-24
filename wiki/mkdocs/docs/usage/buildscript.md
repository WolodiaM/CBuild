## Bildscripts

---

&nbsp;

## Basic buildscript

Create directory `scripts`.  
Create file `script/main.cpp`. This file implements runner for CBuild:

```cpp
/**
 * <doxygen file header and license header>
 */
// C++ libraries
#include "filesystem"
#include "stdlib.h"
#include "string"
#include "vector"
// CBuild headers
#include "CBuild/CBuild.hpp"
// Userspace headers
#include "user_init.hpp"
// Main function
int main(int argc, char **argv, char **envp) {
    // Run user init
    init();
    // Hold parsed command line arguments, see CBuild::parse in CBuild.cpp for
    // more details
    lib::map<std::string, std::string> args;
    // Parse arguments, and also get type of run
    CBuild::RType mode = CBuild::parse(&args, argc, argv);
    // We have some error
    if (mode == CBuild::ERROR)
        exit(0xFF);
    // Add base path
    args.push_back("curr_path",
                    std::string(std::filesystem::current_path().c_str()));
    // Run main loop of CBuild (execute given toolchain / module and exit)
    CBuild::loop(mode, &args);
    // Safe exit without errors
    return 0;
}
```

Create file `scripts/user_init.hpp`. This file works like a glue for other files:

```cpp
/**
 * <doxygen file header and license header>
 */
#ifndef __CBUILD_USER_INIT_HPP__
#define __CBUILD_USER_INIT_HPP__
void init();
#endif // __CBUILD_USER_INIT_HPP__
```
Create file `script/user_init.cpp`. This file implements actual buildscript:
```cpp
/**
 * <doxygen file header and license header>
 */
// C++ libraries
#include "stdio.h"
// Userspace headers
#include "user_init.hpp"
// CBuild headers
#include "CBuild/build/g++.hpp"
#include "CBuild/register.hpp"

CBuild::GXX app("app");

void init() {
    app.set_standart("c++20");
    app.add_file("src/main.cpp");
    app.warn();
    app.set_type(CBuild::EXECUTABLE);
    CBuild::Registry::RegisterTarget(&app);
}
```

If you need you could add as many additional file to your buildscript as you want. By default all `.cpp` files in directory `scripts` will be compiled and linked into buildscript. If you want to nest directories or add external dependencies you could use [`CBuild::Toolchain* CBuild::Registry::GetRebuildTarget()`](../../doxygen/namespaceCBuild_1_1Registry.html#a9125d6318f537f03046f88a935684729) to get CBuild target that is preconfigured and preregister to rebuild your buildscript.

---

&nbsp;

## Tips and Tricks

### File preprocessing

CBuild provides very simple file preprocessor that could be usen to substitude strings from file with provided. You can use with this function: [`int CBuild::fs::replace(std::string file, std::string token, std::string data)`](../../doxygen/namespaceCBuild_1_1fs.html#a66838af175563ee1fbadaa29b10aa706)

### Cross-compilation

There are two aproaches to cross-compilation in CBuild:

 * Creating multiple toolchains with different names preconfigured to build your project for different targets.
 * Hardcoding target in buildscript.
 * Runtime target selection.
 * Use built-in cross-compiler

First is the easiest and in most cases good enough if you need to simply build other targets as your *clean*, *release* build. Third variant is the best if you need to frequently rebuild for different targets.

#### Multiple toolchains

This is very simple - simply create multiple toolchains and you are done. You could use some macroses like this to properly configure some default for yevery toolchain:

```cpp
#define CONFIGURE_TOOLCHAIN(toolchain) \
    toolchain.set_standart("c++20");
    toolchain.warn();
    toolchain.set_type(CBuild::EXECUTABLE);
```

#### Target hardcoding

For this you need to extend toolchain class with something like this:

```cpp
/**
 * <doxygen file header and license header>
 */
#include "CBuild/build/Build.hpp"
#ifndef __CROSS_COMPILER__
#define __CROSS_COMPILER__
typedef enum {
    X86_64,
    ARM64
} Target;
class cross_compiler : CBuild::Toolchain {
    protected:
        Target target;
    public:
        void build() override {
            if (this->target == Target::X86_64) {
                // Things specific to x86_64
            } else if (this->target == Target::ARM64) {
                // Things specific to arm
            }
            // Other part of build
        }
        void link() override {
            if (this->target == Target::X86_64) {
                // Things specific to x86_64
            } else if (this->target == Target::ARM64) {
                // Things specific to arm
            }
            // Other part of link
        }
        void set_target(Target target) {
            this->target = target;
            if (this->target == Target::X86_64) {
                this->compiler = "g++-x86_64";
                this->linker = "g++-x86_64";
                this->packer = "ar-x86_64 cr";
            } else if (this->target = Target::ARM64) {
                this->compiler = "g++-arm64";
                this->linker = "g++-arm64";
                this->packer = "ar-arm64 cr";
            }
        }
        // Other function that you need to overload ...
};
#endif // __CROSS_COMPILER__
```

#### Runtime target selection

For this you need to extend toolchain class with something like this:

```cpp
/**
 * <doxygen file header and license header>
 */
// We use gcc toolchain as our base
#include "CBuild/build/Build.hpp"
#ifndef __CROSS_COMPILER__
#define __CROSS_COMPILER__
typedef enum {
    X86_64,
    ARM64
} Target;
class cross_compiler : CBuild::Toolchain {
    protected:
        Target target;
    public:
        void pre_build() override {
            this->target = Target:X86_64;
            for (auto elem : (*(this->args))) {
                if(elem == std::string("target:x86_64")) {
                    this->target = Target::X86_64;
                    this->compiler = "g++-x86_64";
                    this->linker = "g++-x86_64";
                    this->packer = "ar-x86_64 cr"
                } else if (elem == std::string("target:arm64")) {
                    this->target = Target::ARM64;
                    this->compiler = "g++-arm64";
                    this->linker = "g++-arm64";
                    this->packer = "ar-arm64 cr";
                }
            }
        }
        void build() override {
            if (this->target == Target::X86_64) {
                // Things specific to x86_64
            } else if (this->target == Target::ARM64) {
                // Things specific to arm
            }
            // Other part of build
        }
        void link() override {
            if (this->target == Target::X86_64) {
                // Things specific to x86_64
            } else if (this->target == Target::ARM64) {
                // Things specific to arm
            }
            // Other part of link
        }
        // Other function that you need to overload ...
};
#endif // __CROSS_COMPILER__
```

#### Buildint cross-compiler

Simply use [`CBuild::CrossCompiler`](../../doxygen/classCBuild_1_1CrossCompiler.html) class. You register tagets for each OS/architecture. Then on each rebuild it will ask you which internal target need to be run. If you selected default toolchain using [`CBuild::CrossCompiler::set_default_target(std::string arch)`](../../doxygen/classCBuild_1_1CrossCompiler.html#af5b3fe82018a8523414ffa803ef94bfb) then you can pass `-a select` to enter target select screen.

## If you buildscript binary is broken

This is two step process. Remove all references to external libs from buildscript code. Or better comment out everything except configuring you buildscript's toolchain. (This step can be skipped if you did not modify buildscript's compilation proccess. Then you can rebuild it using this command:

```sh
CBuild_rebuild scripts/ "-Ipath/to/CBuild/headers/if/they/are/not/in/your/search/path -std=c++20" "-lstdc++ -lCBuild -Wl,-z,origin -Wl,-rpath,\$ORIGIN/CBuild/CBuild -Lpath/to/libCBuild.so/if/it/is/not/you/search/path" CBuild.run
```

Then restore your buildscript and run simply rebuild it using builtin `--rebuild` flag.

