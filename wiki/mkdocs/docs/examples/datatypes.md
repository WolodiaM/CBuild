# Working with datatypes

CBuild provide multiple datatypes - dynamic array, string view, string buffer etc. To work with them you need to register them for your specific underlying datatypes. For example, to create a dynamic array of `int's` you could do something like this:

header.h
```c
#include "cbuild.h"
cbuild_da_t(int, int)l
cbuild_da_t_ext_impl(int);
```
source.c
```c
#include "header.h"
cbuild_da_t_impl(int, int)
```

This code will create new ___datatype___ - `cbuild_da_int_t` and its initializer value - `cbuild_da_int`.

Then ___map___ datatype is special - it requires two additional functions `cbuild_map_<key_name>_<value_name>_keycmp` with a signature of `(<key_type)* k1, <key_type>* k2) -> bool` and `cbuild_map_<key_name>_<value_name>_hash_t` with a signature of `(<key_type>* k) -> size_t`.