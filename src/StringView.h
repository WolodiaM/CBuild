/**
 * @file StringView.h
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief String view
 * Implements string view over standard c-string. Don't manage memory and rely
 * on base string to be available. Manages size and can be resized
 * non-destructively.
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
#ifndef __CBUILD_STRING_VIEW_H__
#define __CBUILD_STRING_VIEW_H__
#include "StringBuffer.h"
typedef struct {
	const char* str;
	size_t			len;
} CBuildStrView;
/**
 * @brief Create string view from c-string
 *
 * @param str => char * -> String
 * @return CBuildStrView -> String view
 */
CBuildStrView cbuild_sv_from_cstr(const char* str);
/**
 * @brief Create string view from raw componets
 *
 * @param str => char* -> String
 * @param len => size_t -> Length of string view (must be greater or equal to
 * strlem(str))
 * @return CBuildStrView -> String view
 */
CBuildStrView cbuild_sv_from_parts(const char* str, size_t len);
/**
 * @brief Create string view from string buffer
 *
 * @param sb => CBuildStrBuff* -> String buffer
 * @return CBuildStrView -> String view
 */
CBuildStrView cbuild_sv_from_sb(const CBuildStrBuff* sb);
/**
 * @brief Trim character searching from the left
 *
 * @param str => CBuildStrView -> String view to work on
 * @param c => char -> Character that need to be stripped out
 * @return CBuildStrView -> New string view
 */
CBuildStrView cbuild_sv_trim_left(const CBuildStrView str, char c);
/**
 * @brief Trim character searching from the right
 *
 * @param str => CBuildStrView -> String view to work on
 * @param c => char -> Character that need to be stripped out
 * @return CBuildStrView -> New string view
 */
CBuildStrView cbuild_sv_trim_right(const CBuildStrView str, char c);
/**
 * @brief Cur string view by the delimeter, everything before delemeter is
 * removed
 *
 * @param str => CBuildStrView -> String view to work on
 * @param delim => char -> Delimeter
 * @return CBuildStrView -> New string view
 */
CBuildStrView cbuild_sv_cut_delim(const CBuildStrView str, char delim);
/**
 * @brief strcmp for CBuildStrView
 *
 * @param sb1 => CBuildStrView -> First string buffer
 * @param sb2 => CBuildStrView -> Second String buffer
 * @return -2 -> If size of first string buffer is smaller
 * @return -1 -> If first different character in first string buffer is smaller
 * @return 0  -> If two string buffers are equal
 * @return 1  -> If first different character in first string buffer is larger
 * @return 2  -> If size of first string buffer is larger
 *
 */
int						cbuild_sv_cmp(const CBuildStrView str1, const CBuildStrView str2);
#define CBuildSVFmt			"%.*s"
#define CBuildSVArg(sv) (int)(sv).len, (sv).str
#endif // __CBUILD_STRING_VIEW_H__
