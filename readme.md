# CBuild

![Build status](https://gitlab.com/cbuild/cbuild/badges/master/pipeline.svg?key_text=Build)
![Lates release](https://gitlab.com/cbuild/cbuild/-/badges/release.svg)

C standard library extender and simple build system library.

# Goals

 - Single header library and simple build process
 - Fully POSIX.1-2001 compatible (use `-DSTRICT_POSIX -D_POSIX_C_SOURCE=200112L` to try (`STRICT_POSIX` disable extension detection)).
 - Fully C99+GNU extentions compatible (gnu99)
 - Do not force user to do things in specifc way (eg. do not provide some helper functions that implements fixed build pipeline, better to have ability call any command and have few macro that define default build commands)
 - Do no rely on shell scripts for doing anything
 - Provide minimal but simple API
 - Be properly documented

# [Documentation](https://cbuild.gitlab.io/cbuild/)

# [Inspired by nob.h](https://github.com/tsoding/nob.h)

I had idea of cbuild.h before I found nob.h, but then I was using c++. This resulted in legacy CBuild version, which was heavy, very limiting and too much complex. I found nob.h and it was a really simple thing, so i decided to write my own. Few things are relatively close (cbuild\_da\_t, command.h module). But there were few thing that I don't liked - missing a few useful modules, weird API decisions because of c99 target (I target gnu99, so I have a lot more language features to work with). Also, I provide full Doxygen documentation, if this matters (reading code is cool, but not always convenient). No code from nob.h were explicitly used, but I have looked into API for some modules, and maybe used same API decisions sometimes.

# How to use

 1. Download cbuild.h from repo.
 2. Write some logic for your build into `cbuild.c`.
 3. Run `cc -o cbuild.run cbuild.c`

Actually, you can use any filename for any of your files, provided filenames are examples only.

Releases are not supported for now! But I will make a tags for each new release.

# wikimk.c

This is also a my project - small wiki generator. It uses pandoc and build a bunch of markdown files into static websites, which does not depends on anything and use a very little js (and are fully "Baseline" spec-conformant). In future this will be in its own repo, but for now it is here. I plan to replace Doxygenn with it (not a full doxygen alternative, but support a subset that I need, and only for C). But this will be in future. It does not belong to this repo, have 0 documentation and is not finished. If you want to use it - expect bugs and no support. When I will decide to finish it, it will be in its own repo.