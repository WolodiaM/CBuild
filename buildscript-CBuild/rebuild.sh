#!/usr/bin/env bash
g++ buildscript-CBuild/*.cpp -LCBuild/CBuild/ -lCBuild -Wl,-z,origin -Wl,-rpath,\$ORIGIN/CBuild/CBuild -g -fPIC -std=c++20 -Wall -Wextra -Wno-comments -o build.run