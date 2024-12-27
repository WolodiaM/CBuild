# Runable buildscript

This will allow to run your buildscript by simply running `./cbuild.c` but at the cost of recompling it every time.
This will work on Linux and should work on MacOS as it relies on shebang.

### Code

cbuild.c
```c
///$(which true);DIR="$(cd $(dirname "$0"); pwd -P)";SFILE="$DIR/$(basename "$0")";OFILE="$DIR/$(basename "$0").run";$(which gcc) -o "$OFILE" "$SFILE" || exit $?;exec -a "$0" "$OFILE" "$@"
#include "cbuild.h"
int main(int argc, char** argv) {
    // Your code here
}
```
If you use clang-format to format your code (or use embeded formatter from clangd) you need this file (it will prevent formatter from changeing any comments that dont start from space):  
.clang-format
```yaml
# Dont reformat commens without spacex in front
CommentPragmas:  '^[^ ]'
```

### How it works

Actuall code of this shebang is following:
```sh
$(which true)
DIR="$(cd $(dirname "$0"); pwd -P)"
SFILE="$DIR/$(basename "$0")"
OFILE="$DIR/$(basename "$0").run"
$(which gcc) -o "$OFILE" "$SFILE" || exit $?
exec -a "$0" "$OFILE" "$@"
```
First line is used to provide some command that can be found to comply with shebang specifications.  
Second line creates variable `DIR` that holds directory in which buildscript is located.  
Next two lines define two variables: `SFILE` and `OFILE` for source file and output file paths respectively.  
Fifth line ccompiles buildscript using gcc (you can change gcc to any compler you want and exits if error occured.  
And last line simply run built app with all arguments.  