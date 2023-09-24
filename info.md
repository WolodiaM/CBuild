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
 * All (except .git/ .vscode/ build/ cache/ src/ Makefile scripts/ CBuild.run) - 21M
 * Source code (hpp + cpp + rebuild.sh) - 284K
 * Build script (build.sh + build.run + buildscript-CBuild/ ) - 944K
 * libCBuild.so (without -g) - 1.9M
 * deb folder - 1.8M
 * ppa/ubuntu folder - 8.1M
 * doxygen folder + doxygen.conf - 6.7M  
All sizes are generated using `du -hcs <folder/file0> <folder/file1> ...`
# Profiling
gmon.out - rofiling data for gprof
analisis.txt - output of `gprof ./build.run gmon.out > analisis.txt`

