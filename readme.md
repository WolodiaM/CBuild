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

I implemented first, legacy CBuild before I found nob.h. It targeted c++ and had a few very bad API decision. Few things were really great but compiling was inflexible. When I found nob I was in process of switching to C as main language. So I took few ideas for new cbuild. Some API choices was taken from nob, some were my own, some were taken from other places (e.g., dynamic arrays are based on nob for API, struct layout and some macro; string views use both nob's string view and c++ std::string_view to get some ideas for an API; hashmap use my own API design, same as linked list, so I have not just *stolen* API from other places).
Original cbuild idea was taken from Gradle, which uses groovy, which has full interop with java (language that is compiled with Gradle).
No code was explicitly taken from nob, but cbuild\_da\_t is pretty similar, because I *decided* to use same structure as dynamic array (this is pretty standard dynamic array for C after all, and I saw code from nob so some code may look similar even I was not actually taken code from there), string view API was inspired by both nob's string\_view, tsoding's sv.h and c++'s std::string\_view. On the other hand map.h API is fully mine, and is not based on any other library, same is with flag parser.
Compared to nob, cbuild has a lot bigger API surface, but don't support Windows for now. On the other hand, it requires GNU C compatible compiler, not ISO C compatible. Philosophy of library is also slightly different - main goal of cbuild is to extend standard C library with things that are often required - container datatypes, basic CLI argument parser, filesystem API, which is slightly more convenient than raw POSIX, etc. Compilation helpers are just one part, and if command runner is considered a stand-alone part (it is pretty useful even outside compilation context) it contains just 3 things - macro for compiler flags, cbuild\_selfrebuild group of functions to allow buildscript to be rebuild on startup and two functions for comparing mtime (modification time) to allow caching of compiled artifacts (in same way `make` does).
Compared with other *buildsystems* used for C it allows to use real code inside of a buildscript and allow to use your main language even when writing build code. Also it does not require some structure from buildscript, so for simple projects just few calls to external commands may be fine, for more complex ones you may dynamically generate some input data (e.g., lists of sources), and for some complex projects you may use multiple stages where each reconfigures next, setups it, builds it and then runs it to perform some actions.

# Supported platform

* Full support
  - x86-64 Linux. This is main development platform. It requires 64bit libc, POSIX.1-2001 support and either glibc, musl libc or uclibc. Other libc can be supported by you would probably need to add detection for them and enable/disable enabled extensions. As compiler you should use either gcc or clang. As far as I know no other compilers support all extensions this lib requires. This platform is tested on CI.
  - AArch64 Linux. Support is derived from x86-64 variant. Same set of requirements and limitations applies. This platform is tested on CI.
  - x86-64 MacOS. Support is derived from x86-64 Linux. It support some MacOS-specific extensions and some extensions from its libc. As compiler you should use either gcc or clang. As far as I know no other compilers support all extensions this lib requires. This platform is tested on CI. NOTE: I does not have a way to test it on real hardware, so support may be a bit worse than on Linux but I try to keep it working.
  - AArch64 MacOS. Support is derived from x86-64 variant. Same set of requirements and limitations applies. This platform is tested on CI. NOTE: I does not have a way to test it on real hardware, so support may be a bit worse than on Linux but I try to keep it working.
* Partial support
* Some support
  - x86-64 Windows with Cygwin. This platform is derived from x86-64 Linux. As compiler you should use either gcc or clang. As far as I know no other compilers support all extensions this lib requires. This platform is tested on CI. NOTE: It is often broken on CI because I does not have hardware to test it on. Additionally it has a lot more quirks than MacOS. So while I try to sup As compiler you should use either gcc or clang. As far as I know no other compilers support all extensions this lib requires. This platform is tested on CI.port it, sometimes it may be broken. On the other hand, a lot of CI problems are just CI problems, related to how it runs commands and how cygwin interoperates with windows.
  - x86-64 BSD. This platform is derived from x86-64 Linux. It does not have CI, but I try to support it, test runner fully support it. As compiler you should use either gcc or clang. As far as I know no other compilers support all extensions this lib requires. This platform is tested on CI. NOTE: It is often broken on CI because I does not have hardware to test it on.
  - AArch64 BSD. This platform is derived from x86-64 variant. It does not have CI, but I try to support it, test runner fully support it. As compiler you should use either gcc or clang. As far as I know no other compilers support all extensions this lib requires. This platform is tested on CI. NOTE: It is often broken on CI because I does not have hardware to test it on.
  - Any platform supporting POSIX.1-2001. It should work, but I can not guarantee it would work in all edge cases. All extensions are gated behind conditional compilation and are enabled only if platform is known to support them. You would probably need to add some detection logic and enable/disable extensions as fit for specific platform you are targeting. As compiler you should use either gcc or clang. As far as I know no other compilers support all extensions this lib requires. This platform is tested on CI.

# How to use

 1. Download cbuild.h from repo.
 2. Write some logic for your build into `cbuild.c`.
 3. Run `cc -o cbuild.run cbuild.c`

Actually, you can use any filename for any of your files, provided filenames are examples only.

Releases are deprecated, you can just pull file from repo directly, no build is required! Each release is marked by a tag.

CBuild don't require any structure from your code so you script can be structured in any way you like. Personally, I like to start main from shifting argv[1], which is treated as subcommand, then checking for build directory and then dispatching to one of required function based on subcommand with big if-else block, having strcmp on each check. Some subcommands can be implemented directly in main if they are small. Compilation is handled by one function which take src and dst files and then compile with all required arguments, it performs all error checking and sometimes recovery. Linking is inlined in either 'compile' function or inside of main. Source files are often generated dynamically by just giving which dir need to be compiled and then iterating over all files in that directory. Build directory, src directory and other constant/configuration values are provided by defines. But this is just my approach, CBuild don't have any requirements for code structure of you application/buildsccript.

# Wiki

Wiki was in a pretty bad shape. I rewrote most of it, rewrote all doc comments, transitioned from doxygen to a custom format, and now it is in at least maintainable state. But content is still lacking and will be updated, especially examples section.

# wikimk

This is also a my project - small wiki generator. It uses pandoc and build a bunch of markdown files into static websites, which does not depends on anything and use a very little js (and are fully "Baseline" spec-conformant). In future this will be in its own repo, but for now it is here. I plan to replace Doxygenn with it (not a full doxygen alternative, but support a subset that I need, and only for C). But this will be in future. It does not belong to this repo, have 0 documentation and is not finished. If you want to use it - expect bugs and no support. When I will decide to finish it, it will be in its own repo. It also support building docs out of code with doc comments like in this repo.