#!/usr/bin/env bash
g++ scripts/*.cpp -LCBuild/CBuild/ -lstdc++ -lCBuild -Wl,-z,origin -Wl,-rpath,\$ORIGIN/CBuild/CBuild -g -std=c++20 -Wall -Wextra -Wno-comments -pg -o build.run
