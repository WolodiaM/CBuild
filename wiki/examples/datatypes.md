---
title: Creating custom datatypes
---
# Working with datatypes

::: entry

CBuild provide multiple datatypes - dynamic array, string view, string buffer etc.

To create new datatype you need to define structs that have specific fields and then you can use provided macro on that fields. So, to create a new dynamic array you can simply do. Following code will create dynamic array of ints. Then any `cbuild_da_*` function can be used on this struct:

```c
typedef struct my_da_t {
    int* data;
    size_t size;
    size_t capacity;
} my_da_t;
```

Also, this allow to add custom fields to a struct easily. You can this done in [Command.h](/doxygen/structcbuild__cmd__t.html). There, `cbuild_cmd_t` is a standard dynamic array, but it have another fields, that is specific to this struct.

To create a new stack this struct should be created:

```c
typedef struct my_stack_t {
    int* data;
    size_t ptr;
    size_t capacity;
} my_stack_t;
```

Map is slightly more complex. This datatype rely on internal [structures](/doxygen/structcbuild__map__t.html) and sizes of user data. You need to either use `cbuild_map_t` directly or embed it into your struct. In theory you could place it as a first struct member and cast pointers to your struct to a `cbuild_map_t`. Map support plug-gable *hash* ans *keycmp* (*key compare*) functions. But it also provides its own implementations for basic cases. Field `elem_size` is used to allocate new elements. This should be set to a size of a full userdata payload in bytes (key + one or more values). Field `key_size` should be set to a key size in bytes. This allow to use any size of integers and fixed-size arrays as a keys. If `key_size` is set to **0** then key is assumed to be a *c-string* and key size is a `sizeof(char*)`. Also, *hash* and *keycmp* function can be overriden separatly, if needed. If `key_size` is bigger than **0** then `memcmp` is used to compare 2 keys and hash is performed direcly on a block of memory, pointed by an element point (key is a first 'field' in an element 'struct'). If `key_size` is **0** then `strcmp` is used and key size is calculated by `strlen`.

:::