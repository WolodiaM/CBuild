#pragma once // For LSP 
//! Dynamic string - string builder. Specialization of DynArray.h for `char`{.c} datatype.
//!
//! License: `GPL-3.0-or-later`.

#include "Common.h"
#include "StringView.h"

/// String builder string builder.
///
/// * [fl:data] Pointer to data. This pointer "owns" the data.
/// * [fl:size] Length of string builder. Data in range from `data`{.c} to `data+size`{.c} is guaranteed to be valid.
/// * [fl:capacity] Length of allocated data. Data is guaranteed to be allocated (but may be not initialized) from `data`{.c} to `data+size`{.c}.
typedef struct cbuild_sb_t {
	char* data;
	size_t size;
	size_t capacity;
} cbuild_sb_t;
/// Format description for string view for printf-style formatters.
#define CBuildSBFmt     "%.*s"
/// Format argument for string view for printf-style formatters.
#define CBuildSBArg(sb) (int)(sb).size, (sb).data
/// Append c-string to string builder.
///
/// * [pl:sb:cbuild_sb_t*] String builder object.
/// * [pl:cstr:char*] C-string (NULL-terminated).
#define cbuild_sb_append_cstr(sb, cstr)	cbuild_da_append_arr((sb), (cstr), strlen(cstr))
/// Append NULL-terminator to string builder.
///
/// * [pl:sb:cbuild_sb_t*] String builder object.
#define cbuild_sb_append_null(sb) cbuild_da_append((sb), '\0')
/// `strcmp` for string builder. Fully compatible but extends API. 
///
/// * [r:-2] If size of first string builder is smaller.
/// * [r:-1] If first different character in first string builder is smaller.
/// * [r:0] If two string builders are equal.
/// * [r:1] If first different character in first string builder is larger.
/// * [r:2] If size of first string builder is larger.
CBUILDDEF int cbuild_sb_cmp(cbuild_sb_t a, cbuild_sb_t b);
/// `strcmp` for string builder. Fully compatible but extends API. 
/// Performs case folding for ASCII.
///
/// * [r:-2] If size of first string builder is smaller.
/// * [r:-1] If first different character in first string builder is smaller.
/// * [r:0] If two string builders are equal.
/// * [r:1] If first different character in first string builder is larger.
/// * [r:2] If size of first string builder is larger.
CBUILDDEF int cbuild_sb_cmp_icase(cbuild_sb_t a, cbuild_sb_t b);
/// Append utf8 character to a string builder.
///
/// * [pl:sb] String builder object.
/// * [pl:cp] UTF8 codepoint.
CBUILDDEF void cbuild_sb_append_utf8(cbuild_sb_t* sb, uint32_t cp);
/// Get length of a string builder in utf8 codepoints.
///
/// ::: note
/// Invalid codepoints assumed to have length of 1
/// :::
CBUILDDEF size_t cbuild_sb_utf8len(cbuild_sb_t sb);
/// Convert string builder to a string view.
CBUILDDEF cbuild_sv_t cbuild_sb_to_sv(cbuild_sb_t sb);
/// Convert string builder to a string view.
#define	cbuild_sv_from_sb(sb) cbuild_sb_to_sv(sb)
/// Convert string view to a string builder (allocated new data).
CBUILDDEF cbuild_sb_t cbuild_sv_to_sb(cbuild_sv_t sv);
/// Convert string view to a string builder (allocated new data).
#define cbuild_sb_from_sv(sv) cbuild_sv_to_sb(sv)
/// Apppend string view to a string builder
///
/// * [pl:sb:cbuild_sb_t*] String builder object.
/// * [pl:cstr:cbuild_sv_t] String view.
#define cbuild_sb_append_sv(sb, sv) cbuild_sb_append_arr((sb), (sv).data, (sv).size)
/// Append data to a string builder using printf-style format string.
CBUILD_ATTR_PRINTF(2,
	CBUILDDEF int cbuild_sb_appendf(cbuild_sb_t* sb, const char* fmt, ...));
/// Append data to a string builder using printf-style format string.
CBUILDDEF int cbuild_sb_vappendf(cbuild_sb_t* sb, const char* fmt, va_list args);
