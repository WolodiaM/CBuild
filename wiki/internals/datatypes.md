---
title: Working with custom datatypes from CBuild
---
# Working with custom datatypes from CBuild

::: entry

CBuild provides multiple ***templated*** datatypes - dynamic array, string view, string buffer etc.

To create new datatype, you need to define structure that have specific fields, and then you can use provided macro on that structure. Following code will create dynamic array of integers. Then any `cbuild_da_*` function can be used on this struct:

```c
typedef struct my_da_t {
    int* data;
    size_t size;
    size_t capacity;
} my_da_t;
```

For dynamic arrays you can use provided `cbuild_da_new`. Following code defines same dynamic array:

```c
typedef cbuild_da_new(int) my_da_t;
```

Using provided macro allow you to quickly create needed datastructure, but using your own structure allows you to add additional fields to this structure. You can see this done in [RGlob.h](/doc/symbols.html#ID_cbuild_glob_t), where `cbuild_glob_t` both serves as a dynamic array of matches and additionally stores glob context.

To create a new stack, the following structure should be created:

```c
typedef struct my_stack_t {
    int* data;
    size_t ptr;
    size_t capacity;
} my_stack_t;
```

:::

::: entry

Map datatype is much more complex. Implementing a hash map fully in C macro much harder than dynamic array, so map uses real function for implementation. But C function does not support any templating. So, the map is not templated and rely on [`cbuild_map_t`](/doc/symbols.html#ID_cbuild_map_t), which stores sizes of key and full _pair_ structure. Map implementation makes few assumptions about data stored in it:

* The full _pair_ structure can be `memcpy`.
* Neither data nor key owns any resources. This assumption can be overridden by providing custom element cleanup function.
* If length of key is set to 0, then key is a pointer to a C-string. Still, the implementation assumes that key does not own that string.

When adding a new element to map, it does not fill anything in it and returns empty block of memory with size `elem_size`.

During key hashing and comparison block of memory with size `key_size` that starts at relative offset from the start of _pair_ is used. This behaviour has two exceptions - if `key_size` is 0 more complicated checks are performed and if custom key hashing and/or key comparison functions are provided then they are used.

Proving only a key hashing function is fine, because it can be used to implement different hash algorithm, for example. But providing a custom key comparison function without a key hashing function is pointless, because hash still will be taken from literal bytes of key, while comparison may implement completely different semantics (and default comparison just does `memcmp` over bytes of a key).

:::