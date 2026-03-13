#include "Arena.h"
#include "Common.h"
#include "Log.h"

CBUILDDEF void cbuild_arena_base_realloc(cbuild_arena_t* arena, size_t new_capacity) {
	arena->base = __CBUILD_REALLOC(arena->base, new_capacity);
	cbuild_assert(arena->base != NULL, "Allocation failed.\n");
	arena->capacity = new_capacity;
}
CBUILDDEF void cbuild_arena_base_free(cbuild_arena_t* arena) {
	arena->pointer = 0;
	arena->capacity = 0;
	__CBUILD_FREE(arena->base);
}
CBUILDDEF void* cbuild_arena_malloc(cbuild_arena_t* arena, size_t size) {
	size_t adj_size = (size | ((2*sizeof(void*)) - 1)) + 1;
	if ((arena->pointer + adj_size) >= arena->capacity) return NULL;
	void* ptr = (void*)(arena->base + arena->pointer);
	arena->pointer += adj_size;
	#ifdef CBUILD_PROFILER
		arena->max_pointer = CBUILD_MAX(arena->pointer, arena->max_pointer);
	#endif // CBUILD_PROFILER
	return ptr;
}
CBUILDDEF void* cbuild_arena_realloc(cbuild_arena_t* arena, void* ptr, size_t size) {
	void* new_ptr = cbuild_arena_malloc(arena, size);
	if (!new_ptr) return NULL;
	// NOTE: If allocation not failed it means that 'size' bytes can be safely copied.
	// This can expose some data, so this is not a best idea. But proper size tracking
	// need metadata this allocator does not support.
	memcpy(new_ptr, ptr, size);
	return new_ptr;
}
CBUILDDEF size_t cbuild_arena_checkpoint(cbuild_arena_t* arena) {
	return arena->pointer;
}
CBUILDDEF void cbuild_arena_reset(cbuild_arena_t* arena, size_t checkpoint) {
	arena->pointer = checkpoint;
}
CBUILDDEF char* cbuild_arena_strdup(cbuild_arena_t* arena, const char* src) {
	size_t len = strlen(src);
	char* dst = cbuild_arena_malloc(arena, len + 1);
	if (!dst) return NULL;
	memcpy(dst, src, len);
	dst[len] = 0;
	return dst;
}
CBUILDDEF void* cbuild_arena_memdup(cbuild_arena_t* arena, const void* src, size_t n) {
	void* dst = cbuild_arena_malloc(arena, n);
	if (!dst) return NULL;
	memcpy(dst, src, n);
	return dst;
}
CBUILDDEF char* cbuild_arena_sprintf(cbuild_arena_t* arena, const char* fmt, ...) {
	va_list va;
	va_start(va, fmt);
	char* ret = cbuild_arena_vsprintf(arena, fmt, va);
	va_end(va);
	return ret;
}
CBUILDDEF char* cbuild_arena_vsprintf(cbuild_arena_t* arena, const char* fmt,
	va_list ap) {
	va_list va;
	va_copy(va, ap);
	int n = vsnprintf(NULL, 0, fmt, va);
	va_end(va);
	if(n >= 0) {
		char* ret =	cbuild_arena_malloc(arena, (size_t)n + 1);
		if (!ret) return NULL;
		vsnprintf(ret, (size_t)n + 1, fmt, ap);
		return ret;
	} else {
		return NULL;
	}
}
#ifdef CBUILD_PROFILER
	CBUILDDEF void cbuild_arena_profiler(cbuild_arena_t* arena, const char* arena_id) {
		cbuild_log(CBUILD_LOG_TRACE, "Used %zu/%zu bytes of arena %s.",
			arena->max_pointer, arena->capacity, arena_id);
	}
#endif // CBUILD_PROFILER
