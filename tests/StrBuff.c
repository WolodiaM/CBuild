/**
 * @file StrBuff.c
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Tests for string buffer
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
#include "../src/StringBuffer.h"
#include "framework.h"
// Code
TEST_MAIN(
		{
			CBuildStrBuff str = { 0 };
			TEST_CASE(
					{
						cbuild_sb_append(&str, 'a');
						cbuild_sb_append(&str, 'b');
						cbuild_sb_append(&str, 'c');
						cbuild_sb_append(&str, 'd');
						CHECK_CMP_VALUE(
								str.size, 4,
								"Incorect element count in string buffer: \"4\" expected "
								"but found \"%zu\"",
								str.size);
						CHECK_CMP_VALUE(
								str.capacity, CBUILD_DA_INIT_CAPACITY,
								"Wrong string buffer capacity alocated, expected \"%zu\" "
								"but found \"%zu\"",
								CBUILD_DA_INIT_CAPACITY, str.capacity);
						CHECK_CMP_VALUE(
								str.data[0], 'a',
								"Wrong element at index \"0\", \'a\' expected but found \'%c\'",
								str.data[0]);
						CHECK_CMP_VALUE(
								str.data[1], 'b',
								"Wrong element at index \"0\", \'b\' expected but found \"%c\"",
								str.data[2]);
						CHECK_CMP_VALUE(
								str.data[2], 'c',
								"Wrong element at index \"0\", \'c\' expected but found \"%c\"",
								str.data[3]);
						CHECK_CMP_VALUE(
								str.data[3], 'd',
								"Wrong element at index \"0\", \'d\' expected but found \"%c\"",
								str.data[3]);
					},
					"StringBuffer single-character insertion");
			TEST_CASE(
					{
						cbuild_sb_clear(&str);

						CHECK_CMP_VALUE(str.size, 0,
														"Incorect element count in stray: \"0\" expected "
														"but found \"%zu\"",
														str.size);
						CHECK_CMP_VALUE(str.capacity, 0,
														"Wrong stray capacity alocated, expected \"0\" but "
														"found \"%zu\"",
														str.capacity);
						CHECK_CMP_VALUE(str.data, NULL,
														"Wrong stray data pointer, expected \"NULL\" but "
														"found \"%p\"",
														str.data);
					},
					"StringBuffer free");
			cbuild_sb_clear(&str);
			TEST_CASE(
					{
						char tmp[4];
						tmp[0] = 'a';
						tmp[1] = 'b';
						tmp[2] = 'c';
						tmp[3] = 'd';
						cbuild_sb_append_arr(&str, tmp, 4);
						CHECK_CMP_VALUE(str.size, 4,
														"Incorect element count in stray: \"4\" expected "
														"but found \"%zu\"",
														str.size);
						CHECK_CMP_VALUE(str.capacity, CBUILD_DA_INIT_CAPACITY,
														"Wrong stray capacity alocated, expected \"%zu\" "
														"but found \"%zu\"",
														CBUILD_DA_INIT_CAPACITY, str.capacity);
						CHECK_CMP_VALUE(
								str.data[0], 'a',
								"Wrong element at index \"0\", \'a\' expected but found \"%c\"",
								str.data[0]);
						CHECK_CMP_VALUE(
								str.data[1], 'b',
								"Wrong element at index \"0\", \'b\' expected but found \"%c\"",
								str.data[2]);
						CHECK_CMP_VALUE(
								str.data[2], 'c',
								"Wrong element at index \"0\", \'c\' expected but found \"%c\"",
								str.data[3]);
						CHECK_CMP_VALUE(
								str.data[3], 'd',
								"Wrong element at index \"0\", \'d\' expected but found \"%c\"",
								str.data[3]);
					},
					"StringBuffer insertion of C stray");
			cbuild_sb_clear(&str);
			TEST_CASE(
					{
						cbuild_sb_append_many(&str, 'a', 'b', 'c', 'd');
						CHECK_CMP_VALUE(str.size, 4,
														"Incorect element count in stray: \"4\" expected "
														"but found \"%zu\"",
														str.size);
						CHECK_CMP_VALUE(str.capacity, CBUILD_DA_INIT_CAPACITY,
														"Wrong stray capacity alocated, expected \"%zu\" "
														"but found \"%zu\"",
														CBUILD_DA_INIT_CAPACITY, str.capacity);
						CHECK_CMP_VALUE(
								str.data[0], 'a',
								"Wrong element at index \"0\", \'a\' expected but found \"%c\"",
								str.data[0]);
						CHECK_CMP_VALUE(
								str.data[1], 'b',
								"Wrong element at index \"0\", \'b\' expected but found \"%c\"",
								str.data[2]);
						CHECK_CMP_VALUE(
								str.data[2], 'c',
								"Wrong element at index \"0\", \'c\' expected but found \"%c\"",
								str.data[3]);
						CHECK_CMP_VALUE(
								str.data[3], 'd',
								"Wrong element at index \"0\", \'d\' expected but found \"%c\"",
								str.data[3]);
					},
					"StringBuffer insertion of multiple values");
			cbuild_sb_clear(&str);
			TEST_CASE(
					{
						const char* tmp = "abcd";
						cbuild_sb_append_cstr(&str, tmp);
						CHECK_CMP_VALUE(str.size, 4,
														"Incorect element count in stray: \"4\" expected "
														"but found \"%zu\"",
														str.size);
						CHECK_CMP_VALUE(str.capacity, CBUILD_DA_INIT_CAPACITY,
														"Wrong stray capacity alocated, expected \"%zu\" "
														"but found \"%zu\"",
														CBUILD_DA_INIT_CAPACITY, str.capacity);
						CHECK_CMP_VALUE(
								str.data[0], 'a',
								"Wrong element at index \"0\", \'a\' expected but found \"%c\"",
								str.data[0]);
						CHECK_CMP_VALUE(
								str.data[1], 'b',
								"Wrong element at index \"0\", \'b\' expected but found \"%c\"",
								str.data[2]);
						CHECK_CMP_VALUE(
								str.data[2], 'c',
								"Wrong element at index \"0\", \'c\' expected but found \"%c\"",
								str.data[3]);
						CHECK_CMP_VALUE(
								str.data[3], 'd',
								"Wrong element at index \"0\", \'d\' expected but found \"%c\"",
								str.data[3]);
					},
					"StringBuffer c-string insertion");
			TEST_CASE(
					{
						cbuild_sb_append_null(&str);
						CHECK_CMP_VALUE(str.size, 5,
														"Incorect element count in stray: \"5\" expected "
														"but found \"%zu\"",
														str.size);
						CHECK_CMP_VALUE(
								str.data[4], '\0',
								"Wrong element at index \"0\", \'\\0\' expected but "
								"found \"%c\"",
								str.data[4]);
						cbuild_sb_remove_null(&str);
						CHECK_CMP_VALUE(str.size, 4,
														"Incorect element count in stray: \"4\" expected "
														"but found \"%zu\"",
														str.size);
					},
					"StringBuffer null-insertion and removal");
			TEST_CASE(
					{
						CBuildStrBuff str0 = { 0 };
						CBuildStrBuff str1 = { 0 };
						cbuild_sb_append_cstr(&str0, "abcd");
						cbuild_sb_append_cstr(&str1, "abc");
						int ret = cbuild_sb_cmp(&str0, &str1);
						CHECK_CMP_VALUE(ret, 2,
														"Wrong string size comparison, expected return "
														"\"2\", actual: \"%d\"",
														ret);
						ret = cbuild_sb_cmp(&str1, &str0);
						CHECK_CMP_VALUE(
								ret, -2,
								"Wrong string size comparison, expected return \"-2\", "
								"actual: \"%d\"",
								ret);
						cbuild_sb_append(&str1, 'd');
						ret = cbuild_sb_cmp(&str1, &str0);
						CHECK_CMP_VALUE(ret, 0,
														"Wrong string comparison, expected return \"0\", "
														"actual: \"%d\"",
														ret);
						str0.data[3] = 'a';
						ret					 = cbuild_sb_cmp(&str0, &str1);
						CHECK_CMP_VALUE(ret, -1,
														"Wrong string comparison, expected return \"-1\", "
														"actual: \"%d\"",
														ret);
						ret = cbuild_sb_cmp(&str1, &str0);
						CHECK_CMP_VALUE(ret, 1,
														"Wrong string comparison, expected return \"1\", "
														"actual: \"%d\"",
														ret);
					},
					"StringBuffer compare function");
		},
		"StringBuffer datatype - dynamic string")
