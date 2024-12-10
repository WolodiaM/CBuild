/**
 * @file DynArray.c
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Tests for dynamic array implementation
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
#include "../src/DynArray.h"
#include "framework.h"
// Code
struct int_array {
	int*	 data;
	size_t size;
	size_t capacity;
};
TEST_MAIN(
		{
			struct int_array arr = { 0 };
			TEST_CASE(
					{
						cbuild_da_append(&arr, 1);
						cbuild_da_append(&arr, 2);
						cbuild_da_append(&arr, 3);
						cbuild_da_append(&arr, 4);
						CHECK_CMP_VALUE(arr.size, 4,
														"Incorect element count in array: \"4\" expected "
														"but found \"%zu\"",
														arr.size);
						CHECK_CMP_VALUE(arr.capacity, CBUILD_DA_INIT_CAPACITY,
														"Wrong array capacity alocated, expected \"%zu\" "
														"but found \"%zu\"",
														CBUILD_DA_INIT_CAPACITY, arr.capacity);
						CHECK_CMP_VALUE(
								arr.data[0], 1,
								"Wrong element at index \"0\", \"1\" expected but found \"%d\"",
								arr.data[0]);
						CHECK_CMP_VALUE(
								arr.data[1], 2,
								"Wrong element at index \"0\", \"2\" expected but found \"%d\"",
								arr.data[2]);
						CHECK_CMP_VALUE(
								arr.data[2], 3,
								"Wrong element at index \"0\", \"3\" expected but found \"%d\"",
								arr.data[3]);
						CHECK_CMP_VALUE(
								arr.data[3], 4,
								"Wrong element at index \"0\", \"4\" expected but found \"%d\"",
								arr.data[3]);
					},
					"DynArray single-element insertion");
			TEST_CASE(
					{
						cbuild_da_clear(&arr);

						CHECK_CMP_VALUE(arr.size, 0,
														"Incorect element count in array: \"0\" expected "
														"but found \"%zu\"",
														arr.size);
						CHECK_CMP_VALUE(arr.capacity, 0,
														"Wrong array capacity alocated, expected \"0\" but "
														"found \"%zu\"",
														arr.capacity);
						CHECK_CMP_VALUE(arr.data, NULL,
														"Wrong array data pointer, expected \"NULL\" but "
														"found \"%p\"",
														arr.data);
					},
					"DynArray free");
			cbuild_da_clear(&arr);
			TEST_CASE(
					{
						int tmp[4];
						tmp[0] = 1;
						tmp[1] = 2;
						tmp[2] = 3;
						tmp[3] = 4;
						cbuild_da_append_arr(&arr, tmp, 4);
						CHECK_CMP_VALUE(arr.size, 4,
														"Incorect element count in array: \"4\" expected "
														"but found \"%zu\"",
														arr.size);
						CHECK_CMP_VALUE(arr.capacity, CBUILD_DA_INIT_CAPACITY,
														"Wrong array capacity alocated, expected \"%zu\" "
														"but found \"%zu\"",
														CBUILD_DA_INIT_CAPACITY, arr.capacity);
						CHECK_CMP_VALUE(
								arr.data[0], 1,
								"Wrong element at index \"0\", \"1\" expected but found \"%d\"",
								arr.data[0]);
						CHECK_CMP_VALUE(
								arr.data[1], 2,
								"Wrong element at index \"0\", \"2\" expected but found \"%d\"",
								arr.data[2]);
						CHECK_CMP_VALUE(
								arr.data[2], 3,
								"Wrong element at index \"0\", \"3\" expected but found \"%d\"",
								arr.data[3]);
						CHECK_CMP_VALUE(
								arr.data[3], 4,
								"Wrong element at index \"0\", \"4\" expected but found \"%d\"",
								arr.data[3]);
					},
					"DynArray insertion of C array");
			cbuild_da_clear(&arr);
			TEST_CASE(
					{
						cbuild_da_append_many(&arr, int, 1, 2, 3, 4);
						CHECK_CMP_VALUE(arr.size, 4,
														"Incorect element count in array: \"4\" expected "
														"but found \"%zu\"",
														arr.size);
						CHECK_CMP_VALUE(arr.capacity, CBUILD_DA_INIT_CAPACITY,
														"Wrong array capacity alocated, expected \"%zu\" "
														"but found \"%zu\"",
														CBUILD_DA_INIT_CAPACITY, arr.capacity);
						CHECK_CMP_VALUE(
								arr.data[0], 1,
								"Wrong element at index \"0\", \"1\" expected but found \"%d\"",
								arr.data[0]);
						CHECK_CMP_VALUE(
								arr.data[1], 2,
								"Wrong element at index \"0\", \"2\" expected but found \"%d\"",
								arr.data[2]);
						CHECK_CMP_VALUE(
								arr.data[2], 3,
								"Wrong element at index \"0\", \"3\" expected but found \"%d\"",
								arr.data[3]);
						CHECK_CMP_VALUE(
								arr.data[3], 4,
								"Wrong element at index \"0\", \"4\" expected but found \"%d\"",
								arr.data[3]);
					},
					"DynArray insertion of multiple values");
		},
		"DynArray datatype - dynamic resizable array")
