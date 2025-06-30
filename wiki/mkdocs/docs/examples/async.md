# Async ompilation and then linking

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
cbuild.h - Build system core.  
cbuild.c - Your buildscript:
```c
#include "cbuild.h"
int main(int argc, char** argv) {
    cbuild_selfrebuild(argc, argv);
    cbuild_cmd_t c1 = cbuild_cmd;
    cbuild_cmd_append_many(&c1, CC, "-c", "src/main.c");
    cbuild_proc_t p1 = cbuild_cmd_async(c1);
    cbuild_cmd_t c2 = cbuild_cmd;
    cbuild_cmd_append_many(&c2, CC, "-c", "src/utils.c");
    cbuild_proc_t p2 = cbuild_cmd_async(c2);
    if (!cbuild_proc_wait(p1)) {
        return 1;
    }
    if (!cbuild_proc_wait(p2)) {
        return 1;
    }
    cbuild_cmd_t l = cbuild_cmd;
    cbuild_cmd_append_many(&l, LD, "-o", "app.run", "utils.o", "main.o");
    if (!cbuild_cmd_sync(l)) {
        return 1;
    }
    if (!cbuild_file_remove("main.o")) {
        return 1;
    }
    if (!cbuild_file_remove("utils.o")) {
        return 1;
    }
    return 0;
}
```

### How to run

```bash
cc -o cbuild.run cbuild.c
./cbuild.run
./app.run
```