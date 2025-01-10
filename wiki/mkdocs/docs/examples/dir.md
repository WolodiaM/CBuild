# Compiling directory of C files

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
#include "string.h"
#include "stdlib.h"
int main(int argc, char** argv) {
    cbuild_selfrebuild(argc, argv);
    CBuildPathList sources = {0};
    if (!cbuild_dir_list("src/", &sources)) {
        return 1;
    }
    CBuildCmd cmd = {0};
    cbuild_cmd_append_many(&cmd, CC, "-o", "app.run");
    for (size_t i = 0; i < sources.size; i++) {
        const char *file = sources.data[i];
        const char *ext = cbuild_path_ext(file);
        if (strcmp(ext, "c") == 0) {
            CBuildStrBuff path  = {0};
            cbuild_sb_append_cstr(&path, "src/");
            cbuild_sb_append_cstr(&path, file);
            cbuild_sb_append_null(&path);
            cbuild_cmd_append(&cmd, path.data);
        }
        free((char *)ext);
    }
    bool ret = true;
    if (!cbuild_cmd_sync(cmd)) {
        ret = false;
    }
    for (size_t i = 3; i < cmd.size; i++) {
        free((char *)cmd.data[i]);
    }
    cbuild_pathlist_clear(&sources);
    return ret;
}
```

### How to run

```bash
cc -o cbuild.run cbuild.c
./cbuild.run
./app.run
```