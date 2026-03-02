#pragma once // For LSP 
//! Global arena for CBuild. Uses bump allocator under the hood.

#include "Common.h"

/// Same as `malloc`. Can return NULL.
CBUILDDEF void* cbuild_temp_malloc(size_t size);
/// Same as `realloc`. Does not free old memory. Can return NULL.
CBUILDDEF void* cbuild_temp_realloc(void* ptr, size_t size);
/// `sprintf` that allocates using `cbuild_temp_malloc`.
CBUILD_ATTR_PRINTF(1, 
	CBUILDDEF char* cbuild_temp_sprintf(const char* fmt, ...));
/// `vsprintf` that allocates using `cbuild_temp_malloc`.
CBUILDDEF char* cbuild_temp_vsprintf(const char* fmt, va_list ap);
/// `strdup` that allocates using `cbuild_temp_malloc`.
CBUILDDEF char* cbuild_temp_strdup(const char* str);
/// Same as `strdup` but copies raw memory. Allocates using `cbuild_temp_malloc`.
CBUILDDEF void* cbuild_temp_memdup(const void* mem, size_t size);
/// Reset temporary allocator back to [p:checkpoint]. 
CBUILDDEF void cbuild_temp_reset(size_t checkpoint);
/// Create new allocator checkpoint.
CBUILDDEF size_t cbuild_temp_checkpoint(void);
