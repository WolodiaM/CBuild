## Unimplemented and not seen in near future
mingw debugger
multithreading, last attempt to create multithreaded compilation code produce code, that produce results two times longer, even using two cores, not one

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
 * All (except .git/ .vscode/ build/ cache/) - 31M
 * Source code (update.sh + rebuild.sh + CBuild/headers/ + CBuild/tmp/ + CBuild/CBuild/src/) - 432K
 * Build script (build.sh + build.run + scripts/ ) - 1.1M
 * libCBuild.so (without -g) - 2.2M
 * deb folder - 2.4M
 * ppa/ubuntu folder - 14M
 * doxygen folder + doxygen.conf - 8.5M  
 * template size - 684K   
All sizes are generated using `du -hcs <folder/file0> <folder/file1> ...`
# Profiling
gptof/gmon.out - profiling data for gprof
gprof/analisis.txt - output of `gprof ./build.run gmon.out > analisis.txt`

