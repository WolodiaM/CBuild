/**
 * @file DynArray.c
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Tests for dynamic array implementation
 * This test requires gcc extentions for '({ ... })'
 *
 * @date 2024-12-05
 * @copyright (C) 2024 WolodiaM
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
// Project includes
#include "../cbuild.h"
#include "framework.h"
#define CBUILD_IMPLEMENTATION
#include "../cbuild.h"
// Code
typedef struct da_int_t {
	int *data;
	size_t size;
	size_t capacity;
} da_int_t;
TEST_MAIN({
	da_int_t da = {0};
	TEST_CASE(
	{
		da_int_t da_alloc = {0};
		cbuild_da_resize(&da_alloc, 256);
		TEST_ASSERT_EQ(
		  da_alloc.capacity, 256,
		  "Wrong capacity was set on resize" TEST_EXPECT_MSG(zu),
		  (size_t)256, da_alloc.capacity);
		cbuild_da_clear(&da_alloc);
		TEST_ASSERT_EQ(da_alloc.capacity, 0,
		  "Wrong capcity was set on clear" TEST_EXPECT_MSG(zu),
		  (size_t)0, da_alloc.capacity);
		TEST_ASSERT_EQ(
		  da_alloc.data, NULL,
		  "Data pointer non-null after clear" TEST_EXPECT_MSG(p), NULL,
		  (void*)da_alloc.data);
		cbuild_da_append(&da_alloc, 1);
		cbuild_da_append(&da_alloc, 1);
		cbuild_da_append(&da_alloc, 1);
		TEST_ASSERT_EQ(
		  da_alloc.capacity, 4,
		  "Wrong capacity was set on resize" TEST_EXPECT_MSG(zu),
		  (size_t)4, da_alloc.capacity);
		cbuild_da_clear(&da_alloc);
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
	TEST_CASE(({
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
		cbuild_da_append_many(&da, 1, 2, 3);
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
		cbuild_da_resize(&da, 10);
		da.size = 2;
		cbuild_da_set(&da, 0, 1);
		TEST_ASSERT_EQ(da.data[0], 1,
		"Wrong element after set" TEST_EXPECT_MSG(d), 1,
		da.data[0]);
		cbuild_da_clear(&da);
	},
	"DynArray element set")
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
		cbuild_da_append_many(&da, 1, 2, 3, 4);
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
