#pragma once // For LSP 
//! Linked list. For merge with CBuild
//!
//! License: GPL-3.0-or-later
//!
//! This linked list is based on *shadow data* technique and never returns
//! pointers to real linked list structures. It implements full doubly-linked
//! list and this is why it does not need *head* node (but can assign first
//! element as a head, or any other element, this implementation will happily
//! consume pointer to any element and find start/end of linked list).
//!
//! In reality, layout in memory will look something like this:
//! `|void* prev|void* next|user-provided data|`
//! All functions operate on pointer to this `user-provided data` block. All
//! internal information is hidden from user. On the other hand library knows
//! how to access this hidden data and implement linked list on top of it. This
//! allows for a lot cleaner API, were user directly have data it wants.

#include "Common.h"
#include "Allocator.h"

/// Allocate new element in linked list. New memory would be zero-initialized.
///
/// This function will first iterate forward until it find end and then append
/// new element.
///
/// If [f:elem] is null then it will allocate new linked list.
///
/// * [pl:a] Allocator.
/// * [pl:elem:T*] Any element of linked list.
/// * [pl:size] Length of a data section of element in bytes.
///
/// [r:T*] Pointer to new element.
CBUILDDEF void* cbuild_ll_new(cbuild_allocator_t* a, void* elem, size_t size);
/// Append element to linked list. This is semi-internal function.
///
/// This function will first iterate forward until it find end and then append
/// new element.
///
/// If [f:elem] is null then it will allocate new linked list.
/// If [f:elem] points to null then it will allocate new linked list. Element
/// will be also returned in [f:elem] then.
///
/// * [pl:a] Allocator.
/// * [pl:elem:T*] Any element of linked list. Pointer to pointer to data of this element.
/// * [pl:data:T] Data that need to be put into new node. It will be copied via `memcpy`.
/// * [pl:size] Length of a data in bytes.
///
/// [r:T*] Pointer to new element.
CBUILDDEF void* cbuild_ll_append_raw(cbuild_allocator_t* a, void* elem, void* data, size_t size);
/// Append element to linked list.
///
/// This function will first iterate forward until it find end and then append
/// new element.
///
/// If [f:elem:T*] is null then it will allocate new linked list.
/// If [f:elem:T*] points to null then it will allocate new linked list. Element
/// will be also returned in [f:elem:T*] then.
///
/// * [pl:a:cbuild_allocator_t*] Allocator.
/// * [pl:elem:T*] Any element of linked list. Pointer to pointer to data of this element.
/// * [pl:data:T] Data that need to be put into new node. Must be value and not pointer.
///
/// [r:T*] Pointer to new element.
#define cbuild_ll_append(a, elem, data)                                       \
	({                                                                          \
		__auto_type __cbuild_ll_tmp = data;                                       \
		cbuild_ll_append_raw(a, elem, &__cbuild_ll_tmp, sizeof(__cbuild_ll_tmp)); \
	})
/// Append element to linked list.
///
/// This function will first iterate forward until it find end and then append
/// new element.
///
/// If [f:elem:T*] is null then it will allocate new linked list.
/// If [f:elem:T*] points to null then it will allocate new linked list. Element
/// will be also returned in [f:elem:T*] then.
///
/// * [pl:a:cbuild_allocator_t*] Allocator.
/// * [pl:elem:T*] Any element of linked list. Pointer to pointer to data of this element.
/// * [pl:data:T] Data that need to be put into new node. Must be typed (not `void`) pointer.
///
/// [r:T*] Pointer to new element.
#define cbuild_ll_append_ptr(a, elem, data)                                   \
	({                                                                          \
		__auto_type __cbuild_ll_tmp = data;                                       \
		cbuild_ll_append_raw(a, elem, __cbuild_ll_tmp, sizeof(*__cbuild_ll_tmp)); \
	})
/// Get size of linked list in elements.
///
/// This will first iterate backward until it find start and then iterate
/// forward until end is found and count number of elements.
///
/// * [pl:elem:T*] Any element of linked list.
CBUILDDEF size_t cbuild_ll_size(void* elem);
/// Remove element from linked list.
///
/// * [pl:a] Allocator.
/// * [pl:elem:T*] Element of linked list that should be removed.
CBUILDDEF void cbuild_ll_remove(cbuild_allocator_t* a, void* elem);
/// Free full linked list.
///
/// * [pl:a] Allocator.
/// * [pl:elem:T*] Any element of linked list.
CBUILDDEF void cbuild_ll_free(cbuild_allocator_t* a, void* elem);
/// Get next element from linked list or NULL.
CBUILDDEF void* cbuild_ll_next(void* elem);
/// Get previous element from linked list or NULL.
CBUILDDEF void* cbuild_ll_prev(void* elem);
/// Get head element from linked list.
CBUILDDEF void* cbuild_ll_head(void* elem);
/// Get tail element from linked list.
CBUILDDEF void* cbuild_ll_tail(void* elem);
