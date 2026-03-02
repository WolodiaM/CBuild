#pragma once // For LSP 
//! Dynamic string - string builder. Specialization of DynArray.h for `char`{.c} datatype.

#include "Common.h"
#include "StringView.h"

/// String builder string builder.
///
/// * [fl:sbta] Pointer to data. This pointer "owns" the data.
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
/// Append new element to string builder.
///
/// * [pl:sb:cbuild_sb_t*] String builder object.
/// * [pl:elem:char] Element to append.
#define cbuild_sb_append(sb, elem) cbuild_da_append((sb), (elem))
/// Append new elements to string builder.
///
/// * [pl:sb:cbuild_sb_t*] String builder object.
/// * [pl:arr:char*] Elements to append.
/// * [pl:arr_size:size_t] Number of elements to append.
#define cbuild_sb_append_arr(sb, arr, size) cbuild_da_append_arr((sb), (arr), (size))
/// Append new elements to string builder.
///
/// * [pl:sb:cbuild_sb_t*] String builder object.
/// * [pl:...:char] Elements to append.
#define cbuild_sb_append_many(sb, ...) cbuild_da_append_many((sb), __VA_ARGS__)
/// Append c-string to string builder.
///
/// * [pl:sb:cbuild_sb_t*] String builder object.
/// * [pl:cstr:char*] C-string (NULL-terminated).
#define cbuild_sb_append_cstr(sb, cstr)	cbuild_da_append_arr((sb), (cstr), strlen(cstr))
/// Append NULL-terminator to string builder.
///
/// * [pl:sb:cbuild_sb_t*] String builder object.
#define cbuild_sb_append_null(sb) cbuild_da_append((sb), '\0')
/// Set element in string builder using its index.
///
/// * [pl:sb:cbuild_sb_t*] String builder object.
/// * [pl:idx:size_t] Element index.
/// * [pl:elem:char] New element's value.
/// 
/// [r:bool] False on overflow.
#define cbuild_sb_set(sb, idx, elem) cbuild_da_set((sb), (idx), (elem))
/// Get element from string builder using its index.
///
/// * [pl:sb:const cbuild_sb_t*] String builder object.
/// * [pl:idx:size_t] Element index.
/// 
/// [r:char] Pointer to an element or `NULL`{.c} if index out of bounds.
#define cbuild_sb_get(sb, idx) cbuild_da_get((sb), (idx))
/// Remove element string builder using its index.
///
/// * [pl:sb:cbuild_sb_t*] String builder object.
/// * [pl:idx:size_t] Element index.
/// 
/// [r:bool] False on overflow.
#define cbuild_sb_remove(sb, idx) cbuild_da_remove((sb), (idx))
/// Resize string builder.
///
/// * [pl:sb:cbuild_sb_t*] String builder object.
/// * [pl:new_size:size_t] Resize string builder. If new_size equal 0 default growth strategy will be used.
#define cbuild_sb_resize(sb, new_capacity) cbuild_da_resize((sb), (new_capacity))
/// Free string builder.
///
/// This should be used only if you want to free underlying memory, if you just
/// want to clear array and reuse it, setting `size` to `0` would be much faster.
///
/// * [pl:sb:cbuild_sb_t*] String builder object.
#define cbuild_sb_clear(sb) cbuild_da_clear(sb)
/// Foreach implementation for string builder. Same semantics as 'for' loop.
///
/// * [pl:sb:cbuild_sb_t*] String builder object.
/// * [pl:iter:name] Name if variable that will be used as iterator. Will have type `char`{.c}.
#define cbuild_sb_foreach(sb, iter) cbuild_da_foreach(sb, iter)
/// `strcmp` for string builder. Fully compatible but extends API. 
///
/// [r:-2] If size of first string builder is smaller.
/// [r:-1] If first different character in first string builder is smaller.
/// [r:0] If two string builders are equal.
/// [r:1] If first different character in first string builder is larger.
/// [r:2] If size of first string builder is larger.
CBUILDDEF int cbuild_sb_cmp(cbuild_sb_t a, cbuild_sb_t b);
/// `strcmp` for string builder. Fully compatible but extends API. 
/// Performs case folding for ASCII.
///
/// [r:-2] If size of first string builder is smaller.
/// [r:-1] If first different character in first string builder is smaller.
/// [r:0] If two string builders are equal.
/// [r:1] If first different character in first string builder is larger.
/// [r:2] If size of first string builder is larger.
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
