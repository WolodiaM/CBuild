/**
 * @file framework.h
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Test framework shared header
 * Shared datastructures and some assert magic
 *
 * @date 2025-09-14
 * @copyright (C) 2025 WolodiaM
 * @license GPL-3.0-or-later
 *
 * Copyright (C) 2025 WolodiaM
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef __FRAMEWORK_H__
#define __FRAMEWORK_H__
// Includes
#define CBUILD_IMPLEMENTATION
#include "../cbuild.h"
// Asserts
#define TEST_ASSERT_EQ(val, expected, msg, ...)                                \
	if ((val) != (expected)) {                                                   \
		printf(msg "\n" __VA_OPT__(,) __VA_ARGS__);                                \
		failed++;                                                                  \
	}
#define TEST_ASSERT_NEQ(val, expected, msg, ...)                               \
	if ((val) == (expected)) {                                                   \
		printf(msg "\n" __VA_OPT__(,) __VA_ARGS__);                                \
		failed++;                                                                  \
	}
#define TEST_ASSERT_STREQ(val, expected, msg, ...)                             \
	if (strcmp((val), (expected)) != 0) {                                        \
		printf(msg "\n" __VA_OPT__(,) __VA_ARGS__);                                \
		failed++;                                                                  \
	}
#define TEST_ASSERT_MEMEQ(val, expected, size, msg, ...)                       \
	if (memcmp((void*)(val), (void*)(expected), size) != 0) {                    \
		printf(msg "\n" __VA_OPT__(,) __VA_ARGS__);                                \
		failed++;                                                                  \
	}
#define TEST_ASSERT(func, msg, ...)                                            \
	if (!((func))) {                                                             \
		printf(msg "\n" __VA_OPT__(,) __VA_ARGS__);                                \
		failed++;                                                                  \
	}
#define TEST_NASSERT(func, msg, ...)                                           \
	if ((func)) {                                                                \
		printf(msg "\n" __VA_OPT__(,) __VA_ARGS__);                                \
		failed++;                                                                  \
	}
#define TEST_EXPECT_MSG(type)  ", expected '%" #type "' but found '%" #type "'."
#define TEST_EXPECT_RMSG(type) ", expected '" type "' but found '" type "'."
#endif // __FRAMEWORK_H__
