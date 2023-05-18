# CBuild
Full feathured build system for C / C++.
All build scripts go into scripts folder.
All build scripts is written in C++ (can be written in C but CBuild does not have any C API).
All html and TeX are because of doxygen

# Example
This is folder structure:  
<pre>
project  
    |  
    |- - build (will be created by CBuild)  
    |- - cache (will be created by CBuild)  
    |  
    |--- scripts (build scripts go here)  
    |           |  
    |           |--- main.cpp  
    |           |--- user_init.hpp  
    |           |--- user_init.cpp  
    |  
    |--- src (project code goes here)  
    |--- CBuild.run (main CBuild executable)  
</pre>
main.cpp -> main file of Cbuild executable, change only if you know how CBuild work internally.  
user_init.hpp -> headers of user scripts, have 1 function, that need to be called in main.cpp (init()).  
user_init.cpp -> implemenet init() form headers, thie function will be ruuned at a start of Cbuild, before all core functions.  
example build script for lib and app that use it:
<pre>
// C++ libraries  
#include "stdio.h"  
// Userspace headers  
#include "user_init.hpp"  
// CBuild headers  
#include "CBuild/task/Task.hpp"  
#include "CBuild/register.hpp"  
#include "CBuild/build/g++.hpp"  
#include "CBuild/print.hpp"  
  
CBuild::GXX app("app", "App");  
CBuild::GXX lib("lib", "lib");  
  
void init()  
{  
	app.set_standart("c++20");  
	app.add_file("src/main.cpp");  
	app.warn();  
	app.set_type(CBuild::EXECUTABLE);  
	app.depends_on("lib");  
	CBuild::Registry::RegisterTarget(&app);  
	lib.set_standart("c++20");  
	lib.add_file("src/test.cpp");  
	lib.warn();  
	lib.set_type(CBuild::DYNAMIC_LIBRARY);  
	CBuild::Registry::RegisterTarget(&lib);  
}  
</pre>
It uses g++ as compiler, and c++20.
# Other info
[info](info.md)
