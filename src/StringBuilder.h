/**
 * @file StringBuilder.h
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief StringBuilder for C
 * Simply creates a dynamic array for 'char' datatype. Also defines few function
 * that are appropriate only in a string context.
 *
 * @date 2025-06-09
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
#ifndef __CBUILD_SB_H__
#define __CBUILD_SB_H__
// Project includes:
#include "DynArray.h"
#include "StringView.h"
#include "common.h"
// Code
#define CBuildSBFmt     "%.*s"
#define CBuildSBArg(sb) (int)(sb).size, (sb).data
typedef struct cbuild_sb_t {
	char* data;
	size_t size;
	size_t capacity;
} cbuild_sb_t;
/**
 *  @brief Append character to a string builder
 *
 *  @param sb => cbuild_sb_t* -> String builder. Will be evaluated multiple times.
 *  @param elem => char -> Character
 */
#define cbuild_sb_append(sb, elem) cbuild_da_append((sb), (elem))
/**
 * @brief Append an array to a sb
 *
 * @param sb => cbuild_sb_t* -> String builder. Will be evaluated multiple times.
 * @param arr => char* -> Array pointer
 * @param size => size_t -> Number of new elements
 */
#define cbuild_sb_append_arr(sb, arr, size)                                    \
	cbuild_da_append_arr((sb), (arr), (size))
/**
 * @brief Append list of chars to a string builder
 *
 * @param sb => cbuild_sb_t* -> String builder. Will be evaluated multiple times. to operate on
 * @param ... => char ... -> Chars that need to be inserted
 */
#define cbuild_sb_append_many(sb, ...)                                         \
	cbuild_da_append_many((sb),  __VA_ARGS__)
/**
 * @brief Append a C-string to a sb
 *
 * @param sb => cbuild_sb_t* -> String builder. Will be evaluated multiple times.
 * @param cstr => char* -> C-string
 */
#define cbuild_sb_append_cstr(sb, cstr)                                        \
	cbuild_da_append_arr((sb), (cstr), strlen(cstr))
/**
 * @brief Append NULL-terminator to a string builder
 *
 * @param sb => cbuild_sb_t* -> String builder. Will be evaluated multiple times.
 */
#define cbuild_sb_append_null(sb) cbuild_da_append((sb), '\0')
/**
 * @brief Set a character in a sb using index
 *
 * @param sb => cbuild_sb_t* -> String builder. Will be evaluated multiple times.
 * @param idx => size_t -> Character index
 * @param elem => char -> New character
 */
#define cbuild_sb_set(sb, idx, elem) cbuild_da_set((sb), (idx), (elem))
/**
 * @brief Get a character from a sb using index
 *
 * @param sb => cbuild_sb_t* -> String builder. Will be evaluated multiple times.
 * @param idx => size_t -> Character index
 * @return VAL* -> Element
 */
#define cbuild_sb_get(sb, idx) cbuild_da_get((sb), (idx))
/**
 * @brief Remove an element from a sb using index
 *
 * @param sb => cbuild_sb_t* -> Dynamic array
 * @param idx => size_t -> Element index
 */
#define cbuild_sb_remove(sb, idx) cbuild_da_remove((sb), (idx))
/**
 * @brief Resize sb (Done automatically most of the times ;) )
 *
 * @param sb => cbuild_sb_t* -> String builder. Will be evaluated multiple times.
 * @param size => size_t -> New element count. String will be truncated if it
 * will be lover than sb->size. If it is zero then default behavior is used.
 */
#define cbuild_sb_resize(sb, new_capacity)                                     \
	cbuild_da_resize((sb), new_capacity)
/**
 * @brief Free sb
 *
 * @param sb => cbuild_sb_t* -> String builder. Will be evaluated multiple times.
 */
#define cbuild_sb_clear(sb) cbuild_da_clear(sb)
/**
 * @brief strcmp for string builder
 *
 * @param a => cbuild_sb_t* -> First string buffer
 * @param b => cbuild_sb_t* -> Second String buffer
 * @return -2 -> If size of first string builder is smaller
 * @return -1 -> If first different character in first string builder is smaller
 * @return 0  -> If two string builders are equal
 * @return 1  -> If first different character in first string builder is larger
 * @return 2  -> If size of first string builder is larger
 */
CBDEF int cbuild_sb_cmp(cbuild_sb_t* a, cbuild_sb_t* b);
/**
 * @brief Compare two string builders ignoring case of an ASCII letter (Latin
 * only)
 *
 * @param a => cbuild_sb_t* -> First string builder
 * @param b => cbuild_sb_t* -> Second string builder
 * @return -2 -> If size of first string builder is smaller
 * @return -1 -> If first different character in first StringBuiler is smaller
 * @return 0  -> If two string builders are equal
 * @return 1  -> If first different character in first string builder is larger
 * @return 2  -> If size of first string builder is larger
 */
CBDEF int cbuild_sb_cmp_icase(cbuild_sb_t* a, cbuild_sb_t* b);
/**
 * @brief Convert string builder to string view
 *
 * @param sb => cbuild_sb_t* -> String builder
 * @return cbuild_sv_t -> New string view
 */
CBDEF cbuild_sv_t cbuild_sb_to_sv(cbuild_sb_t* sb);
/**
 * @brief Convert string builder to string view
 *
 * @param sb => cbuild_sb_t* -> String builder
 * @return cbuild_sv_t -> New string view
 */
#define cbuild_sv_from_sb(sb) cbuild_sb_to_sv(sb)
/**
 * @brief Convert string view to a string builder. Does a copy.
 *
 * @param sv => cbuild_sv_t -> String view
 * @return cbuild_sb_t -> New string builder
 */
CBDEF cbuild_sb_t cbuild_sv_to_sb(cbuild_sv_t sv);
/**
 * @brief Convert string view to a string builder. Does a copy.
 *
 * @param sv => cbuild_sv_t -> String view
 * @return cbuild_sb_t -> New string builder
 */
#define cbuild_sb_from_sv(sv) cbuild_sv_to_sb(sv)
/**
 * @brief Append string view to a string builder
 *
 * @param sb => CBUILD_SB* -> String builder. Will be evaluated multiple times.
 * @param sv => cbuild_sv_t -> String view. Will be evaluated twice
 */
#define cbuild_sb_append_sv(sb, sv)                                            \
	cbuild_sb_append_arr((sb), (sv).data, (sv).size)
/**
 * @brief vsprintf for a string builder
 *
 * @param sb => cbuild_sb_t* -> String builder
 * @param fmt => const char* -> Format string
 * @param args => va_list -> Variadic arguments
 * @return int -> Number of bytes printed or -1 on error
 */
CBDEF int cbuild_sb_vappendf(cbuild_sb_t* sb, const char* fmt, va_list args);
/**
 * @brief sprintf for a string builder
 *
 * @param sb => cbuild_sb_t* -> String builder
 * @param fmt => const char* -> Format string
 * @param ... => ... -> Variadic arguments
 * @return int -> Number of bytes printed or -1 on error
 */
CBDEF int cbuild_sb_appendf(cbuild_sb_t* sb, const char* fmt, ...)
__attribute__((format(printf, 2, 3)));
/**
 * @brief Foreach loop
 *
 * @param sb => cbuild_sb_t* -> String builder. Will be evaluated multiple times.
 * @param iter => NAME -> Iteration value name
 */
#define cbuild_sb_foreach(sb, iter) cbuild_da_foreach(sb, iter)
/**
 * @brief Append unicode codepoint to string builder. Encoding will be utf8
 *
 * @param sb => cbuild_sb_t* -> String builder
 * @param cp => uint32_t -> Codepoint
 */
CBDEF void cbuild_sb_append_utf8(cbuild_sb_t* sb, uint32_t cp);
/**
 * @brief strcmp for string builder encoded as utf8.
 * Will use cbuild_sb_utf8cmp under the hood
 * @param a => cbuild_sb_t* -> First string buffer
 * @param b => cbuild_sb_t* -> Second String buffer
 * @return -2 -> If size of first string builder is smaller
 * @return -1 -> If first different character in first string builder is smaller
 * @return 0  -> If two string builders are equal
 * @return 1  -> If first different character in first string builder is larger
 * @return 2  -> If size of first string builder is larger
 */
CBDEF int cbuild_sb_utf8cmp(cbuild_sb_t* a, cbuild_sb_t* b);
/**
 * @brief Get lengths of a string builder with utf8 content
 *
 * @param sv => cbuild_sb_t -> String builder
 * @return size_t -> Number of encoded utf8 codepoints
 */
CBDEF size_t cbuild_sb_utf8len(cbuild_sb_t* sb);
#endif // __CBUILD_SB_H__
