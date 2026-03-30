#pragma once // For LSP 
//! Hasmap implementation in C.
//!
//! License: `GPL-3.0-or-later`.
//!
//! This map has rather interesting API and contains.
//!
//! # No resize
//!
//! It does not resize on its own, but you can approximate usage by checking
//! number of elements in each bucket and reallocate map when it will not create
//! performance problems in your app. This can be done by just creating a new
//! map and copying data from an old one (this is then only way to resize
//! hasmap implemented like this).
//!
//! # Not a tree
//!
//! It is implemented as a real hasmap and has O(1) access time (at least until
//! most buckets have at most one element in them). If buckets are full performace
//! start to degrade to O(n) where n is average number of elements in a bucket.
//!
//! # Managing data is user's responsibility
//!
//! It does not have **set** operation, only **get** and **get_or_alloc**.
//! Because working with generic datatypes in C is pretty hard, this map does not
//! rely on a type of key or data. User need to supply size of a key and full
//! pair struct in bytes, and then key compared and hashed as byte array of
//! specified size. Map functions always take key by pointer because of this.
//! Elements is never written and **get_or_alloc** returns empty zero-initialized
//! memory. You need to write key and value yourself there. You can treat this
//! memory as either two byte arrays one after another or cast it to some struct.
//!
//! Map can also use custom clear function for elements (because it can consist
//! of any data, including pointers that "own" some external memory. While this
//! function primary role is to clear element, it can also be used to properly
//! free key that need some extra care to do that.
//!
//! # Support for C-strings as keys
//!
//! For key one extra case is handled - if key size is declared as 0, then key
//! is assumed to be `const char*` pointer to NULL-terminated string. This will
//! be used only when hashing or comparing keys, so if key is not a constant
//! string and may need to be freed, proper clear function should be provided.
//!
//! # Extensibility
//!
//! If key has some specific properties, or is not stored inline in *pair*
//! structure, then custom **hash** and **keycmp** functions can be provided.
//!
//! If extra data need to be attached to a map, eg. key is an array, with size
//! fixed per map object, structure like this can be defined:
//!
//! ```c
//! typedef struct map_with_array_key_t {
//!     cbuild_map_t map;
//!     size_t key_element_count;
//! } map_with_array_key_t;
//! ```
//!
//! Then pointer to `.map` is passe to map function, but because C places struct
//! fields in memory in a same way they are defined in code, this pointer can
//! later be converted to pointer to outside structure by any of user-passed
//! hook function. This will allow to access any additional data without using
//! standard way of passing arguments to callback via `void*`{.c} parameter, thus
//! simplifying API for a general use case.

#include "Common.h"

/// Hash function for a key.
///
/// * [pl:map] Pointer to map object. Can be used to retrieve extra data.
/// * [pl:key] Pointer to a start of key (start of pair structure).
///
/// [r:] Hash of this specific key. Will be truncated via `% nbuckets`{.c}.
typedef size_t (*cbuild_map_hash_t)(const void* map, const void* key);
/// Key comparison function. Should return `true` if keys are the same.
///
/// ::: note
/// This function should resolve hash collisions, so it can not rely on
/// comparing hashes.
/// :::
///
/// * [pl:map] Pointer to map object. Can be used to retrieve extra data.
/// * [pl:k1] Pointer to a start of key 1 (start of pair structure).
/// * [pl:k2] Pointer to a start of key 2 (start of pair structure).
typedef bool (*cbuild_map_keycmp_t)(const void* map, const void* k1,
	const void* k2);
/// Element clear function.
///
/// * [pl:map] Pointer to map object. Can be used to retrieve extra data.
/// * [pl:elem] Element that should be cleared. Pointer to a start of a pair structure.
typedef void (*cbuild_map_clear_t)(const void* map, void* elem);

/// This structure is internal and should not be used outside of a map.
/// It is docomented because it can be used to retrieve usage starts from a map.
///
/// * [fl:vals] Data of this bucket. Byte array of all pairs.
/// * [fl:nvals] Number of values stored in this bucket.
typedef struct cbuild_map_bucket_t {
	void* vals;
	size_t nvals;
} cbuild_map_bucket_t;

/// Map structure. Some fields here are considered as configuration data, and
/// should be modified by user by hand, while others are considered internal
/// values and should not be modified (but still can be if you know what you
/// are doing).
///
/// * [fl:buckets] List of bucket this map has. Pre-allocated on by [`cbuild_map_init`](#cbuild_map_init).
/// * [fl:nbuckets] Number of buckets. Parameter to [`cbuild_map_init`](#cbuild_map_init).
/// * [fl:hash_func] Custom hash function, can be left as `NULL`{.c} if unused.
/// * [fl:keycmp_func] Custom key comparison function, can be `NULL`{.c} if unused.
/// * [fl:key_size] Size of key. Should be set manually before call to [`cbuild_map_init`](#cbuild_map_init).
/// * [fl:elem_size] Size of pair structure. Should be set manually before call to [`cbuild_map_init`](#cbuild_map_init).
/// * [fl:iter_buckets] Internal counter used by iterator functions.
/// * [fl:iter_vals] Internal counter used by iterator functions.
typedef struct cbuild_map_t {
	cbuild_map_bucket_t* buckets;
	size_t nbuckets;
	cbuild_map_hash_t hash_func;
	cbuild_map_keycmp_t keycmp_func;
	cbuild_map_clear_t clear_func;
	size_t key_size;
	size_t elem_size;
	size_t iter_buckets;
	size_t iter_vals;
} cbuild_map_t;

/// Initialize a new map. Both *key_size* and *elem_size* must be valid at
/// this point. If needed *hash_func*, *keycmp_func* and *clear_func* can be
/// invalid, but it is better to init everything in one place, so it is a good
/// practive to have them valid before call to init too.
CBUILDDEF void cbuild_map_init(cbuild_map_t* map, size_t nbuckets);
/// Get element from a map. Raw semi-internal function.
///
/// * [pl:map] Map object.
/// * [pl:key] Pointer to a key. Need to contain `map->key_size`{.c} bytes.
///
/// [r:] Pointer to a pair structure or NULL if not found.
CBUILDDEF void* cbuild_map_get_raw(const cbuild_map_t* map, const void* key);
/// Get element from a map.
///
/// This function will allocate one stack variable that will hold copy of key.
/// This function will check if size of this variable matches `map->key_size`{.c}.
///
/// * [pl:map:cbuild_map_t*] Map object.
/// * [pl:key:any] Value of a key. Need to be copyable via `=`.
///
/// [r:] Pointer to a pair structure or NULL if not found.
#define cbuild_map_get(map, key)                                               \
	({                                                                           \
		__auto_type __cbuild_tmp_key = key;                                        \
		cbuild_assert(sizeof(__cbuild_tmp_key) == (map)->key_size ||               \
			((map)->key_size == 0 && sizeof(__cbuild_tmp_key) == sizeof(char*)),     \
			"Key type mismatch!\n");                                                 \
		cbuild_map_get_raw((map), &__cbuild_tmp_key);                              \
	})
/// Get element from a map.
///
/// This function will allocate one stack variable that will hold copy of pointer
/// to a key. This function will check if size of this variable matches 
/// `map->key_size`{.c}.
///
/// * [pl:map:cbuild_map_t*] Map object.
/// * [pl:key:const void*] Pointer to a key. Need to contain `map->key_size`{.c} bytes.
///
/// [r:] Pointer to a pair structure or NULL if not found.
#define cbuild_map_get_ptr(map, key)                                           \
	({                                                                           \
		__auto_type __cbuild_tmp_key = key;                                        \
		cbuild_assert(sizeof(*__cbuild_tmp_key) == (map)->key_size ||              \
			((map)->key_size == 0 && sizeof(*__cbuild_tmp_key) == sizeof(char*)),    \
			"Key type mismatch!\n");                                                 \
		cbuild_map_get_raw((map), __cbuild_tmp_key);                               \
	})
/// Get element from a map or allocate a new one. Raw semi-internal function.
///
/// ::: note
/// This will not insert key into an element. You will get full pair structure
/// that is at best zero-initialized. If you are inserted value you may copy key
/// unconditionally. If you key is small you can do this too. If key is big and
/// it may take a lot of time for copy you can have some checks to verify that
/// key is valid.
/// :::
///
/// * [pl:map] Map object.
/// * [pl:key] Pointer to a key. Need to contain `map->key_size`{.c} bytes.
///
/// [r:] Pointer to a pair structure or NULL if not found.
CBUILDDEF void* cbuild_map_get_or_alloc_raw(cbuild_map_t* map, const void* key);
/// Get element from a map or allocate a new one. Raw semi-internal function.
///
/// ::: note
/// This will not insert key into an element. You will get full pair structure
/// that is at best zero-initialized. If you are inserted value you may copy key
/// unconditionally. If you key is small you can do this too. If key is big and
/// it may take a lot of time for copy you can have some checks to verify that
/// key is valid.
/// :::
///
/// This function will allocate one stack variable that will hold copy of key.
/// This function will check if size of this variable matches `map->key_size`{.c}.
///
/// * [pl:map:cbuild_map_t*] Map object.
/// * [pl:key:any] Value of a key. Need to be copyable via `=`.
///
/// [r:] Pointer to a pair structure or NULL if not found.
#define cbuild_map_get_or_alloc(map, key)                                      \
	({                                                                           \
		__auto_type __cbuild_tmp_key = key;                                        \
		cbuild_assert(sizeof(__cbuild_tmp_key) == (map)->key_size ||               \
			((map)->key_size == 0 && sizeof(__cbuild_tmp_key) == sizeof(char*)),     \
			"Key type mismatch!\n");                                                 \
		cbuild_map_get_or_alloc_raw((map), &__cbuild_tmp_key);                     \
	})
/// Get element from a map or allocate a new one. Raw semi-internal function.
///
/// ::: note
/// This will not insert key into an element. You will get full pair structure
/// that is at best zero-initialized. If you are inserted value you may copy key
/// unconditionally. If you key is small you can do this too. If key is big and
/// it may take a lot of time for copy you can have some checks to verify that
/// key is valid.
/// :::
///
/// This function will allocate one stack variable that will hold copy of pointer
/// to a key. This function will check if size of this variable matches 
/// `map->key_size`{.c}.
///
/// * [pl:map:cbuild_map_t*] Map object.
/// * [pl:key:const void*] Pointer to a key. Need to contain `map->key_size`{.c} bytes.
///
/// [r:] Pointer to a pair structure or NULL if not found.
#define cbuild_map_get_or_alloc_ptr(map, key)                                  \
	({                                                                           \
		__auto_type __cbuild_tmp_key = key;                                        \
		cbuild_assert(sizeof(*__cbuild_tmp_key) == (map)->key_size ||              \
			((map)->key_size == 0 && sizeof(*__cbuild_tmp_key) == sizeof(char*)),    \
			"Key type mismatch!\n");                                                 \
		cbuild_map_get_or_alloc_raw((map), __cbuild_tmp_key);                      \
	})
/// Remove element from a map. Custom clear func will be used if provided in
/// the map. Internal data will be freed automatically. Semi-internal function.
///
/// * [pl:map] Map object.
/// * [pl:key] Pointer to a key. Need to contain `map->key_size`{.c} bytes.
///
/// [r:] `false` if key was not found.
CBUILDDEF bool cbuild_map_remove_raw(cbuild_map_t* map, const void* key);
/// Remove element from a map. Custom clear func will be used if provided in
/// the map. Internal data will be freed automatically.
///
/// This function will allocate one stack variable that will hold copy of key.
/// This function will check if size of this variable matches `map->key_size`{.c}.
///
/// * [pl:map:cbuild_map_t*] Map object.
/// * [pl:key:any] Value of a key. Need to be copyable via `=`.
///
/// [r:] `false` if key was not found.
#define cbuild_map_remove(map, key)                                            \
	({                                                                           \
		__auto_type __cbuild_tmp_key = key;                                        \
		cbuild_assert(sizeof(__cbuild_tmp_key) == (map)->key_size ||               \
			((map)->key_size == 0 && sizeof(__cbuild_tmp_key) == sizeof(char*)),     \
			"Key type mismatch!\n");                                                 \
		cbuild_map_remove_raw((map), &__cbuild_tmp_key);                           \
	})
/// Remove element from a map. Custom clear func will be used if provided in
/// the map. Internal data will be freed automatically.
///
/// This function will allocate one stack variable that will hold copy of pointer
/// to a key. This function will check if size of this variable matches 
/// `map->key_size`{.c}.
///
/// * [pl:map:cbuild_map_t*] Map object.
/// * [pl:key:const void*] Pointer to a key. Need to contain `map->key_size`{.c} bytes.
///
/// [r:] `false` if key was not found.
#define cbuild_map_remove_ptr(map, key)                                        \
	({                                                                           \
		__auto_type __cbuild_tmp_key = key;                                        \
		cbuild_assert(sizeof(*__cbuild_tmp_key) == (map)->key_size ||              \
			((map)->key_size == 0 && sizeof(*__cbuild_tmp_key) == sizeof(char*)),    \
			"Key type mismatch!\n");                                                 \
		cbuild_map_remove_raw((map), __cbuild_tmp_key);                            \
	})
/// Fully clear the map. Custom clear func will be used if provided in the map.
/// Internal data will be freed automatically.
CBUILDDEF void cbuild_map_clear(cbuild_map_t* map);
/// Reset map allocator.
CBUILDDEF void cbuild_map_iter_reset(cbuild_map_t* map);
/// Get pointer to a next map element from iterator. `NULL` will be returned when
/// iterator is empty.
///
/// ::: note
/// Removed or addition of elements in map without reset of iterator is UB.
/// While allocation of a new element should not break iterator, it is not
/// known if new element will be iterated over.
/// :::
CBUILDDEF void* cbuild_map_iter_next(cbuild_map_t* map);
/// Foreach loop over map. Based on iterator. This is raw semi-internal function.
///
/// * [pl:map:const cbuild_map_t*] Map object.
/// * [pl:iter:name] Name of variable that will be used in iteration. Will contain `void*`{.c} value.
#define	cbuild_map_foreach_raw(map, iter)                                      \
	cbuild_map_iter_reset(map);                                                  \
	for (void* iter = NULL; ((iter) = cbuild_map_iter_next(map));)
/// Foreach loop over map. Based on iterator.
///
/// * [pl:map:const cbuild_map_t*] Map object.
/// * [pl:T:type] Type of iterator variable.
/// * [pl:iter:name] Name of variable that will be used in iteration. Will contain `T`{.c} value.
#define cbuild_map_foreach(map, T, iter)                                       \
	cbuild_map_iter_reset(map);                                                  \
	for (T* iter = NULL;((iter) = (T*)cbuild_map_iter_next(map));)

//! # Internal functions [line:cbuild-map-internal]

//@ cbuild-map-internal

/// Default map hash function. Use dbj2 hash.
CBUILDDEF size_t __cbuild_map_hash_func(const void* data, size_t len);
