//! License: `GPL-3.0-or-later`.

#include "Map.h"
#include "Common.h"
#include "StringView.h"
#include "StringBuilder.h"
CBUILDDEF size_t __cbuild_map_num_hash(const void* map,
	const void* key, size_t klen) {
	CBUILD_UNUSED(map);
	return CBUILD_MAP_DEFAULT_HASH(key, klen);
}
CBUILDDEF size_t __cbuild_map_cstr_hash(const void* map,
	const void* key, size_t klen) {
	CBUILD_UNUSED(map);
	CBUILD_UNUSED(klen);
	size_t len = strlen(key);
	return CBUILD_MAP_DEFAULT_HASH(key, len);
}
CBUILDDEF size_t __cbuild_map_sv_hash(const void* map,
	const void* key, size_t klen) {
	CBUILD_UNUSED(map);
	CBUILD_UNUSED(klen);
	const cbuild_sv_t* sv = key;
	return CBUILD_MAP_DEFAULT_HASH(sv->data, sv->size);
}
CBUILDDEF size_t __cbuild_map_sb_hash(const void* map,
	const void* key, size_t klen) {
	CBUILD_UNUSED(map);
	CBUILD_UNUSED(klen);
	const cbuild_sb_t* sb = key;
	return CBUILD_MAP_DEFAULT_HASH(sb->data, sb->size);
}
CBUILDDEF bool __cbuild_map_num_keycmp(const void* map,
	const void* k1, const void* k2, size_t klen) {
	CBUILD_UNUSED(map);
	return memcmp(k1, k2, klen) == 0;
}
CBUILDDEF bool __cbuild_map_cstr_keycmp(const void* map,
	const void* k1, const void* k2, size_t klen) {
	CBUILD_UNUSED(map);
	CBUILD_UNUSED(klen);
	return strcmp(k1, k2) == 0;
}
CBUILDDEF bool __cbuild_map_sv_keycmp(const void* map,
	const void* k1, const void* k2, size_t klen) {
	CBUILD_UNUSED(map);
	CBUILD_UNUSED(klen);
	const cbuild_sv_t* sv1 = k1;
	const cbuild_sv_t* sv2 = k2;
	return cbuild_sv_cmp(*sv1, *sv2) == 0;
}
CBUILDDEF bool __cbuild_map_sb_keycmp(const void* map,
	const void* k1, const void* k2, size_t klen) {
	CBUILD_UNUSED(map);
	CBUILD_UNUSED(klen);
	const cbuild_sb_t* sv1 = k1;
	const cbuild_sb_t* sv2 = k2;
	return cbuild_sb_cmp(*sv1, *sv2) == 0;
}
CBUILDDEF size_t __cbuild_map_step(size_t prev) {
	// Splitmix64 with added  '| 1'.
	prev += 0x9e3779b97f4a7c15;
	uint64_t z = prev;
	z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
	z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
	// We probably have less than 4GB of array and we need to have this number odd
	return (z ^ (z >> 31)) | 1;
}
CBUILDDEF size_t cbuild_map_hash_djb2(const void* data, size_t len) {
	const uint8_t* ucPtr = data;
	size_t hash = 5381;
	for (size_t i = 0; i < len; i++) {
		hash = ((hash << 5) + hash) + ucPtr[i]; // hash * 33 + data[i]
	}
	return hash;
}
CBUILDDEF size_t cbuild_map_hash_fnv1(void const *data, size_t len) {
	const uint8_t* bytes = data;
	size_t hash = 14695981039346656037u;
	for (size_t i = 0; i < len; ++i) {
		hash *= 1099511628211u;
		hash ^= (size_t)bytes[i];
	}
	return hash;
}
CBUILDDEF size_t cbuild_map_hash_fnv1a(const void* data, size_t len) {
	const uint8_t* bytes = data;
	size_t hash = 14695981039346656037u;
	for (size_t i = 0; i < len; i++) {
		hash ^= (uint64_t)bytes[i];
		hash *= 1099511628211u;
	}
	return hash;
}
CBUILDDEF size_t cbuild_map_hash_sdbm(void const *data, size_t len) {
	const uint8_t* bytes = data;
	size_t hash = 0u;
	for (size_t i = 0; i < len; i++) {
		hash = bytes[i] + (hash << 6) + (hash << 16) - hash;
	}
	return hash;
}
