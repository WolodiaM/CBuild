# CBuild

![Build status](https://gitlab.com/cbuild/cbuild/badges/master/pipeline.svg?key_text=Build)
![Lates release](https://gitlab.com/cbuild/cbuild/-/badges/release.svg)


Build system that allows you to write your buildscripts for C or C++ (or actually any other scripts) with C, C++ or any language that can use C headers.

# Goals

 - Single header library and simple build process
 - Fully POSIX.1-2008 compatible
 - Fully C99+GNU extentions compatible (gnu99)
 - Do not force user to do things in specifc way (eg. do not provide some helper functions that implements fixed build pipeline, better to have ability call any command and have few macro that define default build commands)
 - Do no rely on shell scripts for doing anything
 - Provide minimal but simple API
 - Be properly documented

# [Documentation](https://cbuild.gitlab.io/cbuild/)

# [Inspired by nob.h](https://github.com/tsoding/nob.h)

I had idea of cbuild.h before I found nob.h, but then I was using c++. This resulted in legacy CBuild version, which was heavy, very limiting and too much complex. I found nob.h and it was a really simple thing, so i decided to write my own. Few things are relatively close (cbuild\_da\_t, command.h module). But there were few thing that I don't liked - missing a few useful modules, weird API decisions because of c99 target (I target gnu99, so I have a lot more language features to work with). Also, my code has tests and proper documentation (eg. Doxygen built documentation, reading code is cool, but not always a good way to find what function expects).

# How to use

 1. Download [cbuild.h](https://gitlab.com/cbuild/cbuild/-/releases/permalink/latest).
 2. Write some logic for your build into `cbuild.c`.
 3. Run `cc -o cbuild.run cbuild.c`

If you want to build from source you can clone this repo and run `./build.sh pack`. This expects POSIX environment with `bash`, `cat`, `tr`, `sed`, `echo`, `rm`, `astyle`.

Actually, you can any filename for any of your files, provided filenames are examples only.

# wikimk.c

This is also a my project - small wiki generator. It uses pandoc and build a bunch of markdown files into static websites, which does not depends on anything and use a very little js (and are fully "Baseline" spec-conformant). In future this will be in its own repo, but for now it is here. I plan to replace Doxygenn with it (not a full doxygen alternative, but support a subset that I need, and only for C). But this will be in future.