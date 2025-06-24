/**
 * @file Map.h
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Map data structure
 * Implemented using hashmap. Uses template-like macro system. You specify
 * datatypes for key and value and "names" for this datatypes and then datypes
 * are used in code generation and names are used to generate unique names for
 * all fucntions and structs.
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
#define cbuild_map_t(K, V, Kname, Vname)                                       \
	typedef struct cbuild_map_##Kname##_##Vname##_pair_t {                       \
		K key;                                                                     \
		V val;                                                                     \
	} cbuild_map_##Kname##_##Vname##_pair_t;                                     \
	typedef struct cbuild_map_##Kname##_##Vname##_bucket_t {                     \
		cbuild_map_##Kname##_##Vname##_pair_t* vals;                               \
		size_t                                 nvals;                              \
	} cbuild_map_##Kname##_##Vname##_bucket_t;                                   \
	typedef cbuild_map_##Kname##_##Vname##_pair_t* (                             \
			*cbuild_map_##Kname##_##Vname##_iter_next_t)(void* self);                \
	typedef struct cbuild_map_##Kname##_##Vname##_iter_t {                       \
		void*                                      map;                            \
		size_t                                     bptr;                           \
		size_t                                     vptr;                           \
		cbuild_map_##Kname##_##Vname##_iter_next_t next;                           \
	} cbuild_map_##Kname##_##Vname##_iter_t;                                     \
	typedef size_t (*cbuild_map_##Kname##_##Vname##_hash_t)(K * key);            \
	typedef bool (*cbuild_map_##Kname##_##Vname##_keycmp_t)(K * k1, K * k2);     \
	typedef void* (*cbuild_map_##Kname##_##Vname##_malloc_t)(size_t bytes);      \
	typedef void* (*cbuild_map_##Kname##_##Vname##_realloc_t)(void*  ptr,        \
	                                                          size_t bytes);     \
	typedef void (*cbuild_map_##Kname##_##Vname##_free_t)(void* ptr);            \
	typedef V* (*cbuild_map_##Kname##_##Vname##_get_t)(void* self, K key);       \
	typedef bool (*cbuild_map_##Kname##_##Vname##_remove_t)(void* self, K key);  \
	typedef bool (*cbuild_map_##Kname##_##Vname##_set_t)(void* self, K key,      \
	                                                     V val);                 \
	typedef bool (*cbuild_map_##Kname##_##Vname##_contains_t)(void* self,        \
	                                                          K     key);            \
	typedef K* (*cbuild_map_##Kname##_##Vname##_keylist_t)(void*   self,         \
	                                                       size_t* num);         \
	typedef void (*cbuild_map_##Kname##_##Vname##_resize_t)(void*  self,         \
	                                                        size_t nbuckets);    \
	typedef void (*cbuild_map_##Kname##_##Vname##_clear_t)(void* self);          \
	typedef cbuild_map_##Kname##_##Vname##_iter_t (                              \
			*cbuild_map_##Kname##_##Vname##_get_iterator_t)(void* self);             \
	typedef size_t (*cbuild_map_##Kname##_##Vname##_profile_t)(void* self);      \
	typedef struct cbuild_map_##Kname##_##Vname##_t {                            \
		cbuild_map_##Kname##_##Vname##_bucket_t*      buckets;                     \
		size_t                                        nbuckets;                    \
		cbuild_map_##Kname##_##Vname##_hash_t         hash;                        \
		cbuild_map_##Kname##_##Vname##_keycmp_t       keycmp;                      \
		cbuild_map_##Kname##_##Vname##_malloc_t       malloc;                      \
		cbuild_map_##Kname##_##Vname##_realloc_t      realloc;                     \
		cbuild_map_##Kname##_##Vname##_free_t         free;                        \
		cbuild_map_##Kname##_##Vname##_get_t          get;                         \
		cbuild_map_##Kname##_##Vname##_set_t          set;                         \
		cbuild_map_##Kname##_##Vname##_remove_t       remove;                      \
		cbuild_map_##Kname##_##Vname##_contains_t     contains;                    \
		cbuild_map_##Kname##_##Vname##_keylist_t      keylist;                     \
		cbuild_map_##Kname##_##Vname##_resize_t       resize;                      \
		cbuild_map_##Kname##_##Vname##_clear_t        clear;                       \
		cbuild_map_##Kname##_##Vname##_get_iterator_t get_iterator;                \
		cbuild_map_##Kname##_##Vname##_profile_t      profile;                     \
	} cbuild_map_##Kname##_##Vname##_t
#define cbuild_map_t_ext_impl(Kname, Vname)                                    \
	extern const cbuild_map_##Kname##_##Vname##_t cbuild_map_##Kname##_##Vname
#define cbuild_map_t_impl(K, V, Kname, Vname)                                                                  \
	V* cbuild_map_##Kname##_##Vname##_get(void* s, K key) {                                                      \
		cbuild_map_##Kname##_##Vname##_t* self = s;                                                                \
		if (self->nbuckets == 0) {                                                                                 \
			cbuild_log(CBUILD_LOG_ERROR, "(LIB_CBUILD_MAP) Size of map is 0.");                                      \
			return CBUILD_PTR_ERR;                                                                                   \
		}                                                                                                          \
		size_t hash = self->hash(&key) % self->nbuckets;                                                           \
		cbuild_map_##Kname##_##Vname##_bucket_t* bucket = &(self->buckets[hash]);                                  \
		for (size_t i = 0; i < bucket->nvals; i++) {                                                               \
			if (self->keycmp(&key, &(bucket->vals[i].key))) {                                                        \
				return &(bucket->vals[i].val);                                                                         \
			}                                                                                                        \
		}                                                                                                          \
		return NULL;                                                                                               \
	}                                                                                                            \
	bool cbuild_map_##Kname##_##Vname##_set(void* s, K key, V val) {                                             \
		cbuild_map_##Kname##_##Vname##_t* self = s;                                                                \
		if (self->nbuckets == 0) {                                                                                 \
			cbuild_log(CBUILD_LOG_ERROR, "(LIB_CBUILD_MAP) Size of map is 0.");                                      \
			return false;                                                                                            \
		}                                                                                                          \
		size_t hash = self->hash(&key) % self->nbuckets;                                                           \
		cbuild_map_##Kname##_##Vname##_bucket_t* bucket = &(self->buckets[hash]);                                  \
		for (size_t i = 0; i < bucket->nvals; i++) {                                                               \
			if (self->keycmp(&key, &(bucket->vals[i].key))) {                                                        \
				bucket->vals[i].val = val;                                                                             \
				return true;                                                                                           \
			}                                                                                                        \
		}                                                                                                          \
		bucket->nvals++;                                                                                           \
		bucket->vals = self->realloc(                                                                              \
				bucket->vals,                                                                                          \
				bucket->nvals * sizeof(cbuild_map_##Kname##_##Vname##_pair_t));                                        \
		cbuild_assert(bucket->vals != NULL,                                                                        \
		              "(LIB_CBUILD_MAP) Allocation failed.\n");                                                    \
		bucket->vals[bucket->nvals - 1] =                                                                          \
				(cbuild_map_##Kname##_##Vname##_pair_t){ .key = key, .val = val };                                     \
		return true;                                                                                               \
	}                                                                                                            \
	bool cbuild_map_##Kname##_##Vname##_remove(void* s, K key) {                                                 \
		cbuild_map_##Kname##_##Vname##_t* self = s;                                                                \
		if (self->nbuckets == 0) {                                                                                 \
			cbuild_log(CBUILD_LOG_ERROR, "(LIB_CBUILD_MAP) Size of map is 0.");                                      \
			return false;                                                                                            \
		}                                                                                                          \
		size_t hash = self->hash(&key) % self->nbuckets;                                                           \
		cbuild_map_##Kname##_##Vname##_bucket_t* bucket   = &(self->buckets[hash]);                                \
		size_t                                   ridx     = 0;                                                     \
		bool                                     ridx_set = false;                                                 \
		for (size_t i = 0; i < bucket->nvals; i++) {                                                               \
			if (self->keycmp(&key, &(bucket->vals[i].key))) {                                                        \
				ridx     = i;                                                                                          \
				ridx_set = true;                                                                                       \
				break;                                                                                                 \
			}                                                                                                        \
		}                                                                                                          \
		if (ridx_set) {                                                                                            \
			if (ridx == bucket->nvals - 1) {                                                                         \
				bucket->nvals--;                                                                                       \
				bucket->vals = self->realloc(                                                                          \
						bucket->vals,                                                                                      \
						bucket->nvals * sizeof(cbuild_map_##Kname##_##Vname##_pair_t));                                    \
				if (bucket->nvals > 0) {                                                                               \
					cbuild_assert(bucket->vals != NULL,                                                                  \
					              "(LIB_CBUILD_MAP) Allocation failed.\n");                                              \
				}                                                                                                      \
				return true;                                                                                           \
			} else {                                                                                                 \
				cbuild_map_##Kname##_##Vname##_pair_t* old_ptr = bucket->vals;                                         \
				bucket->nvals--;                                                                                       \
				bucket->vals = self->malloc(                                                                           \
						bucket->nvals * sizeof(cbuild_map_##Kname##_##Vname##_pair_t));                                    \
				if (bucket->nvals > 0) {                                                                               \
					cbuild_assert(bucket->vals != NULL,                                                                  \
					              "(LIB_CBUILD_MAP) Allocation failed.\n");                                              \
				}                                                                                                      \
				size_t j = 0;                                                                                          \
				for (size_t i = 0; i <= bucket->nvals; i++) {                                                          \
					if (i != ridx) {                                                                                     \
						bucket->vals[j] = old_ptr[i];                                                                      \
						j++;                                                                                               \
					}                                                                                                    \
				}                                                                                                      \
				self->free(old_ptr);                                                                                   \
				return true;                                                                                           \
			}                                                                                                        \
		}                                                                                                          \
		return false;                                                                                              \
	}                                                                                                            \
	bool cbuild_map_##Kname##_##Vname##_contains(void* s, K key) {                                               \
		cbuild_map_##Kname##_##Vname##_t* self = s;                                                                \
		void*                             gptr = self->get(s, key);                                                \
		if (gptr != CBUILD_PTR_ERR && gptr != NULL) {                                                              \
			return true;                                                                                             \
		} else {                                                                                                   \
			return false;                                                                                            \
		}                                                                                                          \
	}                                                                                                            \
	K* cbuild_map_##Kname##_##Vname##_keylist(void* s, size_t* num) {                                            \
		cbuild_map_##Kname##_##Vname##_t* self = s;                                                                \
		*num                                   = 0;                                                                \
		size_t cap                             = self->nbuckets;                                                   \
		K*     list                            = self->malloc(cap * sizeof(K));                                    \
		cbuild_assert(list != NULL,                                                                                \
		              "(LIB_CBUILD_MAP) Auxiliary allocation failed.\n");                                          \
		for (size_t i = 0; i < self->nbuckets; i++) {                                                              \
			cbuild_map_##Kname##_##Vname##_bucket_t* bucket = &(self->buckets[i]);                                   \
			for (size_t j = 0; j < bucket->nvals; j++) {                                                             \
				list[(*num)++] = bucket->vals[j].key;                                                                  \
				if (cap == *num) {                                                                                     \
					cap  = cap * 2;                                                                                      \
					list = self->realloc(list, cap * sizeof(K));                                                         \
					cbuild_assert(list != NULL,                                                                          \
					              "(LIB_CBUILD_MAP) Auxiliary allocation failed.\n");                                    \
				}                                                                                                      \
			}                                                                                                        \
		}                                                                                                          \
		return list;                                                                                               \
	}                                                                                                            \
	void cbuild_map_##Kname##_##Vname##_resize(void* s, size_t nbuckets) {                                       \
		cbuild_map_##Kname##_##Vname##_t* self = s;                                                                \
		if (self->nbuckets == 0) {                                                                                 \
			self->nbuckets = nbuckets;                                                                               \
			self->buckets  = self->malloc(                                                                           \
          nbuckets * sizeof(cbuild_map_##Kname##_##Vname##_bucket_t));                                        \
			cbuild_assert(self->buckets != NULL,                                                                     \
			              "(LIB_CBUILD_MAP) Allocation failed.\n");                                                  \
			for (size_t i = 0; i < self->nbuckets; i++) {                                                            \
				self->buckets[i] = (cbuild_map_##Kname##_##Vname##_bucket_t){ 0 };                                     \
			}                                                                                                        \
		} else {                                                                                                   \
			size_t                                   old_nbuckets = self->nbuckets;                                  \
			cbuild_map_##Kname##_##Vname##_bucket_t* old_buckets  = self->buckets;                                   \
			self->nbuckets                                        = nbuckets;                                        \
			self->buckets                                         = self->malloc(                                    \
          nbuckets * sizeof(cbuild_map_##Kname##_##Vname##_bucket_t)); \
			cbuild_assert(self->buckets != NULL,                                                                     \
			              "(LIB_CBUILD_MAP) Allocation failed.\n");                                                  \
			for (size_t i = 0; i < self->nbuckets; i++) {                                                            \
				self->buckets[i] = (cbuild_map_##Kname##_##Vname##_bucket_t){ 0 };                                     \
			}                                                                                                        \
			for (size_t i = 0; i < old_nbuckets; i++) {                                                              \
				cbuild_map_##Kname##_##Vname##_bucket_t* bucket = &(old_buckets[i]);                                   \
				for (size_t j = 0; j < bucket->nvals; j++) {                                                           \
					self->set(s, bucket->vals[j].key, bucket->vals[j].val);                                              \
				}                                                                                                      \
			}                                                                                                        \
			self->free(old_buckets);                                                                                 \
		}                                                                                                          \
	}                                                                                                            \
	void cbuild_map_##Kname##_##Vname##_clear(void* s) {                                                         \
		cbuild_map_##Kname##_##Vname##_t* self = s;                                                                \
		for (size_t i = 0; i < self->nbuckets; i++) {                                                              \
			self->free(self->buckets[i].vals);                                                                       \
			self->buckets[i].nvals = 0;                                                                              \
		}                                                                                                          \
		self->free(self->buckets);                                                                                 \
		self->nbuckets = 0;                                                                                        \
	}                                                                                                            \
	cbuild_map_##Kname##_##Vname##_pair_t*                                                                       \
			cbuild_map_##Kname##_##Vname##_iter_next(void* s) {                                                      \
		cbuild_map_##Kname##_##Vname##_iter_t* self = s;                                                           \
		cbuild_map_##Kname##_##Vname##_t*      map  = self->map;                                                   \
		if (map->nbuckets > 0) {                                                                                   \
			if (self->bptr < map->nbuckets) {                                                                        \
				while (true) {                                                                                         \
					cbuild_map_##Kname##_##Vname##_bucket_t* bucket =                                                    \
							&map->buckets[self->bptr];                                                                       \
					if (bucket->nvals == 0) {                                                                            \
						self->bptr++;                                                                                      \
						self->vptr = 0;                                                                                    \
					} else {                                                                                             \
						if (self->vptr < bucket->nvals) {                                                                  \
							cbuild_map_##Kname##_##Vname##_pair_t* ret =                                                     \
									&bucket->vals[self->vptr];                                                                   \
							self->vptr++;                                                                                    \
							return ret;                                                                                      \
						} else {                                                                                           \
							self->vptr = 0;                                                                                  \
							self->bptr++;                                                                                    \
						}                                                                                                  \
					}                                                                                                    \
					if (self->bptr == map->nbuckets) {                                                                   \
						return NULL;                                                                                       \
					}                                                                                                    \
				}                                                                                                      \
			}                                                                                                        \
		}                                                                                                          \
		return NULL;                                                                                               \
	}                                                                                                            \
	cbuild_map_##Kname##_##Vname##_iter_t                                                                        \
			cbuild_map_##Kname##_##Vname##_get_iterator(void* s) {                                                   \
		return (cbuild_map_##Kname##_##Vname##_iter_t){                                                            \
			.map  = s,                                                                                               \
			.bptr = 0,                                                                                               \
			.vptr = 0,                                                                                               \
			.next = cbuild_map_##Kname##_##Vname##_iter_next,                                                        \
		};                                                                                                         \
	}                                                                                                            \
	size_t cbuild_map_##Kname##_##Vname##_profile(void* s) {                                                     \
		cbuild_map_##Kname##_##Vname##_t* self = s;                                                                \
		__CBUILD_PRINTF("Map has %zu buckets\n", self->nbuckets);                                                  \
		size_t ret = 0;                                                                                            \
		for (size_t i = 0; i < self->nbuckets; i++) {                                                              \
			cbuild_map_##Kname##_##Vname##_bucket_t* bucket = &(self->buckets[i]);                                   \
			if (bucket->nvals > ret) {                                                                               \
				ret = bucket->nvals;                                                                                   \
			}                                                                                                        \
			__CBUILD_PRINTF("Bucket %zu has %zu values\n", i, bucket->nvals);                                        \
		}                                                                                                          \
		return ret;                                                                                                \
	}                                                                                                            \
	const cbuild_map_##Kname##_##Vname##_t cbuild_map_##Kname##_##Vname = {                                      \
		.buckets      = NULL,                                                                                      \
		.nbuckets     = 0,                                                                                         \
		.hash         = cbuild_map_##Kname##_##Vname##_hash,                                                       \
		.keycmp       = cbuild_map_##Kname##_##Vname##_keycmp,                                                     \
		.malloc       = __CBUILD_MALLOC,                                                                           \
		.realloc      = __CBUILD_REALLOC,                                                                          \
		.free         = __CBUILD_FREE,                                                                             \
		.get          = cbuild_map_##Kname##_##Vname##_get,                                                        \
		.set          = cbuild_map_##Kname##_##Vname##_set,                                                        \
		.remove       = cbuild_map_##Kname##_##Vname##_remove,                                                     \
		.contains     = cbuild_map_##Kname##_##Vname##_contains,                                                   \
		.keylist      = cbuild_map_##Kname##_##Vname##_keylist,                                                    \
		.resize       = cbuild_map_##Kname##_##Vname##_resize,                                                     \
		.clear        = cbuild_map_##Kname##_##Vname##_clear,                                                      \
		.get_iterator = cbuild_map_##Kname##_##Vname##_get_iterator,                                               \
		.profile      = cbuild_map_##Kname##_##Vname##_profile,                                                    \
	}
/**
 * @brief Get element from a map
 *
 * @param map => CBUILD_MAP* -> Map pointer
 * @param key => KEY -> Key
 * @return VAL* -> Value or NULL if not found. CBUILD_PTR_ERR if error.
 */
#define cbuild_map_get(map, key)         (map)->get((map), (key))
/**
 * @brief Remove element from a map
 *
 * @param map => CBUILD_MAP* -> Map pointer
 * @param key => KEY -> Key
 * @return bool -> Error or failure on size 0
 */
#define cbuild_map_remove(map, key)      (map)->remove((map), (key))
/**
 * @brief Add or change element from a map
 *
 * @param map => CBUILD_MAP* -> Map pointer
 * @param key => KEY -> Key
 * @param val => VAL -> New value
  * @return bool -> Error or failure on size 0

 */
#define cbuild_map_set(map, key, val)    (map)->set((map), (key), (val))
/**
 * @brief Check if map contains the element
 *
 * @param map => CBUILD_MAP* -> Map pointer
 * @param key => KEY -> Key
 * @returns bool -> Is key valid
 */
#define cbuild_map_contains(map, key)    (map)->contains((map), (key))
/**
 * @brief Get list of valid map keys
 *
 * @param map => CBUILD_MAP* -> Map pointer
 * @param num => size_t* -> Return value for a size of array
 */
#define cbuild_map_keylist(map, num)     (map)->keylist((map), (num))
/**
 * @brief Resize map
 *
 * @param map => CBUILD_MAP* -> Map pointer
 * @param nbuckets => size_t -> New number of hashmap buckets
 */
#define cbuild_map_resize(map, nbuckets) (map)->resize((map), (nbuckets))
/**
 * @brief Clears map
 *
 * @param map => CBUILD_MAP -> Map pointer
 */
#define cbuild_map_clear(map)            (map)->clear((map))
/**
 * @brief Get iterator over a map
 *
 * @param map => CBUILD_MAP* -> Map pointer
 * @return CBUILD_MAP_ITER -> Map iterator struct
 */
#define cbuild_map_get_iterator(map)     (map)->get_iterator((map))
/**
 * @brief Profile map
 *
 * @param map => CBUILD_MAP* -> Map pointer
 * @return size_t -> Man number of elements in one bucket
 */
#define cbuild_map_profile(map)          (map)->profile((map))
/**
 * @brief Iterate over a map
 *
 * @param iter => CBUILD_MAP_ITER -> Iterator
 * @return CBUILD_MAP_PAIR -> Pair from a map
 */
#define cbuild_map_iter_next(iter)       (iter)->next((iter))
/**
 * @brief DBJ2 hash implementation for char*
 *
 * @param str => char* -> String
 * @return size_t -> Hash
 */
size_t cbuild_map_string_hash(const char* str);
/**
 * @brief DBJ2 hash implementation for any datatype that could be cast to
 * unsigned char*
 *
 * @param arr => void* -> Array
 * @param size => size_t -> Number of elements
 * @return size_t -> Hash
 */
size_t cbuild_map_array_hash(const void* arr, size_t size);
#endif // __CBUILD_MAP_H__
