---
title: Working with custom datatypes from CBuild
---
# Working with custom datatypes from CBuild

::: entry

CBuild provides multiple ***templated*** datatypes - dynamic array, string view, string buffer etc.

To create new datatype, you need to define a structure that has specific fields, and then you can use provided macro on that structure. Following code will create dynamic array of integers. Then any `cbuild_da_*` function can be used on this struct:

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

Using provided macro allows you to quickly create needed datastructure, but using your own structure allows you to add additional fields to this structure. You can see this done in [RGlob.h](DOC:cbuild_glob_t), where `cbuild_glob_t` both serves as a dynamic array of matches and additionally stores glob context. 

To create a new hashmap you need to define two following structures:

```c
typedef struct my_pair_t {
    int key;
    cbuild_map_tombstone_t tombstone;
    int data;
} my_pair_t;
typedef struct my_map_t {
    cbuild_map_pair_t* data;
    size_t size;
    size_t capacity;
    cbuild_map_hash_t hash;
    cbuild_map_keycmp_t keycmp;
    cbuild_map_clear_t clear; // Optional, not used internally
} my_map_t;
```

Then you need to initialize function pointers, either using one of provided `cbuild_map_init_*` functions or manually.

:::

::: entry

Linked list is a little special - you do not need to create structure for it. You just need to allocate element using provided functions and then you can attach new elements to it (or any of elements attached later, API does not care which element from list you pass to it). It rely on shadow data and gives you a direct pointer to an element.

:::