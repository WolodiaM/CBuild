/**
 * @file StringBuffer.h
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief StringBuffer implementation
 * Dynamically resizable non-null-terminated string (optically null terminator
 * can be inserted) with length-tracking and ability to modify it on the fly.
 *
 * @date 2024-12-04
 * @copyright (C) 2024 WolodiaM
 * @license MIT
 *
 * Copyright (C) 2024 WolodiaM
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
// Code
#ifndef __CBUILD_STR_BUFF_H__
#define __CBUILD_STR_BUFF_H__
// Includes
#include "DynArray.h"
// Code
/**
 * @class CBuildStrBuff
 * @brief String buffer struct
 *
 * Buffer is not opaque to user, but need to be threated as read-only except for
 * a specifc APIs. String in the `data` field is not NULL-terminated by default,
 * special function exests to do so
 */
typedef struct {
	char*	 data;
	size_t size;
	size_t capacity;
} CBuildStrBuff;
/**
 * @brief Append element to a string buffer
 *
 * @param sb => CBuildStrBuff * -> String buffer to operate on
 * @param val => char -> Character that will be inserted into string
 */
#define cbuild_sb_append(sb, char) cbuild_da_append(sb, char)
/**
 * @brief Append element to a string buffer
 *
 * @param sb => CBuildStrBuff * -> String buffer to operate on
 * @param vals => char -> Array of chars that need to be inserted
 * @param vals_cnt => size_t -> Number of elements to insert
 */
#define cbuild_sb_append_arr(sb, chars, chars_cnt)                             \
	cbuild_da_append_arr(sb, chars, chars_cnt)
/**
 * @brief Append element to a string buffer
 *
 * @param sb => CBuildStrBuff * -> String buffer to operate on
 * @param ... => char ... -> Chars that need to be inserted
 */
#define cbuild_sb_append_many(sb, ...)                                         \
	do {                                                                         \
		char tmp[] = { __VA_ARGS__ };                                              \
		cbuild_sb_append_arr((sb), tmp, (size_t)(sizeof(tmp) / sizeof(tmp[0])));   \
	} while (0);
/**
 * @brief Append C-string to a string buffer
 *
 * @param sb => CBuildStrBuff * -> String buffer to operate on
 * @param str => char* -> String to insert
 */
#define cbuild_sb_append_cstr(sb, str)                                         \
	cbuild_sb_append_arr(sb, str, strlen((str)))
/**
 * @brief Append NULL-terminator to a string buffer
 *
 * @param sb => CBuildStrBuff * -> String buffer to operate on
 */
#define cbuild_sb_append_null(sb) cbuild_sb_append(sb, '\0')
/**
 * @brief Remove NULL-terminator from a string buffer
 *
 * Character is not actually removed from a buffer, only size is decremented, so
 * memory that stores this character is marked as free. Only '\0' is removed by
 * this function.
 * @param sb => CBuildStrBuff * -> String buffer to operate on
 */
#define cbuild_sb_remove_null(sb)                                              \
	do {                                                                         \
		if ((sb)->data[(sb)->size - 1] == '\0') {                                  \
			(sb)->size--;                                                            \
		}                                                                          \
	} while (0);
/**
 * @brief Clear string buffer
 *
 * @param sb => CBuildStrBuff * -> String buffer to operate on
 */
#define cbuild_sb_clear(sb) cbuild_da_clear(sb)
/**
 * @brief strcmp for CBuildStrBuff
 *
 * @param sb1 => CBuildStrBuff * -> First string buffer
 * @param sb2 => CBuildStrBuff * -> Second String buffer
 * @return -2 -> If size of first string buffer is smaller
 * @return -1 -> If first different character in first string buffer is smaller
 * @return 0  -> If two string buffers are equal
 * @return 1  -> If first different character in first string buffer is larger
 * @return 2  -> If size of first string buffer is larger
 *
 */
int cbuild_sb_cmp(CBuildStrBuff* sb1, CBuildStrBuff* sb2);
#define CBuildSBFmt			"%.*s"
#define CBuildSBArg(sb) (int)(sb).size, (sb).data
#endif // __CBUILD_STR_BUFF_H__
