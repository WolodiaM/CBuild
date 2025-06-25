/**
 * @file Log.h
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Logger
 * Very simple logger
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
#ifndef __CBUILD_LOG_H__
#define __CBUILD_LOG_H__
// Project includes
#include "common.h"
// Code
typedef enum {
	CBUILD_LOG_NO_LOGS = -1,
	CBUILD_LOG_ERROR   = 10,
	CBUILD_LOG_WARN    = 20,
	CBUILD_LOG_INFO    = 30,
	CBUILD_LOG_TRACE   = 40,
	CBUILD_LOG_PRINT   = 100,
} CBuildLogLevel;
typedef void (*CBuildLogFormatter)(CBuildLogLevel level);
/**
 * @brief Print logs
 *
 * @param level => CBuildLogLevel -> Type of log
 * @param fmt => const char* -> Format string
 * @param ... => varargs -> Values for format string
 */
void cbuild_log(CBuildLogLevel level, const char* fmt, ...)
		__attribute__((format(printf, 2, 3)));
/**
 * @brief Print logs but takes va list
 *
 * @param level => CBuildLogLevel -> Type of log
 * @param fmt => const char* -> Format string
 * @param args => va_list -> Variadic arguments
 */
void cbuild_vlog(CBuildLogLevel level, const char* fmt, va_list args);
/**
 * @brief Set minimim log level
 *
 * @param level => CBuildLogLevel -> Log level
 */
void cbuild_log_set_min_level(CBuildLogLevel level);
/**
 * @brief Set formatter for loger attributes
 *
 * @param fmt => CBuildLogFMT -> Log formatter
 */
void cbuild_log_set_fmt(CBuildLogFormatter fmt);
#endif // __CBUILD_LOG_H__
