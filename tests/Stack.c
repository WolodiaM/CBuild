/**
 * @file Stack.c
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Tests for a stack library
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
// Project includes
#include "../src/Stack.h"
#include "framework.h"
/* code */
typedef struct stack_int_t {
	int *data;
	size_t ptr;
	size_t capacity;
} stack_int_t;
TEST_MAIN({
	TEST_CASE(
	{
		stack_int_t stack = {0};
		cbuild_stack_resize(&stack, 256);
		TEST_ASSERT_NEQ(
		  stack.data, NULL, "%s",
		  "Data pointer NULL after resize");
		TEST_ASSERT_EQ(
		  stack.capacity, 256,
		  "Wrong capacity was set on resize" TEST_EXPECT_MSG(zu), 256ul,
		  stack.capacity);
		TEST_ASSERT_NEQ(stack.data, NULL, "%s",
		  "Data is set to NULL after allocation.");
		cbuild_stack_clear(&stack);
		TEST_ASSERT_EQ(stack.data, NULL, "%s",
		  "Data pointer non-null after resize");
		TEST_ASSERT_EQ(stack.capacity, 0,
		  "Wrong capcity was set on clear" TEST_EXPECT_MSG(zu),
		  0ul, stack.capacity);
		TEST_ASSERT_EQ(stack.ptr, 0,
		  "Ptr non zero after clear" TEST_EXPECT_MSG(zu), 0ul,
		  stack.ptr);
	},
	"Allocation sequence");
	TEST_CASE(
	{
		stack_int_t stack = {0};
		cbuild_stack_push(&stack, 1);
		cbuild_stack_push(&stack, 2);
		TEST_ASSERT_EQ(
		  stack.ptr, 2,
		  "Wrong element count after insertion" TEST_EXPECT_MSG(zu), 2ul,
		  stack.ptr);
		TEST_ASSERT_EQ(stack.data[0], 1,
		  "Wrong element at index 0" TEST_EXPECT_MSG(d), 1,
		  stack.data[0]);
		TEST_ASSERT_EQ(stack.data[1], 2,
		  "Wrong element at index 1" TEST_EXPECT_MSG(d), 2,
		  stack.data[1]);
		cbuild_stack_clear(&stack);
	},
	"Pushing");
	// TEST_CASE(
	// {
	// 	stack_int_t stack = {0};
	// 	cbuild_stack_push(&stack, 1);
	// 	cbuild_stack_push(&stack, 2);
	// 	int val;
	// 	val = cbuild_stack_pop(&stack);
	// 	TEST_ASSERT_EQ(val, 2,
	// 	  "Wrong element read at index 1" TEST_EXPECT_MSG(d),
	// 	  2, val);
	// 	val = cbuild_stack_pop(&stack);
	// 	TEST_ASSERT_EQ(val, 1,
	// 	  "Wrong element read at index 0" TEST_EXPECT_MSG(d),
	// 	  1, val);
	// 	cbuild_stack_clear(&stack);
	// },
	// "Popping");
},
"Stack datatype")
