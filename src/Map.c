#include "Map.h"
#include "Common.h"
#include "Log.h"
CBUILDDEF size_t __cbuild_map_hash_func(const void* data, size_t len) {
	const unsigned char* ucPtr = (const unsigned char*)data;
	size_t hash = 5381;
	for(size_t i = 0; i < len; i++) {
		hash = ((hash << 5) + hash) + ucPtr[i]; // hash * 33 + data[i]
	}
	return hash;
}
CBUILDDEF size_t __cbuild_map_get_hash(const cbuild_map_t* map, const void* key) {
	size_t hash = 0;
	if(map->hash_func == NULL) {
		if(map->key_size > 0) {
			hash = CBUILD_MAP_DEFAULT_HASH_FUNC(key, map->key_size)
				% map->nbuckets;
		} else {
			hash = CBUILD_MAP_DEFAULT_HASH_FUNC(*((void**)key), strlen(*((char**)key)))
				% map->nbuckets;
		}
	} else {
		hash = map->hash_func(map, key) % map->nbuckets;
	}
	return hash;
}
CBUILDDEF void* __cbuild_map_check_bucket(const cbuild_map_t* map,
																							const cbuild_map_bucket_t* bucket, const void* key) {
	if(map->keycmp_func == NULL) {
		if(map->key_size > 0) {
			for(size_t i = 0; i < bucket->nvals; i++) {
				if(memcmp(key, ((char*)bucket->vals + (i * map->elem_size)),
							map->key_size) == 0) {
					return ((char*)bucket->vals + (i * map->elem_size));
				}
			}
		} else {
			for(size_t i = 0; i < bucket->nvals; i++) {
				if(strcmp(*((char**)key),
							*(char**)(void*)(((char*)bucket->vals + (i * map->elem_size)))) == 0) {
					return ((char*)bucket->vals + (i * map->elem_size));
				}
			}
		}
	} else {
		for(size_t i = 0; i < bucket->nvals; i++) {
			if(map->keycmp_func(map, key,
											 ((char*)bucket->vals + (i * map->elem_size)))) {
				return ((char*)bucket->vals + (i * map->elem_size));
			}
		}
	}
	return NULL;
}
CBUILDDEF void cbuild_map_init(cbuild_map_t* map, size_t nbuckets) {
	map->nbuckets = nbuckets;
	map->buckets = __CBUILD_MALLOC(map->nbuckets * sizeof(cbuild_map_bucket_t));
	cbuild_assert(map->buckets != NULL, "Allocation failed.\n");
	memset(map->buckets, 0, nbuckets * sizeof(cbuild_map_bucket_t));
}
CBUILDDEF void* cbuild_map_get_raw(const cbuild_map_t* map, const void* key) {
	if(map->nbuckets == 0) {
		cbuild_log_error("Trying to call \"cbuild_map_get\" on an empty map!");
		return CBUILD_PTR_ERR;
	}
	size_t hash = __cbuild_map_get_hash(map, key);
	cbuild_map_bucket_t* bucket = &map->buckets[hash];
	return __cbuild_map_check_bucket(map, bucket, key);
}
CBUILDDEF void* cbuild_map_get_or_alloc_raw(cbuild_map_t* map, const void* key) {
	if(map->nbuckets == 0) {
		cbuild_log_error("Trying to call \"cbuild_map_get_or_alloc\" with an empty map!");
		return NULL;
	}
	size_t hash = __cbuild_map_get_hash(map, key);
	cbuild_map_bucket_t* bucket = &map->buckets[hash];
	void* ret = __cbuild_map_check_bucket(map, bucket, key);
	if(ret != NULL) return ret;
	bucket->nvals++;
	bucket->vals = __CBUILD_REALLOC((char*)bucket->vals,
															 bucket->nvals * map->elem_size);
	cbuild_assert(bucket->vals != NULL, "Allocation failed.\n");
	memset((char*)bucket->vals + (bucket->nvals - 1) * map->elem_size, 0,
				map->elem_size);
	return (char*)bucket->vals + ((bucket->nvals - 1) * map->elem_size);
}
CBUILDDEF bool cbuild_map_remove_raw(cbuild_map_t* map, const void* key) {
	if(map->nbuckets == 0) {
		cbuild_log_error("Trying to call \"cbuild_map_remove_raw\" with an empty map!");
		return false;
	}
	size_t hash = __cbuild_map_get_hash(map, key);
	cbuild_map_bucket_t* bucket = &map->buckets[hash];
	void* elem = __cbuild_map_check_bucket(map, bucket, key);
	if(elem == NULL) return false;
	if(map->clear_func) map->clear_func(map, elem);
	if(bucket->nvals == 1) {
		__CBUILD_FREE(bucket->vals);
		bucket->vals = NULL;
		bucket->nvals = 0;
	} else {
		char* last = ((char*)bucket->vals) +
			((bucket->nvals - 1) * map->elem_size);
		memcpy(elem, last, map->elem_size);
		bucket->nvals--;
	}
	return true;
}
CBUILDDEF void cbuild_map_clear(cbuild_map_t* map) {
	if(map->clear_func == NULL) {
		for(size_t i = 0; i < map->nbuckets; i++) {
			cbuild_map_bucket_t* bucket = &map->buckets[i];
			__CBUILD_FREE(bucket->vals);
			bucket->vals = NULL;
			bucket->nvals = 0;
		}
		__CBUILD_FREE(map->buckets);
		map->buckets = NULL;
		map->nbuckets = 0;
	} else {
		for(size_t i = 0; i < map->nbuckets; i++) {
			cbuild_map_bucket_t* bucket = &map->buckets[i];
			for(size_t j = 0; j < bucket->nvals; j++) {
				map->clear_func(map, bucket->vals + (j * map->elem_size));
			}
			__CBUILD_FREE(bucket->vals);
			bucket->vals = NULL;
			bucket->nvals = 0;
		}
		__CBUILD_FREE(map->buckets);
		map->buckets = NULL;
		map->nbuckets = 0;
	}
}
CBUILDDEF void cbuild_map_iter_reset(cbuild_map_t* map) {
	map->iter_buckets = 0;
	map->iter_vals = 0;
}
CBUILDDEF void* cbuild_map_iter_next(cbuild_map_t* map) {
	while(map->iter_buckets < map->nbuckets) {
		cbuild_map_bucket_t* bucket = &map->buckets[map->iter_buckets];
		while(map->iter_vals < bucket->nvals) {
			return bucket->vals + (map->iter_vals++ * map->elem_size);
		}
		map->iter_buckets++;
		map->iter_vals = 0;
	}
	return NULL;
}
