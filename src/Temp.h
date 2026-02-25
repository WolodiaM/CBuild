#pragma once // For LSP 
//! Global arena for CBuild. Uses bump allocator under the hood.

#include "Common.h"

/// Same as `malloc`. Can return NULL.
CBUILDDEF void* cbuild_temp_alloc(size_t size);
/// `sprintf` that allocates using `cbuild_temp_alloc`.
CBUILDDEF char* cbuild_temp_sprintf(const char* fmt, ...)
__attribute__((format(printf, 1, 2)));
/// `vsprintf` that allocates using `cbuild_temp_alloc`.
CBUILDDEF char* cbuild_temp_vsprintf(const char* fmt, va_list ap);
/// `strdup` that allocates using `cbuild_temp_alloc`.
CBUILDDEF char* cbuild_temp_strdup(const char* str);
/// Same as `strdup` but copies raw memory. Allocates using `cbuild_temp_alloc`.
CBUILDDEF void* cbuild_temp_memdup(const void* mem, size_t size);
/// Reset temporary allocator back to [p:checkpoint]. 
CBUILDDEF void cbuild_temp_reset(size_t checkpoint);
/// Create new allocator checkpoint.
CBUILDDEF size_t cbuild_temp_checkpoint(void);
