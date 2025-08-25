/**
 * @file StrView.c
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Tests for string view
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
#include "../src/StringView.h"
#include "framework.h"
// Code
bool tdelim1(const cbuild_sv_t* sv, size_t idx, void* args) {
	CBUILD_UNUSED(idx);
	CBUILD_UNUSED(args);
	if(idx > 4 && sv->data[idx] == '|') {
		return true;
	}
	return false;
}
bool tdelim2(const cbuild_sv_t* sv, size_t idx, void* args) {
	CBUILD_UNUSED(sv);
	CBUILD_UNUSED(idx);
	CBUILD_UNUSED(args);
	return false;
}
bool tdelim3(const cbuild_sv_t* sv, void* arg) {
	cbuild_sv_t temp = *sv;
	uint32_t cp = cbuild_sv_chop_utf8(&temp);
	*(uint32_t*)arg = cp;
	return cp == 1092;
}
TEST_MAIN({
	TEST_CASE(
	{
		const char* str = "Hello, world!";
		cbuild_sv_t sv1 = cbuild_sv_from_parts(str, strlen(str));
		cbuild_sv_t sv2 = cbuild_sv_from_cstr(str);
		TEST_ASSERT_EQ(
		  sv1.data, str,
		  "Wrong base pointer for cbuild_sv_from_parts" TEST_EXPECT_MSG(
		    p),
		  sv1.data, str);
		TEST_ASSERT_EQ(
		  sv1.size, strlen(str),
		  "Wrong lengths for cbuild_sv_from_parts" TEST_EXPECT_MSG(zu),
		  sv1.size, strlen(str));
		TEST_ASSERT_EQ(
		  sv2.data, str,
		  "Wrong base pointer for cbuild_sv_from_cstr" TEST_EXPECT_MSG(p),
		  sv2.data, str);
		TEST_ASSERT_EQ(
		  sv2.size, strlen(str),
		  "Wrong lengths for cbuild_sv_from_cstr" TEST_EXPECT_MSG(zu),
		  sv2.size, strlen(str));
	},
	"StringView creation");
	TEST_CASE(
	{
		const char* str       = "Hello, world!";
		cbuild_sv_t sv        = cbuild_sv_from_cstr(str);
		char        fmt1[100] = { 0 };
		sprintf(fmt1, "%s", str);
		char fmt2[100] = { 0 };
		sprintf(fmt2, CBuildSVFmt, CBuildSVArg(sv));
		TEST_ASSERT_STREQ(
		  fmt1, fmt2,
		  "Wrong formatting of string view" TEST_EXPECT_MSG(s),
		  "Hello, world!", sv.data);
	},
	"StringView printing");
	TEST_CASE(
	{
		const char* str1 = "   \t\t\tabc";
		cbuild_sv_t sv1  = cbuild_sv_from_cstr(str1);
		cbuild_sv_t sv2  = cbuild_sv_from_cstr(str1);
		cbuild_sv_t sv3  = cbuild_sv_from_cstr(str1);
		const char* str2 = "abc \t \t \t";
		cbuild_sv_t sv4  = cbuild_sv_from_cstr(str2);
		cbuild_sv_t sv5  = cbuild_sv_from_cstr(str2);
		cbuild_sv_t sv6  = cbuild_sv_from_cstr(str2);
		const char* str3 = " \t abc\t \t";
		cbuild_sv_t sv7  = cbuild_sv_from_cstr(str3);
		cbuild_sv_t sv8  = cbuild_sv_from_cstr(str3);
		cbuild_sv_t sv9  = cbuild_sv_from_cstr(str3);
		const char* str4 = "    \t\t    ";
		cbuild_sv_t sv10 = cbuild_sv_from_cstr(str4);
		cbuild_sv_t sv11 = cbuild_sv_from_cstr(str4);
		cbuild_sv_t sv12 = cbuild_sv_from_cstr(str4);
		printf("\tcbuild_sv_trim_left\n");
		TEST_ASSERT_EQ(
		  cbuild_sv_trim_left(&sv1), 6,
		  "Wrong number of characters trimmed from str1" TEST_EXPECT_MSG(
		    zu),
		  (size_t)6, cbuild_sv_trim_left(&sv1));
		char fmt1[20] = { 0 };
		sprintf(fmt1, CBuildSVFmt, CBuildSVArg(sv1));
		TEST_ASSERT_STREQ(
		  fmt1, "abc",
		  "Wrong chacters trimmed from str1" TEST_EXPECT_MSG(s), "abc",
		  fmt1);
		TEST_ASSERT_EQ(
		  cbuild_sv_trim_left(&sv4), 0,
		  "Wrong number of characters trimmed from str2" TEST_EXPECT_MSG(
		    zu),
		  (size_t)0, cbuild_sv_trim_left(&sv4));
		char fmt4[20] = { 0 };
		sprintf(fmt4, CBuildSVFmt, CBuildSVArg(sv4));
		TEST_ASSERT_STREQ(
		  fmt4, "abc \t \t \t",
		  "Wrong chacters trimmed from str2" TEST_EXPECT_MSG(s),
		  "abc \t \t \t", fmt4);
		TEST_ASSERT_EQ(
		  cbuild_sv_trim_left(&sv7), 3,
		  "Wrong number of characters trimmed from str3" TEST_EXPECT_MSG(
		    zu),
		  (size_t)3, cbuild_sv_trim_left(&sv7));
		char fmt7[20] = { 0 };
		sprintf(fmt7, CBuildSVFmt, CBuildSVArg(sv7));
		TEST_ASSERT_STREQ(
		  fmt7, "abc\t \t",
		  "Wrong chacters trimmed from str3" TEST_EXPECT_MSG(s),
		  "abc\t \t", fmt7);
		TEST_ASSERT_EQ(
		  cbuild_sv_trim_left(&sv10), 10,
		  "Wrong number of characters trimmed from str4" TEST_EXPECT_MSG(
		    zu),
		  (size_t)10, cbuild_sv_trim_left(&sv10));
		char fmt10[20] = { 0 };
		sprintf(fmt10, CBuildSVFmt, CBuildSVArg(sv10));
		TEST_ASSERT_STREQ(
		  fmt10, "",
		  "Wrong chacters trimmed from str4" TEST_EXPECT_MSG(s), "",
		  fmt10);
		printf("\tcbuild_sv_trim_right\n");
		TEST_ASSERT_EQ(
		  cbuild_sv_trim_right(&sv2), 0,
		  "Wrong number of characters trimmed from str1" TEST_EXPECT_MSG(
		    zu),
		  (size_t)0, cbuild_sv_trim_right(&sv2));
		char fmt2[20] = { 0 };
		sprintf(fmt2, CBuildSVFmt, CBuildSVArg(sv2));
		TEST_ASSERT_STREQ(
		  fmt2, "   \t\t\tabc",
		  "Wrong chacters trimmed from str1" TEST_EXPECT_MSG(s),
		  "   \t\t\tabc", fmt2);
		TEST_ASSERT_EQ(
		  cbuild_sv_trim_right(&sv5), 6,
		  "Wrong number of characters trimmed from str2" TEST_EXPECT_MSG(
		    zu),
		  (size_t)6, cbuild_sv_trim_right(&sv5));
		char fmt5[20] = { 0 };
		sprintf(fmt5, CBuildSVFmt, CBuildSVArg(sv5));
		TEST_ASSERT_STREQ(
		  fmt5, "abc",
		  "Wrong chacters trimmed from str2" TEST_EXPECT_MSG(s), "abc",
		  fmt5);
		TEST_ASSERT_EQ(
		  cbuild_sv_trim_right(&sv8), 3,
		  "Wrong number of characters trimmed from str3" TEST_EXPECT_MSG(
		    zu),
		  (size_t)3, cbuild_sv_trim_right(&sv8));
		char fmt8[20] = { 0 };
		sprintf(fmt8, CBuildSVFmt, CBuildSVArg(sv8));
		TEST_ASSERT_STREQ(
		  fmt8, " \t abc",
		  "Wrong chacters trimmed from str3" TEST_EXPECT_MSG(s),
		  " \t abc", fmt8);
		TEST_ASSERT_EQ(
		  cbuild_sv_trim_right(&sv11), 10,
		  "Wrong number of characters trimmed from str4" TEST_EXPECT_MSG(
		    zu),
		  (size_t)10, cbuild_sv_trim_right(&sv11));
		char fmt11[20] = { 0 };
		sprintf(fmt11, CBuildSVFmt, CBuildSVArg(sv11));
		TEST_ASSERT_STREQ(
		  fmt11, "",
		  "Wrong chacters trimmed from str4" TEST_EXPECT_MSG(s), "",
		  fmt11);
		printf("\tcbuild_sv_trim\n");
		TEST_ASSERT_EQ(
		  cbuild_sv_trim(&sv3), 6,
		  "Wrong number of characters trimmed from str1" TEST_EXPECT_MSG(
		    zu),
		  (size_t)6, cbuild_sv_trim(&sv3));
		char fmt3[20] = { 0 };
		sprintf(fmt3, CBuildSVFmt, CBuildSVArg(sv3));
		TEST_ASSERT_STREQ(
		  fmt3, "abc",
		  "Wrong chacters trimmed from str1" TEST_EXPECT_MSG(s), "abc",
		  fmt3);
		TEST_ASSERT_EQ(
		  cbuild_sv_trim(&sv6), 6,
		  "Wrong number of characters trimmed from str2" TEST_EXPECT_MSG(
		    zu),
		  (size_t)6, cbuild_sv_trim(&sv6));
		char fmt6[20] = { 0 };
		sprintf(fmt6, CBuildSVFmt, CBuildSVArg(sv6));
		TEST_ASSERT_STREQ(
		  fmt6, "abc",
		  "Wrong chacters trimmed from str2" TEST_EXPECT_MSG(s), "abc",
		  fmt6);
		TEST_ASSERT_EQ(
		  cbuild_sv_trim(&sv9), 6,
		  "Wrong number of characters trimmed from str3" TEST_EXPECT_MSG(
		    zu),
		  (size_t)6, cbuild_sv_trim(&sv9));
		char fmt9[20] = { 0 };
		sprintf(fmt9, CBuildSVFmt, CBuildSVArg(sv9));
		TEST_ASSERT_STREQ(
		  fmt9, "abc",
		  "Wrong chacters trimmed from str3" TEST_EXPECT_MSG(s), "abc",
		  fmt9);
		TEST_ASSERT_EQ(
		  cbuild_sv_trim(&sv12), 10,
		  "Wrong number of characters trimmed from str4" TEST_EXPECT_MSG(
		    zu),
		  (size_t)10, cbuild_sv_trim(&sv12));
		char fmt12[20] = { 0 };
		sprintf(fmt12, CBuildSVFmt, CBuildSVArg(sv12));
		TEST_ASSERT_STREQ(
		  fmt12, "",
		  "Wrong chacters trimmed from str4" TEST_EXPECT_MSG(s), "",
		  fmt12);
	},
	"Trimming whitespaces from a StringView");
	TEST_CASE(
	{
		const char* str1 = "abc";
		cbuild_sv_t sv1  = cbuild_sv_from_cstr(str1);
		const char* str2 = "abc";
		cbuild_sv_t sv2  = cbuild_sv_from_cstr(str2);
		const char* str3 = "ABC";
		cbuild_sv_t sv3  = cbuild_sv_from_cstr(str3);
		const char* str4 = "ABC";
		cbuild_sv_t sv4  = cbuild_sv_from_cstr(str4);
		const char* str5 = "def";
		cbuild_sv_t sv5  = cbuild_sv_from_cstr(str5);
		const char* str6 = "defg";
		cbuild_sv_t sv6  = cbuild_sv_from_cstr(str6);
		printf("\tcbuild_sv_cmp\n");
		TEST_ASSERT_EQ(cbuild_sv_cmp(sv1, sv1), 0,
		  "Compare with self failed" TEST_EXPECT_MSG(d), 0,
		  cbuild_sv_cmp(sv1, sv1));
		TEST_ASSERT_EQ(
		  cbuild_sv_cmp(sv1, sv2), 0,
		  "Compare with same string view failed" TEST_EXPECT_MSG(d), 0,
		  cbuild_sv_cmp(sv1, sv2));
		TEST_ASSERT_EQ(cbuild_sv_cmp(sv1, sv4), 1,
		  "Compare with same string (different case) view "
		  "failed" TEST_EXPECT_MSG(d),
		  1, cbuild_sv_cmp(sv1, sv4));
		TEST_ASSERT_EQ(
		  cbuild_sv_cmp(sv1, sv5), -1,
		  "Wrong difference reported when c1 < c2" TEST_EXPECT_MSG(d), -1,
		  cbuild_sv_cmp(sv1, sv5));
		TEST_ASSERT_EQ(
		  cbuild_sv_cmp(sv5, sv1), 1,
		  "Wrong difference reported when c2 < c1" TEST_EXPECT_MSG(d), 1,
		  cbuild_sv_cmp(sv5, sv1));
		TEST_ASSERT_EQ(
		  cbuild_sv_cmp(sv1, sv6), -2,
		  "Wrong difference reported when size1 < size2" TEST_EXPECT_MSG(
		    d),
		  -2, cbuild_sv_cmp(sv1, sv6));
		TEST_ASSERT_EQ(
		  cbuild_sv_cmp(sv6, sv1), 2,
		  "Wrong difference reported when size1 < size2" TEST_EXPECT_MSG(
		    d),
		  2, cbuild_sv_cmp(sv6, sv1));
		printf("\tcbuild_sv_cmp_icase\n");
		TEST_ASSERT_EQ(cbuild_sv_cmp_icase(sv5, sv5), 0,
		  "Compare with self failed" TEST_EXPECT_MSG(d), 0,
		  cbuild_sv_cmp_icase(sv5, sv5));
		TEST_ASSERT_EQ(
		  cbuild_sv_cmp_icase(sv1, sv2), 0,
		  "Compare with same string view failed" TEST_EXPECT_MSG(d), 0,
		  cbuild_sv_cmp_icase(sv1, sv2));
		TEST_ASSERT_EQ(cbuild_sv_cmp_icase(sv1, sv3), 0,
		  "Compare with same string view (different case) "
		  "failed" TEST_EXPECT_MSG(d),
		  0, cbuild_sv_cmp_icase(sv1, sv3));
		TEST_ASSERT_EQ(
		  cbuild_sv_cmp_icase(sv1, sv5), -1,
		  "Wrong difference reported when c1 < c2" TEST_EXPECT_MSG(d), -1,
		  cbuild_sv_cmp_icase(sv1, sv5));
		TEST_ASSERT_EQ(
		  cbuild_sv_cmp_icase(sv5, sv1), 1,
		  "Wrong difference reported when c2 < c1" TEST_EXPECT_MSG(d), 1,
		  cbuild_sv_cmp_icase(sv5, sv1));
		TEST_ASSERT_EQ(
		  cbuild_sv_cmp_icase(sv1, sv6), -2,
		  "Wrong difference reported when size1 < size2" TEST_EXPECT_MSG(
		    d),
		  -2, cbuild_sv_cmp_icase(sv1, sv6));
		TEST_ASSERT_EQ(
		  cbuild_sv_cmp_icase(sv6, sv1), 2,
		  "Wrong difference reported when size1 < size2" TEST_EXPECT_MSG(
		    d),
		  2, cbuild_sv_cmp_icase(sv6, sv1));
	},
	"StringView comparisons");
	TEST_CASE(
	{
		const char* str1 = "abc|def|-|ghj";
		printf("\tcbuild_sv_chop\n");
		cbuild_sv_t sv1 = cbuild_sv_from_cstr(str1);
		cbuild_sv_t sv2 = cbuild_sv_chop(&sv1, 3);
		cbuild_sv_t sv3 = cbuild_sv_from_cstr(str1);
		cbuild_sv_t sv4 = cbuild_sv_chop(&sv3, 40);
		TEST_ASSERT_EQ(
		  cbuild_sv_cmp(sv1, cbuild_sv_from_cstr("|def|-|ghj")), 0,
		  "Wrong part left after chopping" TEST_EXPECT_RMSG(CBuildSVFmt),
		  CBuildSVArg(cbuild_sv_from_cstr("|def|-|ghj")),
		  CBuildSVArg(sv1));
		TEST_ASSERT_EQ(cbuild_sv_cmp(sv2, cbuild_sv_from_cstr("abc")), 0,
		  "Wrong part chopped" TEST_EXPECT_RMSG(CBuildSVFmt),
		  CBuildSVArg(cbuild_sv_from_cstr("abc")),
		  CBuildSVArg(sv2));
		TEST_ASSERT_EQ(
		  cbuild_sv_cmp(sv3, cbuild_sv_from_cstr("")), 0,
		  "Wrong part left after chopping (on overflow)" TEST_EXPECT_RMSG(
		  CBuildSVFmt),
		  CBuildSVArg(cbuild_sv_from_cstr("")), CBuildSVArg(sv3));
		TEST_ASSERT_EQ(cbuild_sv_cmp(sv4, cbuild_sv_from_cstr(str1)), 0,
		  "Wrong part chopped (on overflow)" TEST_EXPECT_RMSG(
		  CBuildSVFmt),
		  CBuildSVArg(cbuild_sv_from_cstr(str1)),
		  CBuildSVArg(sv4));
		printf("\tcbuild_sv_chop_by_func\n");
		cbuild_sv_t sv5 = cbuild_sv_from_cstr(str1);
		cbuild_sv_t sv6 = cbuild_sv_chop_by_func(&sv5, tdelim1, NULL);
		cbuild_sv_t sv7 = cbuild_sv_from_cstr(str1);
		cbuild_sv_t sv8 = cbuild_sv_chop_by_func(&sv7, tdelim2, NULL);
		TEST_ASSERT_EQ(
		  cbuild_sv_cmp(sv5, cbuild_sv_from_cstr("-|ghj")), 0,
		  "Wrong part left after chopping" TEST_EXPECT_RMSG(CBuildSVFmt),
		  CBuildSVArg(cbuild_sv_from_cstr("-|ghj")), CBuildSVArg(sv5));
		TEST_ASSERT_EQ(
		  cbuild_sv_cmp(sv6, cbuild_sv_from_cstr("abc|def")), 0,
		  "Wrong part chopped" TEST_EXPECT_RMSG(CBuildSVFmt),
		  CBuildSVArg(cbuild_sv_from_cstr("abc|def")), CBuildSVArg(sv6));
		TEST_ASSERT_EQ(
		  cbuild_sv_cmp(sv7, cbuild_sv_from_cstr("")), 0,
		  "Wrong part left after chopping (on overflow)" TEST_EXPECT_RMSG(
		  CBuildSVFmt),
		  CBuildSVArg(cbuild_sv_from_cstr("")), CBuildSVArg(sv7));
		TEST_ASSERT_EQ(cbuild_sv_cmp(sv8, cbuild_sv_from_cstr(str1)), 0,
		  "Wrong part chopped (on overflow)" TEST_EXPECT_RMSG(
		  CBuildSVFmt),
		  CBuildSVArg(cbuild_sv_from_cstr(str1)),
		  CBuildSVArg(sv8));
		printf("\tcbuild_sv_chop_by_delim\n");
		cbuild_sv_t sv9  = cbuild_sv_from_cstr(str1);
		cbuild_sv_t sv10 = cbuild_sv_chop_by_delim(&sv9, '|');
		cbuild_sv_t sv11 = cbuild_sv_from_cstr(str1);
		cbuild_sv_t sv12 = cbuild_sv_chop_by_delim(&sv11, '/');
		TEST_ASSERT_EQ(
		  cbuild_sv_cmp(sv9, cbuild_sv_from_cstr("def|-|ghj")), 0,
		  "Wrong part left after chopping" TEST_EXPECT_RMSG(CBuildSVFmt),
		  CBuildSVArg(cbuild_sv_from_cstr("def|-|ghj")),
		  CBuildSVArg(sv9));
		TEST_ASSERT_EQ(cbuild_sv_cmp(sv10, cbuild_sv_from_cstr("abc")), 0,
		  "Wrong part chopped" TEST_EXPECT_RMSG(CBuildSVFmt),
		  CBuildSVArg(cbuild_sv_from_cstr("abc")),
		  CBuildSVArg(sv10));
		TEST_ASSERT_EQ(
		  cbuild_sv_cmp(sv11, cbuild_sv_from_cstr("")), 0,
		  "Wrong part left after chopping (on overflow)" TEST_EXPECT_RMSG(
		  CBuildSVFmt),
		  CBuildSVArg(cbuild_sv_from_cstr("")), CBuildSVArg(sv11));
		TEST_ASSERT_EQ(cbuild_sv_cmp(sv12, cbuild_sv_from_cstr(str1)), 0,
		  "Wrong part chopped (on overflow)" TEST_EXPECT_RMSG(
		  CBuildSVFmt),
		  CBuildSVArg(cbuild_sv_from_cstr(str1)),
		  CBuildSVArg(sv12));
		printf("\tcbuild_sv_chop_by_sv\n");
		cbuild_sv_t sv13 = cbuild_sv_from_cstr(str1);
		cbuild_sv_t sv14 =
		cbuild_sv_chop_by_sv(&sv13, cbuild_sv_from_cstr("|-|"));
		cbuild_sv_t sv15 = cbuild_sv_from_cstr(str1);
		cbuild_sv_t sv16 =
		cbuild_sv_chop_by_sv(&sv15, cbuild_sv_from_cstr("---"));
		TEST_ASSERT_EQ(
		  cbuild_sv_cmp(sv13, cbuild_sv_from_cstr("ghj")), 0,
		  "Wrong part left after chopping" TEST_EXPECT_RMSG(CBuildSVFmt),
		  CBuildSVArg(cbuild_sv_from_cstr("ghj")), CBuildSVArg(sv13));
		TEST_ASSERT_EQ(
		  cbuild_sv_cmp(sv14, cbuild_sv_from_cstr("abc|def")), 0,
		  "Wrong part chopped" TEST_EXPECT_RMSG(CBuildSVFmt),
		  CBuildSVArg(cbuild_sv_from_cstr("abc|def")), CBuildSVArg(sv14));
		TEST_ASSERT_EQ(
		  cbuild_sv_cmp(sv15, cbuild_sv_from_cstr("")), 0,
		  "Wrong part left after chopping (on overflow)" TEST_EXPECT_RMSG(
		  CBuildSVFmt),
		  CBuildSVArg(cbuild_sv_from_cstr("")), CBuildSVArg(sv15));
		TEST_ASSERT_EQ(cbuild_sv_cmp(sv16, cbuild_sv_from_cstr(str1)), 0,
		  "Wrong part chopped (on overflow)" TEST_EXPECT_RMSG(
		  CBuildSVFmt),
		  CBuildSVArg(cbuild_sv_from_cstr(str1)),
		  CBuildSVArg(sv16));
	},
	"StringView chopping");
	TEST_CASE(
	{
		printf("\tDelimeter is part of long sequence of same chars\n");
		const char* str   = "foo---bar---baz";
		cbuild_sv_t sv    = cbuild_sv_from_cstr(str);
		cbuild_sv_t delim = cbuild_sv_from_cstr("--");

		cbuild_sv_t part  = cbuild_sv_chop_by_sv(&sv, delim);

		TEST_ASSERT_EQ(
		  cbuild_sv_cmp(part, cbuild_sv_from_cstr("foo")), 0,
		  "Multi-char delim chop: chopped part mismatch" TEST_EXPECT_RMSG(
		    CBuildSVFmt),
		  CBuildSVArg(cbuild_sv_from_cstr("foo")), CBuildSVArg(part));

		TEST_ASSERT_EQ(cbuild_sv_cmp(sv, cbuild_sv_from_cstr("-bar---baz")),
		  0,
		  "Multi-char delim chop: remaining part "
		  "mismatch" TEST_EXPECT_RMSG(CBuildSVFmt),
		  CBuildSVArg(cbuild_sv_from_cstr("-bar---baz")),
		  CBuildSVArg(sv));
		printf("\tDelimeter is a single character\n");
		sv    = cbuild_sv_from_cstr(str);
		delim = cbuild_sv_from_cstr("-");

		part  = cbuild_sv_chop_by_sv(&sv, delim);

		TEST_ASSERT_EQ(
		  cbuild_sv_cmp(part, cbuild_sv_from_cstr("foo")), 0,
		  "Multi-char delim chop: chopped part mismatch" TEST_EXPECT_RMSG(
		    CBuildSVFmt),
		  CBuildSVArg(cbuild_sv_from_cstr("foo")), CBuildSVArg(part));

		TEST_ASSERT_EQ(
		  cbuild_sv_cmp(sv, cbuild_sv_from_cstr("--bar---baz")), 0,
		  "Multi-char delim chop: remaining part "
		  "mismatch" TEST_EXPECT_RMSG(CBuildSVFmt),
		  CBuildSVArg(cbuild_sv_from_cstr("--bar---baz")),
		  CBuildSVArg(sv));
	},
	"StringView edge-case chop");
	TEST_CASE(
	{
		cbuild_sv_t sv1 = cbuild_sv_from_cstr("aaabacabaaa");
		TEST_ASSERT_EQ(cbuild_sv_find(sv1, 'b'), 3,
		  "Wrong index in find" TEST_EXPECT_MSG(zu), 3ul,
		  cbuild_sv_find(sv1, 'b'));
		TEST_ASSERT_EQ(cbuild_sv_find(sv1, 'z'), -1,
		  "Wrong error index in find" TEST_EXPECT_MSG(zd),
		  (ssize_t) -1, cbuild_sv_find(sv1, 'z'));
		TEST_ASSERT_EQ(cbuild_sv_rfind(sv1, 'b'), 7,
		  "Wrong index in rfind" TEST_EXPECT_MSG(zu), 7ul,
		  cbuild_sv_rfind(sv1, 'b'));
		TEST_ASSERT_EQ(cbuild_sv_contains(sv1, 'c'), true,
		  "Wrong result from contains" TEST_EXPECT_MSG(s),
		  "true",
		  cbuild_sv_contains(sv1, 'c') ? "true" : "false");
		cbuild_sv_t sv2 = cbuild_sv_from_cstr("aaabacdcdcabaaa");
		TEST_ASSERT_EQ(cbuild_sv_find_sv(sv2, cbuild_sv_from_cstr("dcd")), 6,
		  "Wrong index in find" TEST_EXPECT_MSG(zu), 6ul,
		  cbuild_sv_find_sv(sv2, cbuild_sv_from_cstr("dcd")));
		TEST_ASSERT_EQ(cbuild_sv_find_sv(sv2, cbuild_sv_from_cstr("zcz")), -1,
		  "Wrong error index in find" TEST_EXPECT_MSG(zd),
		  (ssize_t) -1, cbuild_sv_find_sv(sv2, cbuild_sv_from_cstr("zcz")));
		TEST_ASSERT_EQ(cbuild_sv_contains_sv(sv2, cbuild_sv_from_cstr("dcd")), true,
		  "Wrong result from contains" TEST_EXPECT_MSG(s),
		  "true",
		  cbuild_sv_contains_sv(sv2, cbuild_sv_from_cstr("dcd")) ? "true" : "false");
	},
	"StringView search");
	TEST_CASE({
		cbuild_sv_t sv1 = cbuild_sv_from_cstr("Привіт, world!");
		TEST_ASSERT(cbuild_sv_utf8valid(sv1, NULL),
		  "String view \""CBuildSVFmt"\" should be valid!", CBuildSVArg(sv1));
		unsigned char invalid_utf8_truncated[5] = {0};
		invalid_utf8_truncated[0] = 0x73;
		invalid_utf8_truncated[1] = 0xED;
		invalid_utf8_truncated[2] = 0x83;
		invalid_utf8_truncated[3] = 0xA0;
		invalid_utf8_truncated[4] = 0xED;
		cbuild_sv_t sv2 = cbuild_sv_from_parts(invalid_utf8_truncated,
		    sizeof(invalid_utf8_truncated));
		TEST_ASSERT(!cbuild_sv_utf8valid(sv2, NULL), "%s",
		  "String view with truncated codepoint should be invalid!");
		unsigned char invalid_utf8_prefix[3] = {0};
		invalid_utf8_prefix[0] = 0x73;
		invalid_utf8_prefix[1] = 0xC1;
		invalid_utf8_prefix[2] = 0x85;
		cbuild_sv_t sv3 = cbuild_sv_from_parts(invalid_utf8_prefix,
		    sizeof(invalid_utf8_prefix));
		TEST_ASSERT(!cbuild_sv_utf8valid(sv3, NULL), "%s",
		  "String view with invalid prefix character should be invalid!");
		unsigned char invalid_utf8_char[7] = {0};
		invalid_utf8_char[0] = 0xED;
		invalid_utf8_char[1] = 0x90;
		invalid_utf8_char[2] = 0x80;
		invalid_utf8_char[3] = 0xF2;
		invalid_utf8_char[4] = 0x97;
		invalid_utf8_char[5] = 0xCF;
		invalid_utf8_char[6] = 0xAC;
		cbuild_sv_t sv4 = cbuild_sv_from_parts(invalid_utf8_char,
		    sizeof(invalid_utf8_char));
		TEST_ASSERT(!cbuild_sv_utf8valid(sv4, NULL), "%s",
		  "String view with invalid character in codepoint should be invalid!");
	}, "UTF8 validation");
	TEST_CASE({
		cbuild_sv_t sv1 = cbuild_sv_from_cstr("A");
		int cp1len = cbuild_sv_utf8cp_len(sv1);
		TEST_ASSERT_EQ(cp1len, 1,
		  "Wrong length of ASCII char" TEST_EXPECT_MSG(d), 1, cp1len);
		cbuild_sv_t sv2 = cbuild_sv_from_cstr("ф");
		int cp2len = cbuild_sv_utf8cp_len(sv2);
		TEST_ASSERT_EQ(cp2len, 2,
		  "Wrong length of Cyrillic character" TEST_EXPECT_MSG(d), 2, cp2len);
		cbuild_sv_t sv3 = cbuild_sv_from_cstr("€");
		int cp3len = cbuild_sv_utf8cp_len(sv3);
		TEST_ASSERT_EQ(cp3len, 3,
		  "Wrong length of Euro currency symbol character"
		  TEST_EXPECT_MSG(d), 3, cp3len);
		cbuild_sv_t sv4 = cbuild_sv_from_cstr("😀");
		int cp4len = cbuild_sv_utf8cp_len(sv4);
		TEST_ASSERT_EQ(cp4len, 4,
		  "Wrong length of Emoji character" TEST_EXPECT_MSG(d), 4, cp4len);
	}, "UTF8 codepoint size");
	TEST_CASE({
		cbuild_sv_t sv = cbuild_sv_from_cstr("Привіт, world!€😀..."); // 19 chars
		size_t len = cbuild_sv_utf8len(sv);
		TEST_ASSERT_EQ(len, 19,
		  "Wrong length computed for utf8 string" TEST_EXPECT_MSG(zu), 19ul, len);
	}, "Length of a utf8 string view");
	TEST_CASE({
		cbuild_sv_t sv = cbuild_sv_from_cstr("aф€😀");
		uint32_t cp = cbuild_sv_chop_utf8(&sv);
		TEST_ASSERT_EQ(cp, 'a',
		  "Wrong character chopped from utf8 string"
		  TEST_EXPECT_RMSG("0x%08X"), 'a', cp);
		cp = cbuild_sv_chop_utf8(&sv);
		TEST_ASSERT_EQ(cp, 1092,
		  "Wrong character chopped from utf8 string"
		  TEST_EXPECT_RMSG("0x%08X"), 1092, cp);
		cp = cbuild_sv_chop_utf8(&sv);
		TEST_ASSERT_EQ(cp, 8364,
		  "Wrong character chopped from utf8 string"
		  TEST_EXPECT_RMSG("0x%08X"), 8364, cp);
		cp = cbuild_sv_chop_utf8(&sv);
		TEST_ASSERT_EQ(cp, 128512,
		  "Wrong character chopped from utf8 string"
		  TEST_EXPECT_RMSG("0x%08X"), 128512, cp);
	}, "Chopping utf 8 chars");
	TEST_CASE({
		printf("\tcbuild_sv_chop_by_utf8\n");
		cbuild_sv_t sv1 = cbuild_sv_from_cstr("aaaфbbb");
		cbuild_sv_t chopped1 = cbuild_sv_chop_by_utf8(&sv1, 1092);
		TEST_ASSERT_EQ(chopped1.size, 3,
		  "Wrong string view size after a chop"
		  TEST_EXPECT_MSG(zu), 3lu, chopped1.size);
		TEST_ASSERT_EQ(sv1.size, 3,
		  "Wrong string view size after a chop" TEST_EXPECT_MSG(zu), 3lu, sv1.size);
		TEST_ASSERT_MEMEQ(chopped1.data, "aaa", 3,
		  "Wrong string view chopped"
		  TEST_EXPECT_RMSG(CBuildSVFmt),
		  CBuildSVArg(cbuild_sv_from_cstr("aaa")),
		  CBuildSVArg(chopped1));
		TEST_ASSERT_MEMEQ(sv1.data, "bbb", 3,
		  "Wrong string view left after a chop"
		  TEST_EXPECT_RMSG(CBuildSVFmt),
		  CBuildSVArg(cbuild_sv_from_cstr("bbb")),
		  CBuildSVArg(sv1));
		printf("\tcbuild_sv_chop_by_func_utf8\n");
		cbuild_sv_t sv2 = cbuild_sv_from_cstr("aaaфbbb");
		uint32_t cp = 0;
		cbuild_sv_t chopped2 = cbuild_sv_chop_by_func_utf8(&sv2, tdelim3, &cp);
		TEST_ASSERT_EQ(cp, 1092,
		  "Wrong character used as delimiter" TEST_EXPECT_RMSG("0x%08X"), 1092, cp);
		TEST_ASSERT_EQ(chopped2.size, 3,
		  "Wrong string view size after a chop"
		  TEST_EXPECT_MSG(zu), 3ul, chopped2.size);
		TEST_ASSERT_EQ(sv2.size, 3,
		  "Wrong string view size after a chop" TEST_EXPECT_MSG(zu), 3ul, sv2.size);
		TEST_ASSERT_MEMEQ(chopped2.data, "aaa", 3,
		  "Wrong string view chopped"
		  TEST_EXPECT_RMSG(CBuildSVFmt),
		  CBuildSVArg(cbuild_sv_from_cstr("aaa")),
		  CBuildSVArg(chopped2));
		TEST_ASSERT_MEMEQ(sv2.data, "bbb", 3,
		  "Wrong string view left after a chop"
		  TEST_EXPECT_RMSG(CBuildSVFmt),
		  CBuildSVArg(cbuild_sv_from_cstr("bbb")),
		  CBuildSVArg(sv2));
	}, "Chopping by utf8 character");
	TEST_CASE({
		cbuild_sv_t sv1 = cbuild_sv_from_cstr("aф€😀");
		cbuild_sv_t sv2 = cbuild_sv_from_cstr("aф€");
		cbuild_sv_t sv3 = cbuild_sv_from_cstr("aaa");
		TEST_ASSERT_EQ(cbuild_sv_utf8cmp(sv1, sv1), 0,
		  "%s", "Comparison of utf8 strings is broken, same strings are not reported as such!");
		TEST_ASSERT_EQ(cbuild_sv_utf8cmp(sv1, sv2), 2,
		  "%s", "Comparison of utf8 strings is broken, different in size strings are not reported as such!");
		TEST_ASSERT_EQ(cbuild_sv_utf8cmp(sv2, sv3), 1,
		  "%s", "Comparison of utf8 strings is broken, different in content strings are not reported as such!");
	}, "Comparisons of utf8 strings.");
},
"StringView library tests")
