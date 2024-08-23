# Internal CBuild structure

---

&nbsp;

## Folder structure of CBuild project

Provided  folder structure represents project with two targets that was built before and dont have any additional files.

```filelist
project-root/
    ├── build                                   // Build output directory
    │   ├── <target>
    │   │   ├── <target>.meta                   // Metadata file for specific target - compiler, output file etc
    │   │   ├── hash
    │   │   │   └── hash                        // Hash of all file (one file per line), old file, was used only before CBuild 11.0
    │   │   ├── config                          // Directory for per-file metadata - input file, output object, hash, dependencies etc
    │   │   │   └── <target>_src.main.meta
    │   │   ├── objs                            // Directory for cached object files
    │   │   │   └── <target>_src.main.o
    │   │   └── out                             // Directory for output binary
    │   │       └── <Target_name>.so
    │   └── <other_target>
    │       ├── <other_target>.meta             // Metadata file for specific target - compiler, output file etc
    │       ├── config                          // Directory for per-file metadata - input file, output object, hash, dependencies etc
    │       │   └── <other_target>_src.main.meta
    │       ├── objs                            // Directory for cached object files
    │       │   └── <other_target>_src.main.o
    │       └── out                             // Directory for output binary
    │           └── <Other_target_name>.so
    ├── cache                                   // Directory for CBuild temporary data
    │   ├── headers                             // Directory for headers of external dependencies
    │   ├── libs                                // Directory for binary outputs of external dependencies
    │   └── tmp                                 // Directory for temporary files
    ├── CBuild.run                              // Built buildscript
    ├── compile_commands.json
    ├── scripts
    │   ├── main.cpp                            // CBuild loader, main.cpp of CBuild buildscript,
    │   ├── user_init.cpp                       // User scripts
    │   └── user_init.hpp                       // Header to call user functions from CBuild main file
    ├── <target>_src                            // Source code of one target
    │   ├── main.cpp
    │   └── main.hpp
    └── <other_target>_src                      // Source code of other target
        └── main.cpp
```

---

&nbsp;

## Basic classes of CBuild

CBuild build system consists of five major pieces:

 * Extensible command-line parser
 * Main loop
 * Registry system
 * Base classes for use with registry
 * Standard library

&nbsp;

#### CLI parser

CLI parser works by parsing list of command-line arguments to [`lib::map<std::string, std::string>`](../../doxygen/classlib_1_1map.html) and [`CBuild::RType`](../../doxygen/namespaceCBuild.html#a7c7ed9679b1479745032dd97abbd7ce2). Map is used to map data tags to its value. Anything can be used here but only few have some processing in CBuild (and are passed to a **task** or a **targets**. Acceptable values are:

 * *out*, its acceptable values are:
     - *none*
     - *verbose*
 * *gen*, specifies which external **generator** will be used, value is generator id.
 * *toolchain_id*, same but for **targets**
 * *task_id*, same but for **tasks**
 * *pa*___XXX___, **XXX** is any character sequence, by default numbers are used in a counter fashion, value is passed as argument to runned app.
 * *a*___XXX___, **XXX** is any character sequence, by default numbers are used in a counter fashion, any value is accepted, will be passed to user **tasks** or **targets**

 [`CBuild::RType`](../../doxygen/namespaceCBuild.html#a7c7ed9679b1479745032dd97abbd7ce2) is used to determine which action was requested. Parser can be extended by registering new sub-parsers. Each sub-parser consists of a function with this signature [`ARG_TYPE (*)(lib::map<std::string, std::string>*, char**, int, int, CBuild::RType*)`](../../doxygen/namespaceCBuild.html#a54268c36f06128af668374ed62653254) aliased to [`CBuild::handler`](../../doxygen/namespaceCBuild.html#a54268c36f06128af668374ed62653254). All parsing is performed by [`CBuild::Rtype CBuild::parse(lib::map<std::string, std::string>* args, int argc, char** argv)`](../../doxygen/namespaceCBuild.html#a65ab369788ba3c28cb453822f0126206) function.

 &nbsp;

#### Main loop

Split between `int main(int argc, char** argv)` in `main.cpp` file of the buildscript and [`void CBuild::loop(CBuild::Rtype mode, lib::map<std::string, std::string>* args)`](../../doxygen/namespaceCBuild.html#ac649cc483958d2ee270b9f260c70f8e0) function in CBuild core. First perform initialization, call user initialization function (by default `void init()`), call [cli parser](#cli-parser), do some error checking, call main loop from CBuild core and then perform clean exit. Main loop from CBuild core checks which action was requested and then calls it, recursively calling all its dependencies.

&nbsp;

#### Registry system

Implemented in namespace [`CBuild::Registry`](../../doxygen/namespaceCBuild_1_1Registry.html). Allow to register **targets**, **tasks**, **generators**, **keyword** (task alias, aquick way to add command-line parameter that will works as an alias to callins some specific task), change arguments that will be used to rebuild buildscript or add text to `--version` cli parameter. It also allows to retrieve all registered information (except arguments for buildsccript rebuild). It is also responsible for registering default **tasks**, **keywords**, **generators**, dummy handler for additional print for a `--version`, setups **target** that will rebuild buildscript. It relies on proper overloading of [base classes](#base-classes)

&nbsp;

#### Base classes

 * [`CBuild::Toolchain`](../../doxygen/classCBuild_1_1Toolchain.html) - Used to bundle compiler and linker, some default settings and some user-facing function to properly configure them. **Toolchain** name is used when referring this class or its subclasses only, objects of this class or its subclasses are referred as **Target**.
 * [`CBuild::Task`](../../doxygen/classCBuild_1_1Task.html) - Used to specify **tasks**. Each **tasks** consists of id, lists of dependency **tasks**, and function that will be called on execution of this task.
 * [`CBuild::Dependency`](../../doxygen/classCBuild_1_1Dependency.html) - External dependency handler. Has function to check if dependency need to be rebuild and function to actually rebuild it.
 * [`CBuild::generator_base`](../../doxygen/classCBuild_1_1Dependency.html) - External file **generator**. For example, it is used to generate **Makefile** or **compile_commands.json**.
 * [`CBuild::Hash`](../../doxygen/classCBuild_1_1Hash.html) - **Hasher**. Used to check if target nee to be rebuild and if so, which files need to be rebuild.

&nbsp;

#### Standard library

 * [`CBuils/build_data.hpp`](../../doxygen/build__data_8hpp.html) - Parser of CBuild's build metadata formats
 * [`CBuild/environment.hpp`](../../doxygen/environment_8hpp.html) - Manipulate environment variable used when calling any command using CBuild.
 * [`CBuild/filebuff.hpp`](../../doxygen/filebuff_8hpp.html) - File bufferization with read and write ability.
 * [`CBuild/filesystem.hpp`](../../doxygen/filesystem_09_09_8hpp.html), [`CBuild/files.hpp`](../../doxygen/files_8hpp.html) - Filesystem wrapper with some additional functions.
 * [`CBuild/map.hpp`](../../doxygen/map_8hpp.html) - Map datatype implementation, much worse than `std::map` in performance but has few additional functions.
 * [`CBuild/optional.hpp`](../../doxygen/classlib_1_1optional.html) - Optional datatype implementation.
 * [`CBuild/pkgconfig.hpp`](../../doxygen/pkgconfig_8hpp.html) - Wrapper around `pkg-config` command-line tool.
 * [`CBuild/print.hpp`](../../doxygen/print_8hpp.html) - Few print related functions: colored print, print disabling and few log levels.
 * [`CBuild/system.hpp`](../../doxygen/system_8hpp.html) - Wrapper around `std::system` and `popen`.

 ---

 &nbsp;

## Build process in-depth overview

 <span style="color:red">This is based on ***default*** CBuild toolchains, your custom toolchain can work differently.</span>.  

 First `CBuild::Toolchain::call` is called. Then all external dependencies are checked. Then all rguments are preprocesses and saved into internal scratchpad variables. Then dependency **tasks** marked as `CBuild::PRE` are called and then dependency **targets** are caled.  Then **hasher** is run and produces list of files then need to be recompiled. Then `CBuild::Toolchain::stdargs` is called to add standard arguments. Then ***6*** overloaded functions are called. Firstly `pre_build` is called, it can prepare things for build. Then `build` is called to actually compile things and then `post_build` cleans-up after then compilation process. Then The same sequence is called again for linking - `pre_link`, `link` and `post_link`. And then dependency tasks marked as `CBuild::POST` are called.
