/**
 * @file common.c
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Tests for some common and untructured modules
 *
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
// Project includes
#include "../src/common.h"
#include "framework.h"
// Code
TEST_MAIN(
		{
			TEST_CASE(
					{
						int arr[3];
						arr[0] = 1;
						arr[1] = 2;
						arr[2] = 3;
						TEST_ASSERT_EQ(cbuild_arr_len(arr), 3,
														"Incorect array length calculated, calculated "
														"\"%lu\", expected: \"%d\"",
														cbuild_arr_len(arr), 3);
						TEST_ASSERT_EQ(cbuild_arr_get(arr, 1), 2,
														"Wrong element at array at position \"1\" - "
														"\"%d\", expected: \"%d\"",
														cbuild_arr_get(arr, 1), 2);
					},
					"Array oprations");
			TEST_CASE(
					{
						int	 arr[3];
						int* test_ptr = arr;
						test_ptr++;
						int* arr_ptr = arr;
						arr[0]			 = 1;
						arr[1]			 = 2;
						arr[2]			 = 3;
						int arr_sz	 = 3;
						int val			 = cbuild_shift(arr_ptr, arr_sz);
						TEST_ASSERT_EQ(
								val, 1, "Wrong element shifted: \"%d\", expected: \"1\"", val);
						TEST_ASSERT_EQ(
								arr_sz, 2,
								"Wrong array length after shift: \"%d\", expected: \"2\"",
								arr_sz);
						TEST_ASSERT_EQ(arr_ptr, test_ptr,
														"Wrong array pointer: \"%p\", expected to have "
														"\"arr+4\": \"%p+4\"",
														(void*)arr_ptr, (void*)arr);
					},
					"cbuild_shift");
		},
		"Common modules test")
