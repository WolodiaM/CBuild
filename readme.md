# CBuild

![Build status](https://github.com/WolodiaM/CBuild/actions/workflows/test.yml/badge.svg)
![Lates release](https://img.shields.io/github/v/release/WolodiaM/CBuild)

C standard library extender and simple build system library.

# Goals

 - Single header library and simple build process
 - Fully POSIX.1-2001 compatible (use `-DSTRICT_POSIX -D_POSIX_C_SOURCE=200112L` to try (`STRICT_POSIX` disable extension detection)).
 - Fully C99+GNU extensions compatible (gnu99)
 - Do not force user to do things in specific way (eg. do not provide some helper functions that implements fixed build pipeline, better to have ability call any command and have few macro that define default build commands)
 - Do no rely on shell scripts for doing anything
 - Provide minimal but simple API
 - Be properly documented

# [Documentation](https://WolodiaM.github.io/CBuild/)

# [Inspired by nob.h](https://github.com/tsoding/nob.h)

I implemented first, legacy CBuild before I found nob.h. It targeted c++ and had a few very bad API decision. Few things were really great but compiling was inflexible. When I found nob I was in process of switching to C as main language. So I took few ideas for new cbuild. Some API choices was taken from nob, so were my own. some were taken from other places. Original cbuild idea was taken from Gradle, which uses groovy, which has full interop with java (language that is compiled with Gradle). No code was explicitly taken from nob, but cbuild\_da\_t is pretty similar, because I "decided" to use same structure as dynamic array (but this is pretty standard dynamic array for C), string API was inspired by both nob's string\_view, tsoding's sv.h and c++'s std::string\_view. On the other hand map.h API is fully mine,  and is not based on any other library. Compared to nob, cbuild has a lot bigger API surface, but don't support Windows for now. Also, it require GNU C compatible compiler, not ISO C compatible. Philosophy of library is also slightly different - main goal of cbuild is to extend standard C library with things that are often required - container datatypes, basic CLI argument parser, filesystem API, which is slightly more convenient than raw POSIX, etc. Compilation helpers are just one part, and if command runner is considered a stand-alone part (it is pretty useful even outside compilation context) is contains just 3 things - macro for compiler flags, cbuild\_selfrebuild group of functions to allow buildscript to be rebuild on startup and two functions for comparing mtime (modification time) to allow caching of compiled artifacts (in same way make does).

# How to use

 1. Download cbuild.h from repo.
 2. Write some logic for your build into `cbuild.c`.
 3. Run `cc -o cbuild.run cbuild.c`

Actually, you can use any filename for any of your files, provided filenames are examples only.

Releases are deprecated, you can just pull file from repo directly, no build is required! Each release is marked by a tag.

CBuild don't require any structure from your code so you script can be structured in any way you like. Personally, I like to start main from shifting argv[1], which is treated as subcommand, then checking for build directory and then dispatching to one of required function based on subcommand with big if-else block, having strcmp on each check. Some subcommands can be implemented directly in main if they are small. Compilation is handled by one function which take src and dst files and then compile with all required arguments, it performs all error checking and sometimes recovery. Linking is inlined in either 'compile' function or inside of main. Source files are often generated dynamically by just giving which dir need to be compiled and then iterating over all files in that directory. Build directory, src directory and other constant/configuration values are provided by macro. But this is just my approach, CBuild don't have any requirements for code structure of you application/buildsccript.

# wikimk.c

This is also a my project - small wiki generator. It uses pandoc and build a bunch of markdown files into static websites, which does not depends on anything and use a very little js (and are fully "Baseline" spec-conformant). In future this will be in its own repo, but for now it is here. I plan to replace Doxygenn with it (not a full doxygen alternative, but support a subset that I need, and only for C). But this will be in future. It does not belong to this repo, have 0 documentation and is not finished. If you want to use it - expect bugs and no support. When I will decide to finish it, it will be in its own repo.