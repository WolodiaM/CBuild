/**
 * @file Map.h
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Map data structure
 * Implemented using hashmap.
 *
 * @date 2025-01-02
 * @copyright (C) 2025 WolodiaM
 * @license MIT
 *
 * Copyright (C) 2025 WolodiaM
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef __CBUILD_MAP_H__
#define __CBUILD_MAP_H__
// Project includes
#include "common.h"
#include "Log.h"
// Code
typedef struct cbuild_map_bucket_t {
	void* vals;
	size_t nvals;
} cbuild_map_bucket_t;
typedef size_t (*cbuild_map_hash_t)(const void* map, const void* key);
typedef bool (*cbuild_map_keycmp_t)(const void* map, const void* k1,
  const void* k2);
typedef void (*cbuild_map_elem_clear_t)(const void* map, void* elem);
/**
 * @brief Main map datatype
 * User shoudl set elem_size or key_size. Elem size is a size of a full elem
 * struct - key + value. Key size is a size of a key only. If 0 key is assumed
 * to be a 'char*' pointer to a c-string.
 * Hash and key compare function can be overwrittn. Default support any inline
 * keys (number, array, etc. uses memcmp and hash on a raw block of bytes) and
 * c-strings.
 */
typedef struct cbuild_map_t {
	cbuild_map_bucket_t* buckets;
	size_t nbuckets;
	cbuild_map_hash_t hash_func;
	cbuild_map_keycmp_t keycmp_func;
	size_t key_size;
	size_t elem_size;
	size_t iter_buckets;
	size_t iter_vals;
} cbuild_map_t;
/**
 * @brief Init a map to specifc capacity. Do not rehash.
 *
 * @param map => cbuild_map_t* -> Map object
 * @param nbuckets => size_t -> New bucket count
 */
void cbuild_map_init(cbuild_map_t* map, size_t nbuckets);
/**
 * @brief Get element from a map
 *
 * @param map => cbuild_map_t* -> Map object
 * @param key => void* -> Requested key
 * @return void* -> Element of NULL if not found
 */
void* cbuild_map_get_raw(cbuild_map_t* map, const void* key);
/**
 * @brief Get element from a map
 *
 * @param map => cbuild_map_t* -> Map object
 * @param key => T -> Requested key
 * @return void* -> Element of NULL if not found
 */
#define cbuild_map_get(map, key)                                               \
	({                                                                           \
		__auto_type __cbuild_tmp_key = key;                                        \
		cbuild_assert(sizeof(__cbuild_tmp_key) == (map)->key_size ||               \
		  ((map)->key_size == 0 && sizeof(__cbuild_tmp_key) == sizeof(char*)),     \
		  "(LIB_CBUILD_MAP) Key type mismatch!\n");                                \
		cbuild_map_get_raw((map), &__cbuild_tmp_key);                              \
	})
/**
 * @brief Get element from a map
 *
 * @param map => cbuild_map_t* -> Map object
 * @param key => T* -> Requested key
 * @return void* -> Element of NULL if not found
 */
#define cbuild_map_get_ptr(map, key)                                           \
	({                                                                           \
		__auto_type __cbuild_tmp_key = key;                                        \
		cbuild_assert(sizeof(*__cbuild_tmp_key) == (map)->key_size ||              \
		  ((map)->key_size == 0 && sizeof(*__cbuild_tmp_key) == sizeof(char*)),    \
		  "(LIB_CBUILD_MAP) Key type mismatch!\n");                                \
		cbuild_map_get_raw((map), __cbuild_tmp_key);                               \
	})
/**
 * @brief Get element from a map or allocate new
 *
 * @param map => cbuild_map_t* -> Map object
 * @param key => void* -> Requested key
 * @return void* -> Element
 */
void* cbuild_map_get_or_alloc_raw(cbuild_map_t* map, const void* key);
/**
 * @brief Get element from a map or allocate new
 *
 * @param map => cbuild_map_t* -> Map object
 * @param key => T -> Requested key
 * @return void* -> Element
 */
#define cbuild_map_get_or_alloc(map, key)                                      \
	({                                                                           \
		__auto_type __cbuild_tmp_key = key;                                        \
		cbuild_assert(sizeof(__cbuild_tmp_key) == (map)->key_size ||               \
		  ((map)->key_size == 0 && sizeof(__cbuild_tmp_key) == sizeof(char*)),     \
		  "(LIB_CBUILD_MAP) Key type mismatch!\n");                                \
		cbuild_map_get_or_alloc_raw((map), &__cbuild_tmp_key);                     \
	})
/**
 * @brief Get element from a map or allocate new
 *
 * @param map => cbuild_map_t* -> Map object
 * @param key => T* -> Requested key
 * @return void* -> Element
 */
#define cbuild_map_get_or_alloc_ptr(map, key)                                  \
	({                                                                           \
		__auto_type __cbuild_tmp_key = key;                                        \
		cbuild_assert(sizeof(*__cbuild_tmp_key) == (map)->key_size ||              \
		  ((map)->key_size == 0 && sizeof(*__cbuild_tmp_key) == sizeof(char*)),    \
		  "(LIB_CBUILD_MAP) Key type mismatch!\n");                                \
		cbuild_map_get_or_alloc_raw((map), __cbuild_tmp_key);                      \
	})
/**
 * @brief Remove element from a map. Userdata should be memcpy-safe.
 *
 * @param map => cbuild_map_t* -> Map object
 * @param key => void* -> Requested key
 * @param elem_clear_func => cbuild_map_elem_clear_t -> Function that will clear
 * element. Can be NULL.
 * @return bool -> False if key not found
 */
bool cbuild_map_remove_ex_raw(cbuild_map_t* map, const void* key,
  cbuild_map_elem_clear_t elem_clear_func);
/**
 * @brief Remove element from a map. Userdata should be memcpy-safe.
 *
 * @param map => cbuild_map_t* -> Map object
 * @param key => T -> Requested key
 * @param elem_clear_func => cbuild_map_elem_clear_t -> Function that will clear
 * element. Can be NULL.
 * @return bool -> False if key not found
 */
#define cbuild_map_remove_ex(map, key, elem_clear_func)                        \
	({                                                                           \
		__auto_type __cbuild_tmp_key = key;                                        \
		cbuild_assert(sizeof(__cbuild_tmp_key) == (map)->key_size ||               \
		  ((map)->key_size == 0 && sizeof(__cbuild_tmp_key) == sizeof(char*)),     \
		  "(LIB_CBUILD_MAP) Key type mismatch!\n");                                \
		cbuild_map_remove_ex_raw((map), &__cbuild_tmp_key, elem_clear_func);       \
	})
/**
 * @brief Remove element from a map. Userdata should be memcpy-safe.
 *
 * @param map => cbuild_map_t* -> Map object
 * @param key => T* -> Requested key
 * @param elem_clear_func => cbuild_map_elem_clear_t -> Function that will clear
 * element. Can be NULL.
 * @return bool -> False if key not found
 */
#define cbuild_map_remove_ex_ptr(map, key, elem_clear_func)                    \
	({                                                                           \
		__auto_type __cbuild_tmp_key = key;                                        \
		cbuild_assert(sizeof(*__cbuild_tmp_key) == (map)->key_size ||              \
		  ((map)->key_size == 0 && sizeof(*__cbuild_tmp_key) == sizeof(char*)),    \
		  "(LIB_CBUILD_MAP) Key type mismatch!\n");                                \
		cbuild_map_remove_ex_raw((map), __cbuild_tmp_key, elem_clear_func);        \
	})
/**
 * @brief Remove element from a map. Userdata should be memcpy-safe.
 *
 * @param map => cbuild_map_t* -> Map object
 * @param key => void* -> Requested key
 * @return bool -> False if key not found
 */
#define cbuild_map_remove_raw(map, key) cbuild_map_remove_ex_raw(map, key, NULL)
/**
 * @brief Remove element from a map. Userdata should be memcpy-safe.
 *
 * @param map => cbuild_map_t* -> Map object
 * @param key => T -> Requested key
 * @return bool -> False if key not found
 */
#define cbuild_map_remove(map, key) cbuild_map_remove_ex(map, key, NULL)
/**
 * @brief Remove element from a map. Userdata should be memcpy-safe.
 *
 * @param map => cbuild_map_t* -> Map object
 * @param key => T* -> Requested key
 * @return bool -> False if key not found
 */
#define cbuild_map_remove_ptr(map, key) cbuild_map_remove_ex_ptr(map, key, NULL)
/**
 * @brief Fully clear and deallocate map
 *
 * @param map => cbuild_map_t* -> Map object
 * @param elem_cear_func => cbuild_map_elem_clear_t -> Function that will clear
 * element. Can be called on all allocated elements. Can be NULL.
 */
void cbuild_map_clear_ex(cbuild_map_t* map,
  cbuild_map_elem_clear_t elem_clear_func);
/**
 * @brief Fully clear and deallocate map
 *
 * @param map => cbuild_map_t* -> Map object
 */
#define cbuild_map_clear(map) cbuild_map_clear_ex(map, NULL)
/**
 * @brief Reset map iterator
 *
 * @param map => cbuild_map_t -> Map object
 */
void cbuild_map_iter_reset(cbuild_map_t* map);
/**
 * @brief Get next map element
 * Note: Removing invalidates iterator (undefined behaviour if iterator is used
 * after remove without explicit reset)
 * Note: Map element allocations do not invalidate iterator, but it is undefined
 * behaviour if new element will be iterated or not (it may be iterated, or may
 * be not iterated)
 * @param map => cbuild_map_t -> Map object
 * @return void* -> Element pointer or NULL if end reached
 */
void* cbuild_map_iter_next(cbuild_map_t* map);
/**
 * @brief For-each loop over a map
 *
 * @param map => cbuild_map_t* -> Map
 * @param iter => NAME -> Name of the iteration variable (void*)
 */
#define cbuild_map_foreach_raw(map, iter)                                      \
	cbuild_map_iter_reset(map);                                                  \
	for (void* iter = NULL; ((iter) = cbuild_map_iter_next(map));)
/**
 * @brief For-each loop over a map
 *
 * @param map => cbuild_map_t* -> Map
 * @param T => TYPE -> Type map userdata
 * @param iter => NAME -> Name of the iteration variable (T*)
 */
#define cbuild_map_foreach(map, T, iter)                                       \
	cbuild_map_iter_reset(map);                                                  \
	for (T* iter = NULL;((iter) = (T*)cbuild_map_iter_next(map));)
#endif // __CBUILD_MAP_H__
