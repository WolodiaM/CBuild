#pragma once // For LSP 
//! Implementation of string view in C.

#include "Common.h"

/// String view datatype
///
/// * [fl:data] Pointer into data. This pointer does not "own" its data.
/// * [fl:size] Length of string view. Data in range from `data`{.c} to `data+size`{.c} is guaranteed to be valid.
typedef struct cbuild_sv_t {
	char*  data;
	size_t size;
} cbuild_sv_t;
/// Format description for string view for printf-style formatters.
#define CBuildSVFmt     "%.*s"
/// Format argument for string view for printf-style formatters.
#define CBuildSVArg(sv) (int)(sv).size, (sv).data
/// Create new string view from raw parts
#define cbuild_sv_from_parts(_data, _size)                   \
	((cbuild_sv_t){ .data = (char*)(_data), .size = (_size) })
/// Create new string view from C-string. 
#define cbuild_sv_from_cstr(cstr)                                \
((cbuild_sv_t){ .data = (char*)(cstr), .size = strlen((cstr)) })
/// Create new string view from C-string literal (compile-time). 
#define cbuild_sv_from_lit(lit)                                  \
((cbuild_sv_t){ .data = (char*)(lit), .size = sizeof(lit) - 1 })
/// Trim whitespaces from a string view starting from the left.
///
/// [r:] Number of characters removed.
CBUILDDEF size_t cbuild_sv_trim_left(cbuild_sv_t* sv);
/// Trim whitespaces from a string view starting from the right.
///
/// [r:] Number of characters removed.
CBUILDDEF size_t cbuild_sv_trim_right(cbuild_sv_t* sv);
/// Trim whitespaces from a string view starting from both sides.
///
/// [r:] Number of characters removed.
CBUILDDEF size_t cbuild_sv_trim(cbuild_sv_t* sv);
/// Chop [p:size] characters from string view starting from the left.
///
/// * [pl:sv:string_view_t*] String view from which data will be chopped. Its size will be reduced.
/// * [pl:size] Number of character that will be chopped.
///
/// [r:] New string view containing chopped characters.
CBUILDDEF cbuild_sv_t cbuild_sv_chop(cbuild_sv_t* sv, size_t size);
/// Chop [p:size] characters from string view starting from the right.
///
/// * [pl:sv:string_view_t*] String view from which data will be chopped. Its size will be reduced.
/// * [pl:size] Number of character that will be chopped.
///
/// [r:] New string view containing chopped characters.
CBUILDDEF cbuild_sv_t cbuild_sv_chop_right(cbuild_sv_t* sv, size_t size);
/// Chop characters from string view starting from the left until [p:delim] is found.
///
/// * [pl:sv:string_view_t*] String view from which data will be chopped. Its size will be reduced.
/// * [pl:delim] Delimiter character.
///
/// [r:] New string view containing chopped characters. [p:delim] will not be included.
CBUILDDEF cbuild_sv_t cbuild_sv_chop_by_delim(cbuild_sv_t* sv, char delim);
/// Chop characters from string view starting from the right until [p:delim] is found.
///
/// * [pl:sv:string_view_t*] String view from which data will be chopped. Its size will be reduced.
/// * [pl:delim] Delimiter character.
///
/// [r:] New string view containing chopped characters. [p:delim] will not be included.
CBUILDDEF cbuild_sv_t cbuild_sv_chop_right_by_delim(cbuild_sv_t* sv, char delim);
/// Chop characters from string view starting from the left until [p:delim] is found.
///
/// * [pl:sv:string_view_t*] String view from which data will be chopped. Its size will be reduced.
/// * [pl:delim] Delimiter string view.
///
/// [r:] New string view containing chopped characters. [p:delim] will not be included.
CBUILDDEF cbuild_sv_t cbuild_sv_chop_by_sv(cbuild_sv_t* sv, cbuild_sv_t delim);
/// Chop characters from string view starting from the right until [p:delim] is found.
///
/// * [pl:sv:string_view_t*] String view from which data will be chopped. Its size will be reduced.
/// * [pl:delim] Delimiter string view.
///
/// [r:] New string view containing chopped characters. [p:delim] will not be included.
CBUILDDEF cbuild_sv_t cbuild_sv_chop_right_by_sv(cbuild_sv_t* sv, cbuild_sv_t delim);
/// Delimiter function. Used as comparator in [`cbuild_sv_chop_by_func`](#cbuild_sv_chop_by_func)
/// and [`cbuild_sv_chop_right_by_func`](#cbuild_sv_chop_right_by_func)
///
/// * [pl:sv] String view passed to function.
/// * [pl:idx] Current character.
/// * [pl:args] Arguments passed by caller.
///
/// [r:] If true is returned this character will be considered a delimiter
typedef bool (*cbuild_sv_delim_func)(const cbuild_sv_t* sv, size_t idx, void* args);
/// chop characters from string view starting from the left until [p:delim] returns `true`.
///
/// * [pl:sv:string_view_t*] String view from which data will be chopped. its size will be reduced.
/// * [pl:delim] Delimiter function.
/// * [pl:args] Arguments to a function.
///
/// [r:] new string view containing chopped characters. delimeter character will not be in any string view.
CBUILDDEF cbuild_sv_t cbuild_sv_chop_by_func(cbuild_sv_t* sv,
	cbuild_sv_delim_func delim, void* args);
/// chop characters from string view starting from the right until [p:delim] returns `true`.
///
/// * [pl:sv:string_view_t*] String view from which data will be chopped. Its size will be reduced.
/// * [pl:delim] Delimiter function.
/// * [pl:args] Arguments to a function.
///
/// [r:] new string view containing chopped characters. Delimiter character will not be in any string view.
CBUILDDEF cbuild_sv_t cbuild_sv_chop_right_by_func(cbuild_sv_t* sv,
	cbuild_sv_delim_func delim, void* args);
/// `strcmp` for string view. Fully compatible but extends API. 
///
/// [r:-2] If size of first string view is smaller.
/// [r:-1] If first different character in first string view is smaller.
/// [r:0] If two string views are equal.
/// [r:1] If first different character in first string view is larger.
/// [r:2] If size of first string view is larger.
CBUILDDEF int cbuild_sv_cmp(cbuild_sv_t a, cbuild_sv_t b);
/// `strcmp` for string view. Fully compatible but extends API. 
/// Performs case folding for ASCII.
///
/// [r:-2] If size of first string view is smaller.
/// [r:-1] If first different character in first string view is smaller.
/// [r:0] If two string views are equal.
/// [r:1] If first different character in first string view is larger.
/// [r:2] If size of first string view is larger.
CBUILDDEF int cbuild_sv_cmp_icase(cbuild_sv_t a, cbuild_sv_t b);
/// Check if string view has specific prefix.
CBUILDDEF bool cbuild_sv_prefix(cbuild_sv_t sv, cbuild_sv_t prefix);
/// Check if string view has specific suffix.
CBUILDDEF bool cbuild_sv_suffix(cbuild_sv_t sv, cbuild_sv_t suffix);
/// Find specific character in string view.
///
/// [r:] Index of character or `-1`{.c} if not found
CBUILDDEF ssize_t cbuild_sv_find(cbuild_sv_t sv, char c);
/// Find specific substring (provided as string view) in another string view.
///
/// [r:] Index of first character of [p:needle] or `-1`{.c} if not found
CBUILDDEF ssize_t cbuild_sv_find_sv(cbuild_sv_t sv, cbuild_sv_t needle);
/// Find specific character in string view. Searches from the end.
///
/// [r:] Index of character or `-1`{.c} if not found
CBUILDDEF ssize_t cbuild_sv_rfind(cbuild_sv_t sv, char c);
/// Check if string view contains some character.
CBUILDDEF bool cbuild_sv_contains(cbuild_sv_t sv, char c);
/// Check substring (provided as string view) is contained in another string view.
CBUILDDEF bool cbuild_sv_contains_sv(cbuild_sv_t sv, cbuild_sv_t needle);
/// Get length of first utf8 codepoint in string view.
///
/// ::: note
/// Invalid codepoints assumed to have length of 1
/// :::
///
/// [r:] Length of codepoint in bytes.
CBUILDDEF int cbuild_sv_utf8cp_len(cbuild_sv_t sv);
/// `strchr` implementation for string view.
///
/// * [pl:sv] String view to work with
/// * [pl:c] Character that will be searched.
///
/// [r:] Pointer to character or NULL if not found.
CBUILDDEF char* cbuild_sv_strchr(cbuild_sv_t sv, char c);
/// `strchr` implementation for string view.
/// Operates on utf8 codepoints instead of ASCII characters.
///
/// * [pl:sv] String view to work with
/// * [pl:c] Codepoint that will be searched.
///
/// [r:] Pointer to character or NULL if not found.
CBUILDDEF char* cbuild_sv_utf8chr(cbuild_sv_t sv, uint32_t c);
/// Chop first utf8 codepoint out of string view.
///
/// [r:] Unicode codepoint. `UINT32_MAX`{.c} means error.
CBUILDDEF uint32_t cbuild_sv_chop_utf8(cbuild_sv_t* sv);
/// Chop characters from string view starting from the left until [p:delim] is found.
/// Operates on utf8 codepoints instead of ASCII characters.
///
/// * [pl:sv:string_view_t*] String view from which data will be chopped. Its size will be reduced.
/// * [pl:delim] Delimiter character.
///
/// [r:] New string view containing chopped characters. [p:delim] will not be included.
CBUILDDEF cbuild_sv_t cbuild_sv_chop_by_utf8(cbuild_sv_t* sv, uint32_t delim);
/// Delimiter function. Used as comparator in 
/// [`cbuild_sv_chop_by_func_utf8`](#cbuild_sv_chop_by_func_utf8)
///
/// * [pl:sv] String view, prechopped, so first character is possible delimiter and should be checked.
/// * [pl:args] Arguments passed by caller.
///
/// [r:] If true is returned this character will be considered a delimiter
typedef bool (*cbuild_sv_utf8delim_func)(const cbuild_sv_t* sv, void* args);
/// chop characters from string view starting from the right until [p:delim] returns `true`.
/// Operates on utf8 codepoints instead of ASCII characters.
///
/// * [pl:sv:string_view_t*] String view from which data will be chopped. Its size will be reduced.
/// * [pl:delim] Delimiter function.
/// * [pl:args] Arguments to a function.
///
/// [r:] new string view containing chopped characters. Delimiter character will not be in any string view.
CBUILDDEF cbuild_sv_t cbuild_sv_chop_by_func_utf8(cbuild_sv_t* sv,
	cbuild_sv_utf8delim_func delim, void* args);
/// Get length of a string view in utf8 codepoints.
///
/// ::: note
/// Invalid codepoints assumed to have length of 1
/// :::
CBUILDDEF size_t cbuild_sv_utf8len(cbuild_sv_t sv);
/**
 * @brief Validate string as utf8-based
 *
 * @param idx => size_t* -> If non-NULL here will be stored	index of first
 * invalid codepoint, if validation failed.
 * @return bool -> true if valid, false otherwise
 */
/// Check if string view is a valid utf8 data.
///
/// * [pl:sv] String view to work with.
/// * [pl:idx] If validation failed this will store index of first invalid codepoint. Can be `NULL`{.c}.
///
/// [r:] `false` if validation failed.
CBUILDDEF bool cbuild_sv_utf8valid(cbuild_sv_t sv, size_t* idx);
/// Convert string view to c-string. Allocate memory via `malloc`.
CBUILDDEF char* cbuild_sv_to_cstr(cbuild_sv_t sv);
/// Convert string view to c-string. Allocate memory via `malloc`.
#define cbuild_cstr_from_sv(sv) cbuild_sv_to_cstr(sv)
/// Convert string view to c-string. Allocate memory via [`cbuild_temp_malloc`](Temp.html#cbuild_temp_malloc).
CBUILDDEF char* cbuild_sv_to_temp_cstr(cbuild_sv_t sv);
/// Convert string view to c-string. Allocate memory via [`cbuild_temp_malloc`](Temp.html#cbuild_temp_malloc).
#define cbuild_temp_cstr_from_sv(sv) cbuild_temp_sv_to_cstr(sv)
