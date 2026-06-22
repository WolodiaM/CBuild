#pragma once // For LSP 
//! Generic API for any spans. It works with any structures containing specific
//! fields - `data` and `size` (example below). Currently two CBuild datatypes
//! follow this convention - dynamic arrays and string views.
//!
//! ```c
//! typedef struct cbuild_span_t
//!     T* data;
//!     size_t size;
//!     ...
//! }
//! ```
//!
//! License: `GPL-3.0-or-later`.

/// Set element in span using its index.
///
/// * [pl:span:const cbuild_span_t*] Span.
/// * [pl:idx:size_t] Element index.
/// * [pl:elem:*typeof(span->data)] New element's value.
/// 
/// [r:bool] False on overflow.
#define cbuild_span_set(span, idx, elem)                                  \
	({                                                                      \
		bool __cbuild__ret = false;                                           \
		if ((idx) >= (span)->size) {                                          \
			cbuild_log(CBUILD_LOG_ERROR, "Index overflow in cbuild_span_set."); \
			__cbuild__ret = false;                                              \
		} else {                                                              \
			(span)->data[(idx)] = elem;                                         \
			__cbuild__ret = true;                                               \
		}                                                                     \
		__cbuild__ret;                                                        \
	})
/// Get element from span using its index.
///
/// * [pl:span:const cbuild_span_t*] Span.
/// * [pl:idx:size_t] Element index.
/// 
/// [r:typeof(span->data)] Pointer to an element or `NULL`{.c} if index out of bounds.
#define cbuild_span_get(span, idx)                                        \
	({                                                                      \
		typeof(*(span)->data)* __cbuild__ret = NULL;                          \
		if ((idx) >= (span)->size) {                                          \
			cbuild_log(CBUILD_LOG_ERROR, "Index overflow in cbuild_span_get."); \
			__cbuild__ret = NULL;                                               \
		} else {                                                              \
			__cbuild__ret = &((span)->data[(idx)]);                             \
		}                                                                     \
		__cbuild__ret;                                                        \
	})
/// Get first element from span.
///
/// * [pl:span:const cbuild_span_t*] Span.
/// 
/// [r:typeof(span->data)] Pointer to an element.
#define cbuild_span_first(span)                                         \
	({                                                                    \
		typeof(*(span)->data)* __cbuild__ret = NULL;                        \
		if ((span)->size == 0) {                                            \
			cbuild_log_error("Trying to get first element from empty span."); \
			__cbuild__ret = NULL;                                             \
		} else {                                                            \
			__cbuild__ret = &((span)->data[0]);                               \
		}                                                                   \
		__cbuild__ret;                                                      \
	})
/// Get last element from span.
///
/// * [pl:span:const cbuild_span_t*] Span.
/// 
/// [r:typeof(span->data)] Pointer to an element.
#define cbuild_span_last(span)                                         \
	({                                                                   \
		typeof(*(span)->data)* __cbuild__ret = NULL;                       \
		if ((span)->size == 0) {                                           \
			cbuild_log_error("Trying to get last element from empty span."); \
			__cbuild__ret = NULL;                                            \
		} else {                                                           \
			__cbuild__ret = &((span)->data[(span)->size - 1]);               \
		}                                                                  \
		__cbuild__ret;                                                     \
	})
/// Foreach implementation for spans. Same semantics as 'for' loop.
///
/// * [pl:span:cbuild_span_t*] Span.
/// * [pl:iter:name] Name if variable that will be used as iterator. Will have type `typeof(span->data)`{.c}.
#define cbuild_span_foreach(span, iter)              \
	for (typeof(*((span)->data))* iter = (span)->data; \
		iter < ((span)->data + (span)->size); iter++)

#define cbuild_last(span)
