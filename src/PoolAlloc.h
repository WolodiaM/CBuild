#pragma once // For LSP 
//! Pool allocator
//!
//! This allocator implements object pool allocator, but can be used as a block
//! allocator (allocator which is used to allocate blocks of constant size) if
//! you pass no-op callbacks.
//!
//! # Allocator design
//
//! Allocator uses linked list of *chunks*, each holding some number of objects.
//! Additionally it uses linked list of metadata bytes which store bitmap of
//! allocated objects. 
//!
//! # Allocation strategy
//!
//! Allocator tracks allocation pointer and searches for a next free chunk after
//! this pointer. This should speed-up allocations in case of some number of
//! Statically-allocated blocks at the start of allocator (allocated during the
//! initialization of application). It also should be faster if there are some
//! bigger spans of allocated objects. Also it does allocations over bigger span
//! of blocks, so objects are reused later, giving them time to be freed by
//! other code.
//!
//! License: `GPL-3.0-or-later`.
#include "Common.h"
/// Object initialization function for pool allocator.
typedef void (*cbuild_pool_alloc_object_init_t)(void* obj);
/// Object reset function for pool allocator.
///
/// ::: note
/// This should not free object, just reset its state to "just allocated" state.
/// :::
typedef void (*cbuild_pool_alloc_object_reset_t)(void* obj);
/// Pool allocator structure.
///
/// * [fl:object_size] Size of object in bytes.
/// * [fl:object_init] Object initialization function.
/// * [fl:object_reset] Object reset function.
/// * [fl:base] Base of linked list of chunks used by allocator.
/// * [fl:ptr] Allocation ptr (to speed-up allocating new object). Points to last allocated index.
/// * [fl:allocated] Base of linked list of allocator metadata chunks.
/// * [fl:num_objects_per_chunk] Number of objects per chunk.
/// * [fl:num_chunks] Number of already allocated chunks
typedef struct cbuild_pool_alloc_t {
	// Allocator configuration
	size_t object_size;
	cbuild_pool_alloc_object_init_t object_init;
	cbuild_pool_alloc_object_reset_t object_reset;
	// Allocator state. Linked list.
	void* base;
	size_t ptr;
	// Metadata store. Linked list
	uint8_t* allocated;
	// Capacity (per-chunk and number of chunk)
	size_t num_objects_per_chunk;
	size_t num_chunks;
} cbuild_pool_alloc_t;
/// Create new pool allocator. This will pre-allocate one chunks.
CBUILDDEF cbuild_pool_alloc_t cbuild_pool_alloc_new(
	size_t object_size, size_t num_objects_per_chunk,
	cbuild_pool_alloc_object_init_t init, cbuild_pool_alloc_object_reset_t reset);
/// Allocate new object from pool. This will either use one of free objects in
/// already allocated chunks or allocate a new chunk and then allocate.
CBUILDDEF void* cbuild_pool_alloc_malloc(cbuild_pool_alloc_t* a);
/// Free object. [f:ptr] must point to object allocated by
/// [cbuid_pool_alloc_malloc](DOC:cbuild_pool_alloc_malloc). This will reset
/// object with provided callback.
CBUILDDEF void cbuild_pool_alloc_free(cbuild_pool_alloc_t* a, void* ptr);
