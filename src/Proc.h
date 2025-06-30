/**
 * @file Proc.h
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Process manager
 * Simple wrapper around supported APIs that allow to start/stop processes, wait
 * for process and redirect steams to it.
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
#ifndef __CBUILD_PROC_H__
#define __CBUILD_PROC_H__
#include "common.h"
/**
 * @brief Wait until process finishes execurition
 *
 * @param proc => cbuild_proc_t -> Process ID
 * @return true -> Process finished without errors
 * @return false -> Process finished with an error
 */
bool          cbuild_proc_wait(cbuild_proc_t proc);
/**
 * @brief Wait until process finishes execurition
 *
 * @param proc => cbuild_proc_t -> Process ID
 * @return int -> Process exit code. INT_MIN on invalid proc and INT_MAX if
 * process was closed on signal.
 */
int           cbuild_proc_wait_code(cbuild_proc_t proc);
/**
 * @brief Start another process with generic function
 *
 * @param callback => int (*)(void*) -> Callback for created process, return
 * value will be process exit status and context will be passed from parent
 * process
 * @param context => Will be passed to a callback
 * @return CBuildProc -> Process ID
 */
cbuild_proc_t cbuild_proc_start(int (*callback)(void* context), void* context);
/**
 * @struct cbuild_proc_ptr
 * @brief "Pointer" for cbuild_proc_malloc and cbuild_proc_free. Struct
 * because of munmap limitations.
 */
typedef struct cbuild_proc_ptr_t {
	void*  ptr;
	size_t size;
} cbuild_proc_ptr_t;
/**
 * @brief Allocates memory that can be shared between fork'ed processes
 *
 * @param n => size_t -> Number of bytes
 * @return cbuild_proc_ptr_t -> Pointer to a memory
 */
cbuild_proc_ptr_t cbuild_proc_malloc(size_t n);
/**
 * @brief free for cbuild_proc_malloc
 *
 * @param ptr => cbuild_proc_ptr_t -> Pointer (from cbuild_proc_malloc)
 */
void              cbuild_proc_free(cbuild_proc_ptr_t ptr);
#endif // __CBUILD_PROC_H__
