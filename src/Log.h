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
} cbuild_log_level_t;
typedef void (*cbuild_log_fmt_t)(cbuild_log_level_t level);
/**
 * @brief Print logs
 *
 * @param level => cbuild_log_level_t -> Type of log
 * @param fmt => const char* -> Format string
 * @param ... => varargs -> Values for format string
 */
CBDEF void cbuild_log(cbuild_log_level_t level, const char* fmt, ...)
__attribute__((format(printf, 2, 3)));
/**
 * @brief Print logs but takes va list
 *
 * @param level => cbuild_log_level_t -> Type of log
 * @param fmt => const char* -> Format string
 * @param args => va_list -> Variadic arguments
 */
CBDEF void cbuild_vlog(cbuild_log_level_t level, const char* fmt, va_list args);
/**
 * @brief Set minimum log level
 *
 * @param level => cbuild_log_level_t -> Log level
 */
CBDEF void cbuild_log_set_min_level(cbuild_log_level_t level);
/**
 * @brief Get minimum log level
 *
 * @return cbuild_log_level_t -> Current log level
 */
CBDEF cbuild_log_level_t cbuild_log_get_min_level(void);
/**
 * @brief Set formatter for loger attributes
 *
 * @param fmt => cbuild_log_fmt_t  -> Log formatter
 */
CBDEF void cbuild_log_set_fmt(cbuild_log_fmt_t fmt);
#endif // __CBUILD_LOG_H__
