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
 * @param proc => CBuildProc -> Process ID
 * @return true -> Process finished without errors
 * @return false -> Process finished with an error
 */
bool cbuild_proc_wait(CBuildProc proc);
/**
 * @brief Wait until process finishes execurition
 *
 * @param proc => CBuildProc -> Process ID
 * @return int -> Process exit code
 */
int	 cbuild_proc_wait_code(CBuildProc proc);
#endif // __CBUILD_PROC_H__
