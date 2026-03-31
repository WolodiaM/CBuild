#pragma once // For LSP
//! Abstract runtime loading of dynamic libraries.
//!
//! License: `GPL-3.0-or-later`.

#include "Common.h"

/// Load dynamic library into handle.
///
/// * [pl:filename] Full path to a library or its platform-dependant "ID".
/// * [pl:eager] By default symbols are loaded lazily (during calls to [`cbuild_dlib_get_sym`](#ID_cbuild_dlib_get_sym). This makes them load eagerly. Can be user to run global constructors.
///
/// [r:] `NULL` if error happened.
CBUILDDEF cbuild_dlib_t cbuild_dlib_open(const char* filename, bool eager);
/// Get symbols from library handle.
///
/// [r:] `NULL` if error happened.
CBUILDDEF void* cbuild_dlib_get_sym(cbuild_dlib_t handle, const char* name);
/// Close dynamic library handle and unload library.
CBUILDDEF void cbuild_dlib_close(cbuild_dlib_t handle);
