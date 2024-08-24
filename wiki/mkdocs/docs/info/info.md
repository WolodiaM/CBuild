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
 
 * All (except hiden directories and  directories `build`, `cache`) - *28 __MB__*
 <!--du -hcs build.sh CBuild.run CBuild compile_commands.json doxygen.conf LICENSE mkdocs.yml packages/ readme.md scripts/ sh/ template/ TODO
/ wiki/-->
 * Source code (directories `sh` and `CBuild` except `CBuild/CBuild/libCBuild.*`) - *576 __KB__*
 <!--du -hcs sh CBuild --exclude CBuild/CBuild/libCBuild.*-->
 * Build script (files `build.sh`, `CBuild.run`, directory `scripts`) - *384 __KB__*
 <!--du -hcs build.sh CBuild.run scripts/-->
 * Core library (file `CBuild/CBuild/libCBuild.so`) - *2.2 __MB__*
 <!--du -hcs CBuild/CBuild/libCBuild.so-->
 * Output packages (directory `packages`) - *24 __MB__*
 <!--du -hcs packages/-->
 * Wiki (Directory `wiki`, files `doxygen.conf` and `mkdocs.yml`) - *76 __KB__*  
 <!--du -hcs wiki-->
 * Cbuild template - *688 __KB__*   
 <!--du -hcs template-->
All sizes are generated using `du -hcs <folder/file0> <folder/file1> ...`

---
