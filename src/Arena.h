/**
 * @file Arena.h
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Arena allocator implementation for CBuild.
 * This impl is experimental and will change in future. For now it only proides
 * "temporary allocator". In future it will provide full allocaor.
 *
 * @date 2025-07-30
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
// Code
#ifndef __CBUILD_ARENA_H__
#define __CBUILD_ARENA_H__
// Includes
#include "common.h"
#include "Log.h"
// Code
/**
 * @brief Create new temp allocation
 *
 * @param size => size_t -> Number of bytes requested
 * @return void* -> Allocated pointer
 */
CBUILDDEF void* cbuild_temp_alloc(size_t size);
/**
 * @brief sprintf using temp allocation
 *
 * @param fmt => char* -> Format string
 * @param ... => variadics -> rguments
 * @return char* -> New string
 */
CBUILDDEF char* cbuild_temp_sprintf(char* fmt, ...)
__attribute__((format(printf, 1, 2)));
/**
 * @brief vsprintf using temp allocation
 *
 * @param fmt => char* -> Format string
 * @param ap. => va_list -> rguments
 * @return char* -> New string
 */
CBUILDDEF char* cbuild_temp_vsprintf(char* fmt, va_list ap);
/**
 * @brief strdup using temp allocation
 *
 * @param str => char* -> Original string
 * @return char* -> New dupped string
 */
CBUILDDEF char* cbuild_temp_strdup(char* str);
/**
 * @brief Dupes memory using temporary allocation
 *
 * @param mem => void* -> Original pointer
 * @param size => size_t -> Number of bytes to dup
 * @return char* -> New dupped memory block
 */
CBUILDDEF void* cbuild_temp_memdup(void* mem, size_t size);
/**
 * @brief Deallocate all temporary allocations
 */
CBUILDDEF void cbuild_temp_reset(void);
#endif // __CBUILD_ARENA_H__
