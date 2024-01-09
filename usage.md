# CBuild usage and some useful info
## CLI arguments
 * `-f` - force recompilation of all files.
 * `-v` - verbose output, some addition info and some debugging info, mask `--no-cli-out` if used before.
 * `--no-cli-out` - no output from CBuild at all, mask `-v` if used before.
 * `-g make` - generate Makefile based on output from CBuild, simly log all executed command and pack it in Makefile syntax, so, sadly, CBuild::fs calls not propagate to Makefile.
 * `-g ccj` - generate compile_commands.json, use internal command evaluator in CBuild::Toolchain, from the box work only for toolchains, that use standard compiler calling convention, see below.
 * `-ld <toolchain_name>` - load dependencies of specified target, only for CBuild dependencies.
 * `-b <toolchain_name>` - build specified target.
 * `-b all` - build all registered targets
 * `-r <toolchain_name>` - run specified target.
 * `-br <toolchain_name>` - build and then run specified target.
 * `-d <toolchain_name>` - build and then run specified target in debug mode. Now work only with gdb on linux.
 * `-c <toolchain_name>` - clear all data of specified target.
 * `-t <task_name>` - run specified task.
 * `-a <arg>` - set some argument (see it as #define arg, without value). It can be used by ran toolchain or ran task. No preprocessing, simple propagation for user-defined arguments, preserver order in what arguments is specified.
 * `-pa <arg>` - specify argument for ran app, propagated to app ran by `-r` and `-br`, in `-d` arguments propagates to debugger.
 * `--rebuild` - rebuild CBuild executable.
 * `--init` - init CBuild folder structure, not always required.
 * `--help` - print help message - this information.
 * `--version` - print curent version of CBuild
 * `--task-list` - print list of all registered tasks
 * `--target-list` - print list of all registered targets
 * `--generator-list` - print list of all registered generators
 * `--commands-list` - print list of all registered user commands
## Folder structure
```
<project_root>
 |
 | - src                                : Project sources folder
 |    |
 |    | - main.cpp                      : Have main() function of project implemented
 |
 | - scripts
 |    |
 |    | - main.cpp                      : Main file of CBuild user scripts, init CBuild and call user init
 |    | - user_init.cpp                 : Implement user init function, have build sript implemented
 |    | - user_init.hpp                 : Header to glue together two .cpp files
 | - cache                              : CBuild dir for some temporary data
 |    |
 |    | - headers                       : Header of included CBuild projects
 |    | - libs                          : .so / .a files of included CBuild projects
 |    | - tmp                           : Directory for temporary runtime data of CBuild 
 |
 | - build
 |    |
 |    | - <toolchain_name>
 |    |    | - config                   : Hash storage directory
 |    |    |    |
 |    |    |    | - src.main.meta       : Metadata for src/main.cpp
 |    |    | - objs                     : Saved .o files of specified target
 |    |    | - out                      : Output files of specified target
 |    |    | - <toolchain_name>.meta    : Metadata for a full target
 |
 | - CBuild.run                         : Main executable of CBuild
```
## Programmer info
### Compiler calling convention:
All compilations is done in two steps: compilation `<compiler> -c <file> <arguments> -o <object>` and then linking `<linker> <objects> <arguments> -o <output binary>`
### Hasher general rewuirments
All hashers are inherited of CBuild::Hash class.  
Class constructor need to take one argument of type std::string and pass it to a base class.  
Function 'get_files_for_recompilation' need to construct a `lib::map<std::string, std::string>` of source->object mappings of files that need to be recompiled, files and objects array are for one set of files, `objects.at(1)` is an object for a source file at `files.at(1)`. So, this function need to produce a list of files that need to be recompiled and update stored metadata.  
All other function need to return an int value acording to their doxygen docs and also update a coresponding metadata value.  
