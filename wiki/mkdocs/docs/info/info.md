# General project information

---

&nbsp;

## Dependencies

### Libraries

 * Linux
 * glibc6 that has this elf symbols:
     - `GLIBC_2.2.5`
     - `GLIBC_2.7`
     - `GLIBC_2.14`
 * libstdc++ that supports c++20 and has this elf symbols:
     - `GLIBCXX_3.4.20`
     - `GLIBCXX_3.4.11`
     - `GLIBCXX_3.4.18`
     - `GLIBCXX_3.4.14`
     - `GLIBCXX_3.4.21`
     - `GLIBCXX_3.4.15`
     - `GLIBCXX_3.4.26`
     - `GLIBCXX_3.4`
 * libgcc from gcc `10.5.0`
 * libpthread (if you need multithreaded compilation)

### Shell commands

 * rm (need to suppprt `-rf` keys) -> Used by `./CBuild.run -r <target_name>`
 * Some debugger, these are support out of the box:
     - `gdb`
     - `lldb`
 * Some compiler, these are support out of the box:
     - `gcc`
     - `g++`
     - `clang`
     - `clang++`
     - `x86_64-w64-mingw32-gcc`
     - `x86_64-w64-mingw32-g++`

## Build dependencies

### Libraries

 * Same as normal CBuild dependencies
 * libpthread

### Shell commands

 * Same as normal CBuild dependencies
 * cp
 * cd
 * dpkg-scanpackages
 * apt-ftparchive
 * gpg
 * gzip
 * dpkg-deb

---

&nbsp;

## Unimplemented features that I will not implement by myself
 
 * Debugger for foreign executable (at least not by myself, if you want to contribute to CBuild and add debugger to mingw-* toolchains or add another toolchain feel free to submit a pullrequst)

---

&nbsp;

## Filesizes
 
 * All (except hiden directories and  directories `build`, `cache`) - *40 __MB__*
 * Source code (directories `sh` and `CBuild` except `CBuild/CBuild/libCBuild.*`) - *564 __KB__*
 * Build script (files `build.sh`, `CBuild.run`, directory `scripts`) - *396 __KB__*
 * Core library (file `CBuild/CBuild/libCBuild.so`) - *2.2 __MB__*
 * Output packages (directory `packages`) - *24 __MB__*
 * Wiki (Directory `wiki` except `wiki/mkdocs/site`, file `doxygen.conf`) - *12 __MB__*  
 * Cbuild template - *688 __KB__*   
All sizes are generated using `du -hcs <folder/file0> <folder/file1> ...`

---
