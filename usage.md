# CBuild usage and some useful info
## CLI arguments
 * `-f` - force recompilation of all files.
 * `-v` - verbose output, some addition info and some debugging info, mask `--no-cli-out` if used before.
 * `--no-cli-out` - no output from CBuild at all, mask `-v` if used before.
 * `-gm` - generate Makefile based on output from CBuild, simly log all executed command and pack it in Makefile syntax, so, sadly, CBuild::fs calls not propagate to Makefile.
 * `-gc` - generate compile_commands.json, with simmilar to `-gm` limitation
 * `-ld <toolchain_name>` - load dependencies of some target, only for CBuild dependencies.
 * `-b <toolchain_name>` - build specified target.
 * `-r <toolchain_name>` - run specified target.
 * `-br <toolchain_name>` - build and then run specified target.
 * `-d <toolchain_name>` - build and then run specified target in debug mode. Now work only with gdb on linux.
 * `-c <toolchain_name>` - clear all data of specified target.
 * `-t <task_name>` - run specified task.
 * `-a <arg>` - set some argument (see it as #define arg, without value). It can be used by ran toolchain or ran task. No preprocessing, simple propagation for user-defined arguments.
 * `-pa <arg>` - specified argument for ran app, propagated to app ran by `-r` and `-br`, in `-d` arguments propagates to gdb.
 * `--rebuild` - rebuild CBuild executable.
 * `--init` - init CBuild folder structure, not always required.
 * `--help` - print help message - this information.
## Folder structure
```
<project_root>
 |
 | - src                    : Project sources folder
 |    |
 |    | - main.cpp          : Have main() function implemented
 |
 | - scripts
 |    |
 |    | - main.cpp          : Main file of CBuild user scripts, init CBuild and call user init
 |    | - user_init.cpp     : Implement user init function, have build sript implemented
 |    | - user_init.hpp     : Header to glue together two .cpp files
 | - cache                  : CBuild der for some temporary data
 |    |
 |    | - headers           : Header of included CBuild projects
 |    | - libs              : .so / .a files of included CBuild projects
 |    | - tmp               : Directory for temporary runtime data of CBuild 
 |
 | - build
 |    |
 |    | - <toolchain_name>
 |    |    | - hash         : Hash storage directory
 |    |    |    |
 |    |    |    | - hash    : Hash database for specified target
 |    |    | - objs         : Saved .o files of specified target
 |    |    | - out          : Output files of specified target
 |    |
 |
 | - CBuild.run             : Main executable of CBuild
```
## Programmer info
### Compiler calling convention:
All compilation is done in two steps: compilation `<compiler> -c <file> <arguments> -o <object>` and then linking `<compiler/linker> <object> <arguments> -o <output binary>`