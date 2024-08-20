## Unimplemented and not seen in near future
mingw debugger

# Depends
### Libs:
 * libc6
 * libstdc++
### Shell commands:
 * For main run
    * rm
    * debuger, default:
        * gdb
    * compiler, default:
        * gcc
        * g++
        * g++-mingw-w64-x86-64-win32
        * gcc-mingw-w64-x86-64-win32
        * clang
        * clang++
 * For build.run executable for building cbuild
    * Required
        * cp
        * mv
        * cd
    * Deb packet
        * gpg
        * dpkg-scanpackages
        * apt-ftparchive
        * gzip
        * dpkg-deb
# Sizes of files
 * All (except .git/ .vscode/ build/ cache/) - 40M
 * Source code (sh/ + CBuild/headers/ + CBuild/tmp/ + CBuild/CBuild/src/ + project_init.sh.template + update.sh) - 564K
 * Build script (build.sh + CBuild.run + scripts/ ) - 396K
 * libCBuild.so (without -g) - 2.2M
 * packages/deb folder - 2.4M
 * packages/ppa/ubuntu folder - 19M
 * doxygen folder + doxygen.conf - 12M  
 * template size - 688K   
All sizes are generated using `du -hcs <folder/file0> <folder/file1> ...`
# Profiling
gptof/gmon.out - profiling data for gprof
gprof/analisis.txt - output of `gprof ./build.run gmon.out > analisis.txt`

