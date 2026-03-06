#pragma once // For LSP 
//! Arena allocator.

#include "Common.h"

/// Arena allocator state.
///
/// ::: note
/// Arena automatically pad things to `2 * sizeof(void*)`{.c}. This is done to
/// be a little more cache-friendly. Padding is done after allocation so it is
/// always placed after the data.
/// :::
///
/// * [fl:base] Base pointer for allocated memory.
/// * [fl:capacity] Capacity of this arena.
/// * [fl:pointer] Pointer to first unallocated byte.
typedef struct cbuild_arena_t {
	void* base;
	size_t capacity;
	size_t pointer;
} cbuild_arena_t;

/// Reallocate arena base to new size.
///
/// If old base is `NULL` then new memory will be allocated.
///
/// ::: note
/// This is just a direct call to `realloc` plus some metadata management.
/// :::
CBUILDDEF void cbuild_arena_base_realloc(cbuild_arena_t* arena, size_t new_capacity);
/// Allocate new arena base.
///
/// * [pl:arena:cbuild_arena_t*] Arena object.
/// * [pl:capacity:size_t] Capacity of arena.
#define cbuild_arena_base_alloc(arena, capacity) cbuild_arena_base_realloc(arena, capacity)
/// Free arena base.
CBUILDDEF void cbuild_arena_base_free(cbuild_arena_t* arena);
/// Allocate inside of arena.
///
/// * [pl:arena] Arena object.
/// * [pl:size] Number of bytes to allocate.
///
/// [r:] Pointer into arena. NULL if out-of-memory.
CBUILDDEF void* cbuild_arena_malloc(cbuild_arena_t* arena, size_t size);
/// `realloc` for arena. Will not free old memory.
///
/// It [p:size] is bigger than old size then memory from adjacent allocations
/// will be used to initialize new memory block. This is safe access (no
/// segmentation fault possible), but can expose unrelated memory trough
/// new allocation.
CBUILDDEF void* cbuild_arena_realloc(cbuild_arena_t* arena, void* ptr, size_t size);
/// Save checkpoint into arena.
CBUILDDEF size_t cbuild_arena_checkpoint(cbuild_arena_t* arena);
/// Reset arena to some checkpoint.
CBUILDDEF void cbuild_arena_reset(cbuild_arena_t* arena, size_t checkpoint);
/// Free arena. Set `pointer` to 0.
///
/// * [pl:arena:cbuild_arena_t*] Arena object.
#define cbuild_arena_free(arena) cbuild_arena_reset(arena, 0)
/// `strdup` that uses arena as allocator.
CBUILDDEF char* cbuild_arena_strdup(cbuild_arena_t* arena, const char* src);
/// `memdup` that uses arena as its allocator.
CBUILDDEF void* cbuild_arena_memdup(cbuild_arena_t* arena, const void* src, size_t n);
/// `sprintf` that uses arena as its allocator.
CBUILDDEF char* cbuild_arena_sprintf(cbuild_arena_t* arena, const char* fmt, ...);
/// `vsprintf` that uses arena as its allocator.
CBUILDDEF char* cbuild_arena_vsprintf(cbuild_arena_t* arena, const char* fmt,
	va_list args);
/// Print formatter arena usage.
CBUILDDEF void cbuild_arena_profile(cbuild_arena_t* arena);
