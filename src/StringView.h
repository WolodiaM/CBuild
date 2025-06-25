/**
 * @file StringView.h
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief String view library
 * Simple single-header string-view implementation
 *
 * @date 2025-02-22
 * @copyright (C) 2025 WolodiaM
 * @license MIT
 *
 * Copyright (C) 2025 WolodiaM
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef __CBUILD_SV_H__
#define __CBUILD_SV_H__
// Project includes:
#include "common.h"
// Code
typedef struct cbuild_sv_t {
	char*  data;
	size_t size;
} cbuild_sv_t;
// Code
#define CBuildSVFmt     "%.*s"
#define CBuildSVArg(sv) (int)(sv).size, (sv).data
/**
 * @brief Create string view
 *
 * @param data => const char* -> Char array
 * @param size => size_t -> Length of a view
 * @return cbuild_sv_t -> New string view
 */
cbuild_sv_t cbuild_sv_from_parts(const char* data, size_t size);
/**
 * @brief Create string view from c string
 *
 * @param cstr => const char* -> C-string
 * @return cbuild_sv_t -> New string view
 */
cbuild_sv_t cbuild_sv_from_cstr(const char* cstr);
/**
 * @brief Trim whitespaces from a string view starting from the left
 *
 * @param sv => cbuild_sv_t* -> String view to work with
 * @return size_t -> Number of characters removed
 */
size_t      cbuild_sv_trim_left(cbuild_sv_t* sv);
/**
 * @brief Trim whitespaces from a string view starting from the right
 *
 * @param sv => cbuild_sv_t* -> String view to work with
 * @return size_t -> Number of characters removed
 */
size_t      cbuild_sv_trim_right(cbuild_sv_t* sv);
/**
 * @brief Trim whitespaces from a string view starting from both sides
 *
 * @param sv => cbuild_sv_t* -> String view to work with
 * @return size_t -> Number of characters removed
 */
size_t      cbuild_sv_trim(cbuild_sv_t* sv);
/**
 * @brief Chop characters from one string view into another
 * Note: Original string view will be truncated
 * @param sv => cbuild_sv_t* -> String view to work with
 * @param size => size_t -> Number of characters to chop
 * @return cbuild_sv_t -> New string view
 */
cbuild_sv_t cbuild_sv_chop(cbuild_sv_t* sv, size_t size);
/**
 * @brief Chop characters from one string view into another
 * Note: Original string view will be truncated. delim will be removed from both
 * string views
 * @param sv => cbuild_sv_t* -> String view to work with
 * @param delim => char -> Character on which function stops.
 * @return cbuild_sv_t -> New string view
 */
cbuild_sv_t cbuild_sv_chop_by_delim(cbuild_sv_t* sv, char delim);
/**
 * @brief Chop characters from one string view into another
 * Note: Original string view will be truncated. delim will be removed from both
 * string views
 * @param sv => cbuild_sv_t* -> String view to work with
 * @param delim => cbuild_sv_t -> Character on which function stops.
 * @return cbuild_sv_t -> New string view
 */
cbuild_sv_t cbuild_sv_chop_by_sv(cbuild_sv_t* sv, cbuild_sv_t delim);
/**
 * @brief Delimiter func for sv_chop_by_delim
 *
 * @param sv => const cbuild_sv_t* -> String view currently being chopped
 * @param idx => size_t -> Current scan index
 * @param args => void* -> Some user arguments
 * @return bool -> True if this character should be marked as a delimiter
 */
typedef bool (*cbuild_sv_delim_func)(const cbuild_sv_t* sv, size_t idx,
                                     void* args);
/**
 * @brief Chop characters from one string view into another
 * Note: Original string view will be truncated. Index which function marked as
 * an end will not be in any string view string views.
 * @param sv => cbuild_sv_t* -> String view to work with
 * @param delim => cbuild_sv_delim_func -> Delimiter scanner
 * @param args => void* -> Will be passed into delim function
 * @return cbuild_sv_t -> New string view
 */
cbuild_sv_t cbuild_sv_chop_by_func(cbuild_sv_t* sv, cbuild_sv_delim_func delim,
                                   void* args);
/**
 * @brief strcmp for string view
 *
 * @param a => cbuild_sv_t -> First StringView
 * @param b => cbuild_sv_t -> Second StringView
 * @return -2 -> If size of first string view is smaller
 * @return -1 -> If first different character in first string view is smaller
 * @return 0  -> If two string views are equal
 * @return 1  -> If first different character in first string view is larger
 * @return 2  -> If size of first string view is larger
 */
int         cbuild_sv_cmp(cbuild_sv_t a, cbuild_sv_t b);
/**
 * @brief compare two string view ignoring case of a latin letters for string
 * view
 *
 * @param a => cbuild_sv_t -> First StringView
 * @param b => cbuild_sv_t -> Second StringViewr
 * @return -2 -> If size of first string view is smaller
 * @return -1 -> If first different character in first string view is smaller
 * @return 0  -> If two string views are equal
 * @return 1  -> If first different character in first string view is larger
 * @return 2  -> If size of first string view is larger
 */
int         cbuild_sv_cmp_icase(cbuild_sv_t a, cbuild_sv_t b);
/**
 * @brief Check if string string view has specific prefix
 *
 * @param sv => cbuild_sv_t -> String view to work with
 * @param prefix => cbuild_sv_t -> Prefix
 * @return bool -> True if sv contains prefix
 */
bool        cbuild_sv_prefix(cbuild_sv_t sv, cbuild_sv_t prefix);
/**
 * @brief Check if string string view has specific suffix
 *
 * @param sv => cbuild_sv_t -> String view to work with
 * @param suffix => cbuild_sv_t -> Prefix
 * @return bool -> True if cbuild_sv contains suffix
 */
bool        cbuild_sv_suffix(cbuild_sv_t sv, cbuild_sv_t suffix);
/**
 * @brief Find characted in a string
 *
 * @param sv => cbuild_sv_t -> String view to work with
 * @param c => char -> Charcter to look for
 * @return size_t -> Index of a character or -1
 */
size_t      cbuild_sv_find(cbuild_sv_t sv, char c);
/**
 * @brief Find characted in a string starting from the back
 *
 * @param sv => cbuild_sv_t -> String view to work with
 * @param c => char -> Charcter to look for
 * @return size_t -> Index of a character or -1
 */
size_t      cbuild_sv_rfind(cbuild_sv_t sv, char c);
/**
 * @brief Check if string view contains the cracter
 *
 * @param sv => cbuild_sv_t -> String view to work with
 * @param c => char -> Charcter to look for
 * @return bool -> Contains or not
 */
bool        cbuild_sv_contains(cbuild_sv_t sv, char c);
#endif // __CBUILD_SV_H_
