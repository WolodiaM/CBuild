/**
 * @file StringBuilder.c
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Tests for StringBuilder datatype
 *
 * @date 2025-06-19
 * @copyright (C) 2025 WolodiaM
 * @license MIT
 *
 * Copyright (C) 2025 WolodiaM
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
#include "../src/StringBuilder.h"
#include "framework.h"
// Code
TEST_MAIN({
	cbuild_sb_t sb = {0};
	TEST_CASE(
	{
		cbuild_sb_t sb_alloc = {0};
		cbuild_sb_resize(&sb_alloc, 256);
		TEST_ASSERT_EQ(
		  sb_alloc.capacity, 256,
		  "Wrong capacity was set on resize" TEST_EXPECT_MSG(zu),
		  (size_t)256, sb_alloc.capacity);
		TEST_ASSERT_NEQ(sb_alloc.data, NULL, "%s",
		  "Data pointer is null after allocation.");
		cbuild_sb_clear(&sb_alloc);
		TEST_ASSERT_EQ(sb_alloc.capacity, 0,
		  "Wrong capcity was set on clear" TEST_EXPECT_MSG(zu),
		  (size_t)0, sb_alloc.capacity);
		TEST_ASSERT_EQ(
		  sb_alloc.data, NULL,
		  "Data pointer non-null after clear" TEST_EXPECT_MSG(p), NULL,
		  sb_alloc.data);
		cbuild_sb_append(&sb_alloc, 'a');
		cbuild_sb_append(&sb_alloc, 'a');
		cbuild_sb_append(&sb_alloc, 'a');
		cbuild_sb_clear(&sb_alloc);
	},
	"StringBuilder allocation");
	TEST_CASE(
	{
		cbuild_sb_append(&sb, 'a');
		cbuild_sb_append(&sb, 'b');
		TEST_ASSERT_EQ(
		sb.size, 2,
		"Wrong element count after insertion" TEST_EXPECT_MSG(zu),
		(size_t)2, sb.size);
		TEST_ASSERT_EQ(sb.data[0], 'a',
		"Wrong element at index 0" TEST_EXPECT_MSG(c), 'a',
		sb.data[0]);
		TEST_ASSERT_EQ(sb.data[1], 'b',
		"Wrong element at index 1" TEST_EXPECT_MSG(c), 'b',
		sb.data[1]);
		cbuild_sb_clear(&sb);
	},
	"StringBuilder single-element insertion");
	TEST_CASE(({
		char arr[] = { 'a', 'b', 'c' };
		cbuild_sb_append_arr(&sb, arr, 2);
		TEST_ASSERT_EQ(
		  sb.size, 2,
		  "Wrong element count after insertion" TEST_EXPECT_MSG(zu),
		  (size_t)2, sb.size);
		TEST_ASSERT_EQ(sb.data[0], 'a',
		  "Wrong element at index 0" TEST_EXPECT_MSG(c),
		  'a', sb.data[0]);
		TEST_ASSERT_EQ(sb.data[1], 'b',
		  "Wrong element at index 1" TEST_EXPECT_MSG(c),
		  'b', sb.data[1]);
		cbuild_sb_clear(&sb);
	}),
	"StringBuilder array insertion");
	TEST_CASE(
	{
		cbuild_sb_append_many(&sb, 'a', 'b', 'c');
		TEST_ASSERT_EQ(
		sb.size, 3,
		"Wrong element count after insertion" TEST_EXPECT_MSG(zu),
		(size_t)3, sb.size);
		TEST_ASSERT_EQ(sb.data[0], 'a',
		"Wrong element at index 0" TEST_EXPECT_MSG(c), 'a',
		sb.data[0]);
		TEST_ASSERT_EQ(sb.data[1], 'b',
		"Wrong element at index 1" TEST_EXPECT_MSG(c), 'b',
		sb.data[1]);
		TEST_ASSERT_EQ(sb.data[2], 'c',
		"Wrong element at index 2" TEST_EXPECT_MSG(c), 'c',
		sb.data[2]);
		cbuild_sb_clear(&sb);
	},
	"StringBuilder multi-element insertion");
	TEST_CASE(
	{
		cbuild_sb_append_cstr(&sb, "Hello, world");
		cbuild_sb_append(&sb, '\0');
		TEST_ASSERT_STREQ(
		sb.data, "Hello, world",
		"Wrong elements inserted as a string" TEST_EXPECT_MSG(s),
		"Hello, world", sb.data);
		cbuild_sb_clear(&sb);
	},
	"StringBuilder c-string insertion");
	TEST_CASE(
	{
		cbuild_sb_resize(&sb, 10);
		sb.size = 2;
		cbuild_sb_set(&sb, 0, 'a');
		TEST_ASSERT_EQ(sb.data[0], 'a',
		"Wrong element after set" TEST_EXPECT_MSG(c), 'a',
		sb.data[0]);
		cbuild_sb_clear(&sb);
	},
	"StringBuilder element set")
	TEST_CASE(
	{
		cbuild_sb_append(&sb, 'a');
		cbuild_sb_append(&sb, 'b');
		TEST_ASSERT_EQ(*cbuild_sb_get(&sb, 0), 'a',
		"Wrong element read at index 0" TEST_EXPECT_MSG(c),
		'a', *cbuild_sb_get(&sb, 0));
		TEST_ASSERT_EQ(*cbuild_sb_get(&sb, 1), 'b',
		"Wrong element read at index 1" TEST_EXPECT_MSG(c),
		'b', *cbuild_sb_get(&sb, 1));
		cbuild_sb_clear(&sb);
	},
	"StringBuilder reading");
	TEST_CASE(
	{
		cbuild_sb_append_many(&sb, 'a', 'b', 'c', 'd');
		cbuild_sb_remove(&sb, 1);
		TEST_ASSERT_EQ(sb.data[0], 'a',
		"Wrong element at index 0" TEST_EXPECT_MSG(c), 'a',
		sb.data[0]);
		TEST_ASSERT_EQ(sb.data[1], 'c',
		"Wrong element at index 1" TEST_EXPECT_MSG(c), 'c',
		sb.data[1]);
		TEST_ASSERT_EQ(sb.data[2], 'd',
		"Wrong element at index 2" TEST_EXPECT_MSG(c), 'd',
		sb.data[2]);
		cbuild_sb_clear(&sb);
	},
	"StringBuilder remove");
	TEST_CASE(
	{
		const char* str1 = "abc";
		cbuild_sb_t sb1  = {0};
		cbuild_sb_append_cstr(&sb1, str1);
		const char* str2 = "abc";
		cbuild_sb_t sb2  = {0};
		cbuild_sb_append_cstr(&sb2, str2);
		const char* str3 = "ABC";
		cbuild_sb_t sb3  = {0};
		cbuild_sb_append_cstr(&sb3, str3);
		const char* str4 = "ABC";
		cbuild_sb_t sb4  = {0};
		cbuild_sb_append_cstr(&sb4, str4);
		const char* str5 = "def";
		cbuild_sb_t sb5  = {0};
		cbuild_sb_append_cstr(&sb5, str5);
		const char* str6 = "defg";
		cbuild_sb_t sb6  = {0};
		cbuild_sb_append_cstr(&sb6, str6);
		printf("\tcbuild_sb_cmp\n");
		TEST_ASSERT_EQ(cbuild_sb_cmp(&sb1, &sb1), 0,
		  "Compare with self failed" TEST_EXPECT_MSG(d), 0,
		  cbuild_sb_cmp(&sb1, &sb2));
		TEST_ASSERT_EQ(
		  cbuild_sb_cmp(&sb1, &sb2), 0,
		  "Compare with same string view failed" TEST_EXPECT_MSG(d), 0,
		  cbuild_sb_cmp(&sb1, &sb2));
		TEST_ASSERT_EQ(cbuild_sb_cmp(&sb1, &sb4), 1,
		  "Compare with same string (different case) view "
		  "failed" TEST_EXPECT_MSG(d),
		  1, cbuild_sb_cmp(&sb1, &sb4));
		TEST_ASSERT_EQ(
		  cbuild_sb_cmp(&sb1, &sb5), -1,
		  "Wrong difference reported when c1 < c2" TEST_EXPECT_MSG(d), -1,
		  cbuild_sb_cmp(&sb1, &sb5));
		TEST_ASSERT_EQ(
		  cbuild_sb_cmp(&sb5, &sb1), 1,
		  "Wrong difference reported when c2 < c1" TEST_EXPECT_MSG(d), 1,
		  cbuild_sb_cmp(&sb5, &sb1));
		TEST_ASSERT_EQ(
		  cbuild_sb_cmp(&sb1, &sb6), -2,
		  "Wrong difference reported when size1 < size2" TEST_EXPECT_MSG(
		    d),
		  -2, cbuild_sb_cmp(&sb1, &sb6));
		TEST_ASSERT_EQ(
		  cbuild_sb_cmp(&sb6, &sb1), 2,
		  "Wrong difference reported when size1 < size2" TEST_EXPECT_MSG(
		    d),
		  2, cbuild_sb_cmp(&sb6, &sb1));
		printf("\tcbuild_sb_cmp_icase\n");
		TEST_ASSERT_EQ(cbuild_sb_cmp_icase(&sb5, &sb5), 0,
		  "Compare with self failed" TEST_EXPECT_MSG(d), 0,
		  cbuild_sb_cmp_icase(&sb5, &sb5));
		TEST_ASSERT_EQ(
		  cbuild_sb_cmp_icase(&sb1, &sb2), 0,
		  "Compare with same string view failed" TEST_EXPECT_MSG(d), 0,
		  cbuild_sb_cmp_icase(&sb1, &sb2));
		TEST_ASSERT_EQ(cbuild_sb_cmp_icase(&sb1, &sb3), 0,
		  "Compare with same string view (different case) "
		  "failed" TEST_EXPECT_MSG(d),
		  0, cbuild_sb_cmp_icase(&sb1, &sb3));
		TEST_ASSERT_EQ(
		  cbuild_sb_cmp_icase(&sb1, &sb5), -1,
		  "Wrong difference reported when c1 < c2" TEST_EXPECT_MSG(d), -1,
		  cbuild_sb_cmp_icase(&sb1, &sb5));
		TEST_ASSERT_EQ(
		  cbuild_sb_cmp_icase(&sb5, &sb1), 1,
		  "Wrong difference reported when c2 < c1" TEST_EXPECT_MSG(d), 1,
		  cbuild_sb_cmp_icase(&sb5, &sb1));
		TEST_ASSERT_EQ(
		  cbuild_sb_cmp_icase(&sb1, &sb6), -2,
		  "Wrong difference reported when size1 < size2" TEST_EXPECT_MSG(
		    d),
		  -2, cbuild_sb_cmp_icase(&sb1, &sb6));
		TEST_ASSERT_EQ(
		  cbuild_sb_cmp_icase(&sb6, &sb1), 2,
		  "Wrong difference reported when size1 < size2" TEST_EXPECT_MSG(
		    d),
		  2, cbuild_sb_cmp_icase(&sb6, &sb1));
		cbuild_sb_clear(&sb1);
		cbuild_sb_clear(&sb2);
		cbuild_sb_clear(&sb3);
		cbuild_sb_clear(&sb4);
		cbuild_sb_clear(&sb5);
		cbuild_sb_clear(&sb6);
	},
	"StringBuilder comparisons");
	TEST_CASE(
	{
		const char* str = "abc";
		cbuild_sv_t sv  = cbuild_sv_from_cstr(str);
		sv.size++;
		cbuild_sb_append_sv(&sb, sv);
		TEST_ASSERT_EQ(
		  sb.size, sv.size,
		  "Wrong number of elements appended" TEST_EXPECT_MSG(zu),
		  sb.size, sv.size);
		// Could be done only because it is baked by C-string and size
		// included '\0'
		TEST_ASSERT_STREQ(sb.data, sv.data,
		  "Wrong string appended" TEST_EXPECT_MSG(s),
		  sb.data, sv.data)
		cbuild_sv_t sv_out = cbuild_sb_to_sv(&sb);
		TEST_ASSERT_EQ(sv.size, sv_out.size,
		  "Wrong number of elements after transformations "
		  "(sv->sb->sv)" TEST_EXPECT_MSG(zu),
		  sv.size, sv_out.size)
		TEST_ASSERT_STREQ(sv.data, sv_out.data,
		  "Wrong elements after transformations "
		  "(sv->sb->sv)" TEST_EXPECT_MSG(s),
		  sv.data, sv_out.data)
		cbuild_sb_clear(&sb);
		sb = cbuild_sv_to_sb(sv);
		TEST_ASSERT_EQ(sb.size, sv.size,
		  "Wrong number of elements set" TEST_EXPECT_MSG(zu),
		  sb.size, sv.size);
		// Could be done only because it is baked by C-string and size
		// included '\0'
		TEST_ASSERT_STREQ(sb.data, sv.data,
		  "Wrong string set" TEST_EXPECT_MSG(s), sb.data,
		  sv.data)
		cbuild_sb_clear(&sb);
	},
	"StringView inter-operation");
	TEST_CASE(
	{
		cbuild_sb_appendf(&sb, "%d: %s", 2, "Hello, world!");
		cbuild_sb_append_null(&sb);
		TEST_ASSERT_EQ(sb.size, 17,
		"Wrong size after all appends" TEST_EXPECT_MSG(zu),
		(size_t)17, sb.size);
		TEST_ASSERT_STREQ(
		sb.data, "2: Hello, world!",
		"Wrong elements after all appends" TEST_EXPECT_MSG(s),
		"2: Hello, world!", sb.data);
		cbuild_sb_clear(&sb);
		char long_fmt[1024];
		memset(long_fmt, 'x', 1023);
		long_fmt[1023] = '\0';
		cbuild_sb_appendf(&sb, "%s", long_fmt);
		cbuild_sb_append_null(&sb);
		TEST_ASSERT_EQ(sb.size, 1024,
		"Long string append failed" TEST_EXPECT_MSG(zu),
		(size_t)1024, sb.size);
		cbuild_sb_clear(&sb);
	},
	"StringBuilder format insertion");
	TEST_CASE({
		cbuild_sb_t sb = {0};
		const char* test = "aф€😀";
		cbuild_sb_append_utf8(&sb, 'a');
		cbuild_sb_append_utf8(&sb, 1092);
		cbuild_sb_append_utf8(&sb, 8364);
		cbuild_sb_append_utf8(&sb, 128512);
		cbuild_sb_append_null(&sb);
		TEST_ASSERT_EQ(sb.size, 11,
		  "Wrong size after appends" TEST_EXPECT_MSG(zu), 11ul, sb.size);
		TEST_ASSERT_MEMEQ(sb.data, test, 10,
		  "Error while appending utf8 characters to a string builder"
		  TEST_EXPECT_MSG(s), test, sb.data);
		cbuild_sb_t copy = {0};
		cbuild_sb_append_cstr(&copy, test);
		cbuild_sb_append_null(&copy);
		cbuild_sb_t different = {0};
		cbuild_sb_append_cstr(&different, "abc");
		cbuild_sb_append_null(&different);
		TEST_ASSERT(cbuild_sb_utf8cmp(&sb, &copy) == 0,
		  "%s", "Same utf8 string reported as different!");
		TEST_ASSERT(cbuild_sb_utf8cmp(&sb, &different) != 0,
		  "%s", "Different utf8 string reported as same!");
		size_t len = cbuild_sb_utf8len(&sb);
		TEST_ASSERT_EQ(len, 5,
		  "Wrong codepoint count calculated" TEST_EXPECT_MSG(zu), 5ul, len);
		cbuild_sb_clear(&sb);
		cbuild_sb_clear(&copy);
		cbuild_sb_clear(&different);
	}, "UTF8 operations");
},
"StringBuilder datatype - dynamic re-sizable length-based string")
