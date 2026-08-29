//! License: GPL-3.0-or-later
#include "PoolAlloc.h"
#include "LL.h"
#ifdef CBUILD_TRACE_PA
	#define pool_alloc_trace(...) cbuild_log_trace(__VA_ARGS__)
#else
	#define pool_alloc_trace(...)
#endif // TRACE_PA
void cbuild_pool_alloc_add_chunk(cbuild_pool_alloc_t* a) {
	size_t chunk_in_bytes = a->object_size * a->num_objects_per_chunk;
	cbuild_allocator_t alloc = cbuild_allocator_from_libc();
	// If 'a->base == NULL' new linked list would be allocated
	// If 'a->base == <ptr>' then new element would be appended after tail.
	void* ab = cbuild_ll_new(&alloc, a->base, chunk_in_bytes);
	if (!a->base) a->base = ab;
	size_t chunk_meta_in_bytes = (a->num_objects_per_chunk + 7) / 8;
	for (size_t i = 0; i < a->num_objects_per_chunk; i++) {
		a->object_init(ab + i * a->object_size);
	}
	// If 'a->base == NULL' new linked list would be allocated
	// If 'a->base == <ptr>' then new element would be appended after tail.
	void* am = cbuild_ll_new(&alloc, a->allocated, chunk_meta_in_bytes);
	if (!a->allocated) a->allocated = am;
	pool_alloc_trace(
		"PA[%p]: allocating new chunk at alloc_base=%p, meta_base=%p with index %zu.",
		a, ab, am, a->num_chunks);
	a->num_chunks++;
}
cbuild_pool_alloc_t cbuild_pool_alloc_new(size_t object_size, size_t num_objects_per_chunk,
	cbuild_pool_alloc_object_init_t object_init, cbuild_pool_alloc_object_reset_t object_reset) {
	cbuild_pool_alloc_t ret = {0};
	ret.object_size = object_size;
	ret.num_objects_per_chunk = num_objects_per_chunk;
	ret.object_init = object_init;
	ret.object_reset = object_reset;
	cbuild_pool_alloc_add_chunk(&ret);
	ret.ptr = ret.num_objects_per_chunk;
	pool_alloc_trace(
		"PA: creating new allocator with object_size=%zu, num_objects_per_chunk=%zu",
		ret.object_size, ret.num_objects_per_chunk);
	return ret;
}
bool cbuild_pool_alloc_is_free(cbuild_pool_alloc_t* a, size_t idx) {
	size_t meta_chunk = idx / a->num_objects_per_chunk;
	idx %= a->num_objects_per_chunk;
	size_t meta_byte = idx >> 3; // 2**3 == 8
	size_t meta_bit = idx & 0x7; // 0b111 == 0x7
	pool_alloc_trace(
		"PA[%p]: reading meta in chunk %zu for object %zu in byte %zu in bit %zu.",
		a, meta_chunk, idx, meta_byte, meta_bit);
	return (((uint8_t*)cbuild_ll_get(a->allocated, meta_chunk))[meta_byte] & (1 << meta_bit)) == 0;
}
void cbuild_pool_alloc_set_meta(cbuild_pool_alloc_t* a, size_t idx, bool val) {
	size_t meta_chunk = idx / a->num_objects_per_chunk;
	idx %= a->num_objects_per_chunk;
	size_t meta_byte = idx >> 3; // 2**3 == 8
	size_t meta_bit = idx & 0x7; // 0b111 == 0x7
	uint8_t mask = (uint8_t)(1 << meta_bit);
	if (val) {
		((uint8_t*)cbuild_ll_get(a->allocated, meta_chunk))[meta_byte] |= mask;
	} else {
		((uint8_t*)cbuild_ll_get(a->allocated, meta_chunk))[meta_byte] &= ~mask;
	}
	pool_alloc_trace(
		"PA[%p]: setting meta in chunk %zu for object %zu in byte %zu in bit %zu to value %s.",
		a, meta_chunk, idx, meta_byte, meta_bit, val ? "true" : "false");
}
void* cbuild_pool_alloc_malloc(cbuild_pool_alloc_t* a) {
	pool_alloc_trace("PA[%p]: requesting new object.", a);
	size_t old_ptr = a->ptr;
	do {
		a->ptr++;
		if (a->ptr >= a->num_chunks * a->num_objects_per_chunk) a->ptr = 0;
		if (old_ptr == a->ptr) {
			cbuild_pool_alloc_add_chunk(a);
			// And now it loops until it finds new chunk and finally can allocate.
		}
	} while (!cbuild_pool_alloc_is_free(a, a->ptr));
	cbuild_pool_alloc_set_meta(a, a->ptr, true);
	size_t chunk = a->ptr / a->num_objects_per_chunk;
	size_t block = a->ptr % a->num_objects_per_chunk;
	pool_alloc_trace("PA[%p]: allocating new object at index %zu.", a, a->ptr);
	return cbuild_ll_get(a->base, chunk) + block * a->object_size;
}
size_t cbuild_pool_alloc_ptr_to_idx(cbuild_pool_alloc_t* a, void* ptr) {
	void* rb = a->base;
	size_t idx_prefix = 0;
	while (rb != NULL) {
		size_t idx = (size_t)(ptr - rb) / a->object_size;
		if (idx < a->num_objects_per_chunk) return idx_prefix + idx;
		rb = cbuild_ll_next(rb);
		idx_prefix += a->num_objects_per_chunk;
	}
	return a->num_chunks * a->num_objects_per_chunk;
}
void cbuild_pool_alloc_free(cbuild_pool_alloc_t* a, void* ptr) {
	pool_alloc_trace("PA[%p]: freeing object %p.", a, ptr);
	a->object_reset(ptr);
	size_t idx = cbuild_pool_alloc_ptr_to_idx(a, ptr);
	pool_alloc_trace("PA[%p]: freeing object at index %zu.", a, idx);
	cbuild_pool_alloc_set_meta(a, idx, false);
}
