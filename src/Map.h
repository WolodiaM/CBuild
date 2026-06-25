#pragma once // For LSP 
//! Hasmap implementation in C.
//!
//! License: `GPL-3.0-or-later`.
//!
//! This implements [open-addressing](https://en.wikipedia.org/wiki/Open_addressing)
//! hashmap built on top of dynamic array. Below there is a few details about
//! implementation.
//!
//! # No resize
//!
//! Resize is complex and slow. User need to perform it manually, as map can
//! overflow. Each append operation may report failure. Also, you may manually
//! calculate map utilization (both high number of *used* elements and a lot
//! of *deleted* elements are bad signals).
//!
//! # Creating your map
//!
//! Underlying store is just a dynamic array with a few more fields:
//!
//! ```cpp
//! typedef struct cbuild_map_t {
//!     cbuild_map_pair_t* data;
//!     size_t size;
//!     size_t capacity;
//!     cbuild_map_hash_t hash;
//!     cbuild_map_keycmp_t keycmp;
//!     cbuild_map_clear_t clear;
//! } cbuild_map_t;
//! ```
//!
//! You can init all function pointers to default values using one of
//! `cbuild_map_init_*` functions.
//!
//! Pair structure also need to have some fields:
//!
//! ```cpp
//! typedef struct cbuild_map_pair_t {
//!     T key;
//!     cbuild_map_tombstone_t tombstone;
//! } cbuild_map_pair_t;
//! ```
//!
//! Key can be any value, but you need to use `init` function that support
//! specific type of keys.
//!
//! # Backing memory
//!
//! Backing memory is a dynamic array. It allows to do a few things for maps
//! (like freeing it, iterating over all elements, accessing elements if you
//! have index, etc). But you should set `size` to `capacity` after resize
//! for this to properly work. Also, capacity must be power-of-2

#include "Common.h"
#include "DynArray.h"
_Static_assert(SIZE_WIDTH == 64, "Libary require 64-bit size_t.");

/// Hash function for a key.
///
/// * [pl:map] Pointer to map object. Can be used to retrieve extra data.
/// * [pl:key] Pointer to a start of key (start of pair structure).
///
/// [r:] Hash of this specific key. Will be truncated via `& (map.capacity - 1)`{.c}.
typedef size_t (*cbuild_map_hash_t)(const void* map, const void* key, size_t klen);
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
typedef bool (*cbuild_map_keycmp_t)(const void* map,
	const void* k1, const void* k2, size_t klen);
/// Element clear function.
///
/// * [pl:map] Pointer to map object. Can be used to retrieve extra data.
/// * [pl:elem] Element that should be cleared. Pointer to a start of a pair structure.
typedef void (*cbuild_map_clear_t)(const void* map, void* elem);
/// Tombstones enum
typedef enum cbuild_map_tombstone_t {
	CBUILD_MAP_EMPTY = 0,
	CBUILD_MAP_FULL,
	CBUILD_MAP_DELETED,
} cbuild_map_tombstone_t;
/// Initialize map which uses numbers as keys (number can be arbitrary in size
/// and is more like "a random binary blob").
///
/// * [pl:map:cbuild_map_t*] Map object.
#define cbuild_map_init_num(map)        \
	(map)->hash = __cbuild_map_num_hash;    \
	(map)->keycmp = __cbuild_map_num_keycmp;
/// Initialize map which uses c-strings as keys.
///
/// * [pl:map:cbuild_map_t*] Map object.
#define cbuild_map_init_cstr(map)        \
	(map)->hash = __cbuild_map_cstr_hash;    \
	(map)->keycmp = __cbuild_map_cstr_keycmp;
/// Initialize map which uses string views as keys.
///
/// * [pl:map:cbuild_map_t*] Map object.
#define cbuild_map_init_sv(map)        \
	(map)->hash = __cbuild_map_sv_hash;    \
	(map)->keycmp = __cbuild_map_sv_keycmp;
/// Initialize map which uses string builders as keys.
///
/// * [pl:map:cbuild_map_t*] Map object.
#define cbuild_map_init_sb(map)        \
	(map)->hash = __cbuild_map_sb_hash;    \
	(map)->keycmp = __cbuild_map_sb_keycmp;
/// Find element in a map. This function is guaranteed to not modify map.
///
/// This function may return NULL if element is not in a map.
///
/// * [pl:map:cbuild_map_t*] Map object.
/// * [pl:key_:typeof(map->data[0].key)] Key.
///
/// [r:typeof(map->data)] Pointer to a pair.
#define cbuild_map_find(map, key_)                                           \
	({                                                                         \
		typeof((map)->data) __map_result = NULL;                                 \
		typeof((map)->data[0].key) __map_k = key_;                               \
		size_t __map_ksize = sizeof((map)->data[0].key);                         \
		size_t __map_hash = (map)->hash((map), &__map_k, __map_ksize);           \
		size_t __map_step = __cbuild_map_step(__map_hash);                       \
		for (size_t __map_i = 0; __map_i < (map)->capacity;                      \
			__map_i++, __map_hash += __map_step) {                                 \
			size_t __map_idx = __map_hash & ((map)->capacity - 1);                 \
			if ((map)->data[__map_idx].tombstone == CBUILD_MAP_EMPTY) {            \
				break;                                                               \
			} else if ((map)->data[__map_idx].tombstone == CBUILD_MAP_FULL) {      \
				if ((map)->keycmp((map), &(map)->data[__map_idx].key,                \
					&__map_k, __map_ksize)) {                                          \
					__map_result = &(map)->data[__map_idx];                            \
					break;                                                             \
				}                                                                    \
			}                                                                      \
		}                                                                        \
		__map_result;                                                            \
	})
/// Find element in a map. This function is guaranteed to not modify map.
///
/// This function may return `(size_t)-1` if element is not in a map.
///
/// * [pl:map:cbuild_map_t*] Map object.
/// * [pl:key_:typeof(map->data[0].key)] Key.
///
/// [r:size_t] Pointer to a pair.
#define cbuild_map_find_idx(map, key_)                                  \
	({                                                                    \
		size_t __map_res_idx = (size_t)-1;                                  \
		typeof((map)->data[0].key) __map_k = key_;                          \
		size_t __map_ksize = sizeof((map)->data[0].key);                    \
		size_t __map_hash = (map)->hash((map), &__map_k, __map_ksize);      \
		size_t __map_step = __cbuild_map_step(__map_hash);                  \
		for (size_t __map_i = 0; __map_i < (map)->capacity;                 \
			__map_i++, __map_hash += __map_step) {                            \
			size_t __map_idx = __map_hash & ((map)->capacity - 1);            \
			if ((map)->data[__map_idx].tombstone == CBUILD_MAP_EMPTY) {       \
				break;                                                          \
			} else if ((map)->data[__map_idx].tombstone == CBUILD_MAP_FULL) { \
				if ((map)->keycmp((map), &(map)->data[__map_idx].key,           \
					&__map_k, __map_ksize)) {                                     \
					__map_res_idx = __map_idx;                                    \
					break;                                                        \
				}                                                               \
			}                                                                 \
		}                                                                   \
		__map_res_idx;                                                      \
	})
/// Set element in map. This will allocate new element (even if element with
/// same key exists in map already). This function can be a little faster
/// than `cbuild_map_get` as it returns on first EMPTY/DELETED slot found,
/// and `cbuild_map_get` exits only on EMPTY slow or when key is found.
///
/// This function will copy key into new pair using `memcpy`.
///
/// This function may return NULL if map overflows.
///
/// * [pl:map:cbuild_map_t*] Map object.
/// * [pl:key_:typeof(map->data[0].key)] Key.
///
/// [r:typeof(map->data)] Pointer to a pair.
#define cbuild_map_append(map, key_)                               \
	({                                                               \
		typeof((map)->data) __map_result = NULL;                       \
		typeof((map)->data[0].key) __map_k = key_;                     \
		size_t __map_ksize = sizeof((map)->data[0].key);               \
		size_t __map_hash = (map)->hash((map), &__map_k, __map_ksize); \
		size_t __map_step = __cbuild_map_step(__map_hash);             \
		for (size_t __map_i = 0; __map_i < (map)->capacity;            \
			__map_i++, __map_hash += __map_step) {                       \
			size_t __map_idx = __map_hash & ((map)->capacity - 1);       \
			if ((map)->data[__map_idx].tombstone == CBUILD_MAP_EMPTY ||  \
				(map)->data[__map_idx].tombstone == CBUILD_MAP_DELETED) {  \
				__map_result = &(map)->data[__map_idx];                    \
				memcpy(&__map_result->key, &__map_k, __map_ksize);         \
				__map_result->tombstone = CBUILD_MAP_FULL;                 \
				break;                                                     \
			}                                                            \
		}                                                              \
		__map_result;                                                  \
	})
/// Set element in map. This will allocate new element if key is not already in
/// map.
///
/// This function will copy key into new pair using `memcpy`.
///
/// This function may return NULL if map overflows.
///
/// * [pl:map:cbuild_map_t*] Map object.
/// * [pl:key_:typeof(map->data[0].key)] Key.
///
/// [r:typeof(map->data)] Pointer to a pair.
#define cbuild_map_get(map, key_)                                          \
	({                                                                       \
		typeof((map)->data) __map_result = NULL;                               \
		typeof((map)->data) __first_empty = NULL;                              \
		typeof((map)->data[0].key) __map_k = key_;                             \
		size_t __map_ksize = sizeof((map)->data[0].key);                       \
		size_t __map_hash = (map)->hash((map), &__map_k, __map_ksize);         \
		size_t __map_step = __cbuild_map_step(__map_hash);                     \
		for (size_t __map_i = 0; __map_i < (map)->capacity;                    \
			__map_i++, __map_hash += __map_step) {                               \
			size_t __map_idx = __map_hash & ((map)->capacity - 1);               \
			if ((map)->data[__map_idx].tombstone == CBUILD_MAP_EMPTY) {          \
				if (!__first_empty) __first_empty = &(map)->data[__map_idx];       \
				break;                                                             \
			} else if ((map)->data[__map_idx].tombstone == CBUILD_MAP_DELETED) { \
				if (!__first_empty) __first_empty = &(map)->data[__map_idx];       \
			} else if ((map)->data[__map_idx].tombstone == CBUILD_MAP_FULL) {    \
				if ((map)->keycmp((map), &(map)->data[__map_idx].key,              \
					&__map_k, __map_ksize)) {                                        \
					__map_result = &(map)->data[__map_idx];                          \
					break;                                                           \
				}                                                                  \
			}                                                                    \
		}                                                                      \
		if (!__map_result && __first_empty) {                                  \
			__map_result = __first_empty;                                        \
			memcpy(&__map_result->key, &__map_k, __map_ksize);                   \
			__map_result->tombstone = CBUILD_MAP_FULL;                           \
		}                                                                      \
		__map_result;                                                          \
	})
/// Rehash elements from one map into another. New map is assumed to be larger
/// than old one.
///
/// * [pl:map1:cbuild_map_t*] Source map object.
/// * [pl:map2:cbuild_map_t*] Destination map object.
#define cbuild_map_rehash(map1, map2)                                             \
	do {                                                                            \
		cbuild_da_foreach((map1), __map_e1) {                                         \
			if (__map_e1->tombstone == CBUILD_MAP_FULL) {                               \
				typeof((map1)->data) __map_e2 = cbuild_map_append((map2), __map_e1->key); \
				memcpy(__map_e2, __map_e1, sizeof(*__map_e1));                            \
			}                                                                           \
		}                                                                             \
		cbuild_da_clear(map1);                                                        \
	} while(0)
/// Resizes the map, sets `size` and zero-inits memory.
///
/// This function will automatically round-up size to next power-of-2.
///
/// * [pl:map:cbuild_map_t*] Map object.
/// * [pl:new_elems:size_t] New size.
#define cbuild_map_resize(map, new_elems)                               \
	do {                                                                  \
		size_t __map_elems = new_elems;                                     \
		__map_elems--;                                                      \
		__map_elems |= __map_elems >> 1; __map_elems |= __map_elems >> 2;   \
		__map_elems |= __map_elems >> 4; __map_elems |= __map_elems >> 8;   \
		__map_elems |= __map_elems >> 16; __map_elems |= __map_elems >> 32; \
		__map_elems++;                                                      \
		cbuild_da_resize(map, __map_elems);                                 \
		(map)->size = (map)->capacity;                                      \
		memset((map)->data, 0, (map)->size * sizeof(*(map)->data));         \
	} while(0)

//! # Internal functions [line:cbuild-map-internal]

//@ cbuild-map-internal

/// Base hashing function. Uses dbj2 hash.
CBUILDDEF size_t __cbuild_map_hash_func(const void* data, size_t len);
///
CBUILDDEF size_t __cbuild_map_num_hash(const void* map,
	const void* key, size_t klen);
///
CBUILDDEF size_t __cbuild_map_cstr_hash(const void* map,
	const void* key, size_t klen);
///
CBUILDDEF size_t __cbuild_map_sv_hash(const void* map,
	const void* key, size_t klen);
///
CBUILDDEF size_t __cbuild_map_sb_hash(const void* map,
	const void* key, size_t klen);
///
CBUILDDEF bool __cbuild_map_num_keycmp(const void* map,
	const void* k1, const void* k2, size_t klen);
///
CBUILDDEF bool __cbuild_map_cstr_keycmp(const void* map,
	const void* k1, const void* k2, size_t klen);
///
CBUILDDEF bool __cbuild_map_sv_keycmp(const void* map,
	const void* k1, const void* k2, size_t klen);
///
CBUILDDEF bool __cbuild_map_sb_keycmp(const void* map,
	const void* k1, const void* k2, size_t klen);
/// Implements Splitmix64.
CBUILDDEF size_t __cbuild_map_step(size_t prev);
