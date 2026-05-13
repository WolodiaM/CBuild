#pragma once // For LSP 
//! Allocator abstraction.
//!
//! This is based on allocator object, which combines three standard C
//! allocation function (`malloc`, `realloc`, and `free`) and state pointer.
//! Api for these functions are same as in libc. Because allocator functions
//! take one additional argument you can not just put libc functions there,
//! but very thin wrappers are enough to make allocator object based on libc
//! allocator.
//!
//! ::: note
//! `free` function can be implemented as no-op for allocator that does not
//! support free of random pointers. Realloc also could also leave old pointer
//! still allocated in such case.
//! :::
//!
//! You can attach state to allocator using `state`. No functions will ever
//! modify it, so it could be used to pass additional parameters to
//! allocator's functions.
//!
//! License: `GPL-3.0-or-later`.

#include "Common.h"
#include "Arena.h"

/// Allocator object. Can be used to write allocator-agnostic functions
/// without reinventing the wheel.
typedef struct cbuild_allocator_t {
	void* (*malloc)(struct cbuild_allocator_t* self, size_t size);
	void* (*realloc)(struct cbuild_allocator_t* self, void* ptr, size_t size);
	void (*free)(struct cbuild_allocator_t* self, void* ptr);
	void* state;
} cbuild_allocator_t;
/// Create allocator from arena.
CBUILDDEF cbuild_allocator_t cbuild_allocator_from_arena(cbuild_arena_t* arena);
/// Create allocator from libc allocator.
CBUILDDEF cbuild_allocator_t cbuild_allocator_from_libc(void);
