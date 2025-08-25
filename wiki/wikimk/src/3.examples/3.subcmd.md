---
title: Sub commands
---
# Handling subcommandds

::: entry

### Code

Directory structure:
```tree
.
├── src
│   ├── utils.h
│   ├── utils.c
│   └── main.c
│
├── cbuild.h
└── cbuild.c
```
main.c - You app source code:
```c
#include "stdio.h"
#include "utils.h"
int main(int argc, char** argv) {
    printf("Hello, world!\nSum: %d\n", sum(1, 1));
    return 0;
}
```
utils.h - Another source code file:
```c
#ifndef UTILS_H
#define UTILS_H
int sum(int a, int b);
#endif // UTILS_H
```
utils.c - Another source code file:
```c
#include "utils.h"
int sum(int a, int b) {
    return a + b;
}
```
cbuild.c - Your buildscript:
```c
#include "cbuild.h"
#include "string.h"
bool run() {
    cbuild_cmd_t cmd = {0};
    cbuild_cmd_append_many(&cmd, "./app.run");
    return cbuild_cmd_sync(&cmd);
}
bool build() {
    cbuild_cmd_t c = {0};
    cbuild_cmd_append_many(&c, CC, "-c", "src/main.c");
    cbuild_proc_t p1;
    cbuild_cmd_run(&c, .pass_proc = true, .proc = &p1);
    cbuild_cmd_append_many(&c, CC, "-c", "src/utils.c");
    cbuild_proc_t p2;
    cbuild_cmd_run(&c, .pass_proc = true, .proc = &p2);
    if (!cbuild_proc_wait(p1)) {
        return false;
    }
    if (!cbuild_proc_wait(p2)) {
        return false;
    }
    cbuild_cmd_t l = {0};
    cbuild_cmd_append_many(&l, LD, "-o", "app.run", "utils.o", "main.o");
    if (!cbuild_cmd_run(&l)) {
        return false;
    }
    if (!cbuild_file_remove("main.o")) {
        return false;
    }
    if (!cbuild_file_remove("utils.o")) {
        return false;
    }
    return true;
}
int main(int argc, char** argv) {
    cbuild_selfrebuild(argc, argv);
    // Shift-out app name (argv[0])
    cbuild_shift(argv, argc);
    const char* cmd = cbuild_shift(argv, argc);
    if (strcmp(cmd, "build") == 0) {
        if(!build()) {
            return 1;
        }
    } else if (strcmp(cmd, "run") == 0) {
        if(!run()) {
            return 1;
        }
    }
    return 0;
}
```
cbuild.h - Build system core.

:::
:::entry

### How to run

```bash
cc -o cbuild.run cbuild.c
./cbuild.run
./app.run
```

:::