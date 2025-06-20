/**
 * @file arrayt.c
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Tests for an array library
 *
 * @date 2025-02-17
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
// Library includes
#include "../CBTest/CBTest.h"
#include "../CBTest/assert.h"
#include <stdlib.h>
// Project includes
#define DA_INIT_CAPACITY 2ul
#include "array.h"
/* code */
// da_t
da_t(int, int);
// da_t_impl
da_t_impl(int, int);
/* code */
size_t talloc_last_alloc_size = 0;
void*	 talloc_last_alloc_ptr	= NULL;
void*	 talloc(size_t num) {
	 talloc_last_alloc_size = num;
	 void* ptr							= malloc(num);
	 talloc_last_alloc_ptr	= ptr;
	 return ptr;
}
void* tfree_last_free_ptr = NULL;
void	tfree(void* ptr) {
	 tfree_last_free_ptr = ptr;
	 free(ptr);
}
void* trealloc(void* ptr, size_t size) {
	talloc_last_alloc_size = size;
	tfree_last_free_ptr		 = ptr;
	return realloc(ptr, size);
}
TEST_FUNC(tda_alloc, "Cannot allocate da.", {
	da_int_t da = da_int;
	da.malloc		= talloc;
	da.free			= tfree;
	da.realloc	= trealloc;
	da_resize(&da, 256);
	TEST_ASSERT_EQ(talloc_last_alloc_size, 256ul * sizeof(int),
								 "Wrong allocation value on resize.\n");
	TEST_ASSERT_EQ(da.capacity, 256, "Wrong capacity was set on resize.");
	da_clear(&da);
	TEST_ASSERT_EQ(tfree_last_free_ptr, talloc_last_alloc_ptr,
								 "Wrong pointer was sent to free.\n");
	TEST_ASSERT_EQ(da.capacity, 0, "Wrong capcity was set on clear.");
	da_append(&da, 1);
	TEST_ASSERT_EQ(talloc_last_alloc_size, 2ul * sizeof(int),
								 "Wrong alloc requested on append");
	da_append(&da, 1);
	da_append(&da, 1);
	TEST_ASSERT_EQ(talloc_last_alloc_size, 4ul * sizeof(int),
								 "Wrong alloc requested on append");
});
TEST_FUNC(tda_append, "Cannot append to a da.", {
	da_int_t da = da_int;
	l->infomsg("Single-element insertions");
	da_append(&da, 1);
	da_append(&da, 2);
	TEST_ASSERT_EQ(da.size, 2, "Wrong element count after insertion.");
	TEST_ASSERT_EQ(da.data[0], 1, "Wrong element at index 0.");
	TEST_ASSERT_EQ(da.data[1], 2, "Wrong element at index 1.");
	da_clear(&da);
	l->infomsg("Array insertions");
	int arr[] = { 1, 2, 3 };
	da_append_arr(&da, arr, 2);
	TEST_ASSERT_EQ(da.size, 2, "Wrong element count after insertion.");
	TEST_ASSERT_EQ(da.data[0], 1, "Wrong element at index 0.");
	TEST_ASSERT_EQ(da.data[1], 2, "Wrong element at index 1.");
	da_append_arr(&da, arr, 2);
	TEST_ASSERT_EQ(da.size, 4, "Wrong element count after insertion.");
	TEST_ASSERT_EQ(da.data[2], 1, "Wrong element at index 2.");
	TEST_ASSERT_EQ(da.data[3], 2, "Wrong element at index 3.");
	da_clear(&da);
	l->infomsg("Multi-element insertions");
	da_append_many(&da, int, 1, 2, 3);
	TEST_ASSERT_EQ(da.size, 3, "Wrong element count after insertion.");
	TEST_ASSERT_EQ(da.data[0], 1, "Wrong element at index 0.");
	TEST_ASSERT_EQ(da.data[1], 2, "Wrong element at index 1.");
	TEST_ASSERT_EQ(da.data[2], 3, "Wrong element at index 2.");
	da_clear(&da);
});
TEST_FUNC(tda_read, "Cannot read from a da.", {
	da_int_t da = da_int;
	da_append(&da, 1);
	da_append(&da, 2);
	TEST_ASSERT_EQ(*da_get(&da, 0), 1, "Wrong element read at index 0.");
	TEST_ASSERT_EQ(*da_get(&da, 1), 2, "Wrong element read at index 1.");
	da_clear(&da);
});
TEST_FUNC(tda_set, "Cannot write into a da.", {
	da_int_t da = da_int;
	da_resize(&da, 256ul);
	da_append(&da, 0);
	da_append(&da, 1);
	da_set(&da, 0, 1);
	da_set(&da, 1, 2);
	TEST_ASSERT_EQ(*da_get(&da, 0), 1, "Wrong element read at index 0.");
	TEST_ASSERT_EQ(*da_get(&da, 1), 2, "Wrong element read at index 1.");
	TEST_ASSERT_EQ(da.size, 2, "Wrong size after set.");
	da_clear(&da);
});
TEST_FUNC(tda_remove, "Cannot remove elements from a DA.", {
	da_int_t da = da_int;
	da_append_many(&da, int, 1, 2, 3, 4);
	da_remove(&da, 1);
	TEST_ASSERT_EQ(da.data[0], 1, "Wrong element at index 0.");
	TEST_ASSERT_EQ(da.data[1], 3, "Wrong element at index 1.");
	TEST_ASSERT_EQ(da.data[2], 4, "Wrong element at index 2.");
	da_clear(&da);
});
void test_main(void) {
	test_register_group("DA tests", "Tests for dynamic array data structure.",
											"DA datatype is broken.");
	test_register(tda_alloc, NULL,
								"Check for allocation sequence on resize and clean.",
								"Allocating", "DA tests");
	test_register(tda_append, NULL, "Check for appending into an array.",
								"Appending", "DA tests");
	test_register(tda_read, NULL, "Check for reading from an array.", "Reading",
								"DA tests");
	test_register(tda_set, NULL, "Check for writing into an array.", "Writing",
								"DA tests");
	test_register(tda_remove, NULL, "Check for removing elements from an array",
								"Removing", "DA tests");
}
