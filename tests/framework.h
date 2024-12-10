/**
 * @file framework.h
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Very simple test framework
 * Allows to register test. Register test case. Do assertion. And pretty print
 * results.
 *
 * @date 2024-12-05
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
#ifndef INCLUDE_TESTS_FRAMEWORK_H_
#define INCLUDE_TESTS_FRAMEWORK_H_
// Includes
#include "assert.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "strings.h"
// Code
#define CHECK_CMP_VALUE(val, expected, msg, ...)                               \
	if (val != expected) {                                                       \
		printf(msg "\n", __VA_ARGS__);                                             \
		err_code++;                                                                \
	}

#define CHECK_CMP_STR(val, expected, msg, ...)                                 \
	if (strcmp(val, expected) != 0) {                                            \
		printf(msg "\n", __VA_ARGS__);                                             \
		err_code++;                                                                \
	}

#define CHECK_CMP_MEM(val, expected, size, msg, ...)                           \
	if (memcmp((void*)val, (void*)expected, size) != 0) {                        \
		printf(msg "\n", __VA_ARGS__);                                             \
		err_code++;                                                                \
	}

#define TEST_CASE(code, info)                                                  \
	{                                                                            \
		int err_code = 0;                                                          \
		printf(info "\n");                                                         \
		code;                                                                      \
		if (err_code == 0) {                                                       \
			printf("Test case succeed\n");                                           \
		} else {                                                                   \
			printf("Test case failed. Number of failed checks: %d\n", err_code);     \
			global_err_code++;                                                       \
		}                                                                          \
		printf("\n");                                                              \
	}

#define TEST_MAIN(code, info)                                                  \
	int main(int argc, char** argv) {                                            \
		int global_err_code = 0;                                                   \
		printf(info "\n------------------------------\n");                         \
		code;                                                                      \
		if (global_err_code == 0) {                                                \
			printf("Test succeed\n");                                                \
		} else {                                                                   \
			printf("Test failed. Number of failed subtests: %d\n", global_err_code); \
		}                                                                          \
		return global_err_code;                                                    \
	}

#endif // INCLUDE_TESTS_FRAMEWORK_H_
