# Build config

### Code

Directory structure:  
```tree
.
├── cbuild.h
├── cbuild2.c
└── cbuild.c
```
cbuild.h - Build system core.  
cbuild.c - Your build system wrapper and configuration script:
```cpp
#include "cbuild.h"
int main(int argc, char** argv) {
    cbuild_selfrebuild(argc, argv);
    cbuild_sb_t cfg = cbuild_sb;
    cbuild_sb_append_cstr(&cfg, "#define CONFIG_VAR");
    if (!cbuild_file_check("config.h")) {
        if (!cbuild_file_write("config.h", &cfg)) {
            return 1;
        }
    }
    cbuild_cmd_t cmd = cbuild_cmd;
    cbuild_cmd_append_many(&cmd, CC, "-o", "cbuild2.run", "cbuild2.c");
    if (!cbuild_cmd_sync(cmd)) {
        return 1;
    }
    cmd.args.size = 0;
    cbuild_cmd_append(&cmd, "./cbuild2.run");
    cbuild_cmd_append_arr(&cmd, argv, argc);
    if (!cbuild_cmd_sync(cmd)) {
        return 1;
    }
    return 0;
}
```
cbuild2.c - Your buildscript:
```c
#include "cbuild.h"
#include "config.h"
int main(int argc, char** argv) {
#ifdef CONFIG_VAR
    cbuild_log(CBUILD_LOG_PRINT, "Hello, world!\n");
#endif // CONFIG_VAR
    return 0;
}
```

### How to run

```bash
cc -o cbuild.run cbuild.c
./cbuild.run
./app.run
```