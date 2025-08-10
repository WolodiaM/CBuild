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
void* cbuild_temp_alloc(size_t size);
char* cbuild_temp_sprintf(char* fmt, ...);
char* cbuild_temp_vsprintf(char* fmt, va_list ap);
char* cbuild_temp_strdup(char* str);
void* cbuild_temp_memdup(void* mem, size_t size);
void cbuild_temp_reset(void);
#endif // __CBUILD_ARENA_H__
