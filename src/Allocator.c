//! License: `GPL-3.0-or-later`.

#include "Allocator.h"
#include "Arena.h"

void* __cbuild_allocator_arena_malloc(cbuild_allocator_t* self, size_t size) {
	return cbuild_arena_malloc(self->state, size);
}
void* __cbuild_allocator_arena_realloc(cbuild_allocator_t* self, void* ptr, size_t size) {
	return cbuild_arena_realloc(self->state, ptr, size);
}
void __cbuild_allocator_arena_free(cbuild_allocator_t* self, void* ptr) {
	CBUILD_UNUSED(self);
	CBUILD_UNUSED(ptr);
}
void* __cbuild_allocator_libc_malloc(cbuild_allocator_t* self, size_t size) {
	CBUILD_UNUSED(self);
	return malloc(size);
}
void* __cbuild_allocator_libc_realloc(cbuild_allocator_t* self, void* ptr, size_t size) {
	CBUILD_UNUSED(self);
	return realloc(ptr, size);
}
void __cbuild_allocator_libc_free(cbuild_allocator_t* self, void* ptr) {
	CBUILD_UNUSED(self);
	free(ptr);
}
cbuild_allocator_t cbuild_allocator_from_arena(cbuild_arena_t* arena) {
	return (cbuild_allocator_t){
		.malloc = __cbuild_allocator_arena_malloc,
		.realloc = __cbuild_allocator_arena_realloc,
		.free = __cbuild_allocator_arena_free,
		.state = arena,
	};
}
cbuild_allocator_t cbuild_allocator_from_libc(void) {
	return (cbuild_allocator_t){
		.malloc = __cbuild_allocator_libc_malloc,
		.realloc = __cbuild_allocator_libc_realloc,
		.free = __cbuild_allocator_libc_free,
	};
}
