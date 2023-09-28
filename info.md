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
 * All (except .git/ .vscode/ build/ cache/) - 22M
 * Source code (rebuild.sh + CBuild/headers/ + CBuild/tmp/ + CBuild/CBuild/src/) - 320K
 * Build script (build.sh + build.run + buildscript-CBuild/ ) - 960K
 * libCBuild.so (without -g) - 1.9M
 * deb folder - 2.0M
 * ppa/ubuntu folder - 8.4M
 * doxygen folder + doxygen.conf - 7.0M  
 * template size - 712K   
All sizes are generated using `du -hcs <folder/file0> <folder/file1> ...`
# Profiling
gmon.out - profiling data for gprof
analisis.txt - output of `gprof ./build.run gmon.out > analisis.txt`

