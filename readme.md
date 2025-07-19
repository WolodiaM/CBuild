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

# How to use

 1. Download [cbuild.h](https://gitlab.com/cbuild/cbuild/-/jobs/artifacts/master/browse?job=build).
 2. Write some logic for your build into `cbuild.c`.
 3. Run `cc -o cbuild.run cbuild.c`

Actually, you can any filename for any of your files, provided filenames are examples only.