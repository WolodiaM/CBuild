/**
 * @file DynArray.c
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Tests for dynamic array implementation
 * This test requires gcc extentions for '({ ... })'
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
cbuild_da_t(int, int);
cbuild_da_t_impl(int, int);
size_t talloc_last_alloc_size = 0;
void*  talloc_last_alloc_ptr  = NULL;
void*  talloc(size_t num) {
  talloc_last_alloc_size = num;
  void* ptr              = malloc(num);
  talloc_last_alloc_ptr  = ptr;
  return ptr;
}
void* tfree_last_free_ptr = NULL;
void  tfree(void* ptr) {
  tfree_last_free_ptr = ptr;
  free(ptr);
}
void* trealloc(void* ptr, size_t size) {
	talloc_last_alloc_size = size;
	tfree_last_free_ptr    = ptr;
	return realloc(ptr, size);
}
TEST_MAIN(
		{
			cbuild_da_int_t da = cbuild_da_int;
			TEST_CASE(
					{
						cbuild_da_int_t da_alloc = cbuild_da_int;
						da_alloc.malloc          = talloc;
						da_alloc.free            = tfree;
						da_alloc.realloc         = trealloc;
						cbuild_da_resize(&da_alloc, 256);
						TEST_ASSERT_EQ(
								talloc_last_alloc_size, 256ul * sizeof(int),
								"Wrong allocation value on resize" TEST_EXPECT_MSG(zu),
								256ul * sizeof(int), talloc_last_alloc_size);
						TEST_ASSERT_EQ(
								da_alloc.capacity, 256,
								"Wrong capacity was set on resize" TEST_EXPECT_MSG(zu),
								(size_t)256, da_alloc.capacity);
						cbuild_da_clear(&da_alloc);
						TEST_ASSERT_EQ(tfree_last_free_ptr, talloc_last_alloc_ptr,
		                       "Wrong pointer was sent to free" TEST_EXPECT_MSG(p),
		                       talloc_last_alloc_ptr, tfree_last_free_ptr);
						TEST_ASSERT_EQ(da_alloc.capacity, 0,
		                       "Wrong capcity was set on clear" TEST_EXPECT_MSG(zu),
		                       (size_t)0, da_alloc.capacity);
						cbuild_da_append(&da_alloc, 1);
						TEST_ASSERT_EQ(
								talloc_last_alloc_size, 2ul * sizeof(int),
								"Wrong alloc requested on append" TEST_EXPECT_MSG(zu),
								2ul * sizeof(int), talloc_last_alloc_size);
						cbuild_da_append(&da_alloc, 1);
						cbuild_da_append(&da_alloc, 1);
						TEST_ASSERT_EQ(
								talloc_last_alloc_size, 4ul * sizeof(int),
								"Wrong alloc requested on append" TEST_EXPECT_MSG(zu),
								4ul * sizeof(int), talloc_last_alloc_size);
					},
					"DynArray allocation");
			TEST_CASE(
					{
						cbuild_da_append(&da, 1);
						cbuild_da_append(&da, 2);
						TEST_ASSERT_EQ(
								da.size, 2,
								"Wrong element count after insertion" TEST_EXPECT_MSG(zu),
								(size_t)2, da.size);
						TEST_ASSERT_EQ(da.data[0], 1,
		                       "Wrong element at index 0" TEST_EXPECT_MSG(d), 1,
		                       da.data[0]);
						TEST_ASSERT_EQ(da.data[1], 2,
		                       "Wrong element at index 1" TEST_EXPECT_MSG(d), 2,
		                       da.data[1]);
						cbuild_da_clear(&da);
					},
					"DynArray single-element insertion");
			TEST_CASE(__extension__({
									int arr[] = { 1, 2, 3 };
									cbuild_da_append_arr(&da, arr, 2);
									TEST_ASSERT_EQ(
											da.size, 2,
											"Wrong element count after insertion" TEST_EXPECT_MSG(zu),
											(size_t)2, da.size);
									TEST_ASSERT_EQ(da.data[0], 1,
		                             "Wrong element at index 0" TEST_EXPECT_MSG(d),
		                             1, da.data[0]);
									TEST_ASSERT_EQ(da.data[1], 2,
		                             "Wrong element at index 1" TEST_EXPECT_MSG(d),
		                             2, da.data[1]);
									cbuild_da_clear(&da);
								}),
	              "DynArray array insertion");
			TEST_CASE(
					{
						cbuild_da_append_many(&da, int, 1, 2, 3);
						TEST_ASSERT_EQ(
								da.size, 3,
								"Wrong element count after insertion" TEST_EXPECT_MSG(zu),
								(size_t)3, da.size);
						TEST_ASSERT_EQ(da.data[0], 1,
		                       "Wrong element at index 0" TEST_EXPECT_MSG(d), 1,
		                       da.data[0]);
						TEST_ASSERT_EQ(da.data[1], 2,
		                       "Wrong element at index 1" TEST_EXPECT_MSG(d), 2,
		                       da.data[1]);
						TEST_ASSERT_EQ(da.data[2], 3,
		                       "Wrong element at index 2" TEST_EXPECT_MSG(d), 3,
		                       da.data[2]);
						cbuild_da_clear(&da);
					},
					"DynArray multi-element insertion");
			TEST_CASE(
					{
						cbuild_da_append(&da, 1);
						cbuild_da_append(&da, 2);
						TEST_ASSERT_EQ(*cbuild_da_get(&da, 0), 1,
		                       "Wrong element read at index 0" TEST_EXPECT_MSG(d),
		                       1, *cbuild_da_get(&da, 0));
						TEST_ASSERT_EQ(*cbuild_da_get(&da, 1), 2,
		                       "Wrong element read at index 1" TEST_EXPECT_MSG(d),
		                       2, *cbuild_da_get(&da, 1));
						cbuild_da_clear(&da);
					},
					"DynArray reading");
			TEST_CASE(
					{
						cbuild_da_append_many(&da, int, 1, 2, 3, 4);
						cbuild_da_remove(&da, 1);
						TEST_ASSERT_EQ(da.data[0], 1,
		                       "Wrong element at index 0" TEST_EXPECT_MSG(d), 1,
		                       da.data[0]);
						TEST_ASSERT_EQ(da.data[1], 3,
		                       "Wrong element at index 1" TEST_EXPECT_MSG(d), 3,
		                       da.data[1]);
						TEST_ASSERT_EQ(da.data[2], 4,
		                       "Wrong element at index 2" TEST_EXPECT_MSG(d), 4,
		                       da.data[2]);
						cbuild_da_clear(&da);
					},
					"DynArray remove");
		},
		"DynArray datatype - dynamic resizable array")
