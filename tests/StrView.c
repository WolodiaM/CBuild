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
#include "../src/StringBuffer.h"
#include "../src/StringView.h"
#include "framework.h"
// Code
TEST_MAIN(
		{
			const char* reference1 = "abcd";
			TEST_CASE(
					{
						CBuildStrView str = cbuild_sv_from_cstr(reference1);

						CHECK_CMP_VALUE(str.len, 4,
														"Expected length is \"4\", real length is \"%zu\".",
														str.len);
						CHECK_CMP_MEM(reference1, str.str, 4,
													"StringView expected to have \"abcd\", but it "
													"actually has \"%.4s\".",
													str.str);
					},
					"Construction of StringView from c-string");
			TEST_CASE(
					{
						CBuildStrView str = cbuild_sv_from_parts(reference1, 4);

						CHECK_CMP_VALUE(str.len, 4,
														"Expected length is \"4\", real length is \"%zu\".",
														str.len);
						CHECK_CMP_MEM(reference1, str.str, 4,
													"StringView expected to have \"abcd\", but it "
													"actually has \"%.4s\".",
													str.str);
					},
					"Construction of StringView from raw parts");
			TEST_CASE(
					{
						CBuildStrBuff sb = { 0 };
						cbuild_sb_append_cstr(&sb, "abcd");
						CBuildStrView str = cbuild_sv_from_sb(&sb);

						CHECK_CMP_VALUE(str.len, 4,
														"Expected length is \"4\", real length is \"%zu\".",
														str.len);
						CHECK_CMP_MEM(reference1, str.str, 4,
													"StringView expected to have \"abcd\", but it "
													"actually has \"%.4s\".",
													str.str);
					},
					"Construction of StringView from StringBuff");
			TEST_CASE(
					{
						const char*		reference2 = "    abcd";
						CBuildStrView str				 = cbuild_sv_from_cstr(reference2);
						str											 = cbuild_sv_trim_left(str, ' ');
						CHECK_CMP_VALUE(str.len, 4,
														"Expected length is \"4\", real length is \"%zu\".",
														str.len);
						// NOTE: Don't do this in real code. This code work only because
						// str.str points from some point and to the end of c-string
						CHECK_CMP_STR(reference1, str.str,
													"StringView expected to have \"abcd\", but it "
													"actually has \"%.4s\".",
													str.str);
					},
					"StringView trim chars from left");
			TEST_CASE(
					{
						const char*		reference2 = "abcd    ";
						CBuildStrView str				 = cbuild_sv_from_cstr(reference2);
						str											 = cbuild_sv_trim_right(str, ' ');
						CHECK_CMP_VALUE(str.len, 4,
														"Expected length is \"4\", real length is \"%zu\".",
														str.len);
						CHECK_CMP_MEM(reference2, str.str, 4,
													"StringView expected to have \"abcd\", but it "
													"actually has \"%.4s\".",
													str.str);
					},
					"StringView trim chars from right");
			TEST_CASE(
					{
						const char*		reference2 = "cd";
						CBuildStrView str				 = cbuild_sv_from_cstr(reference1);
						str											 = cbuild_sv_cut_delim(str, 'b');
						CHECK_CMP_VALUE(str.len, 2,
														"Expected length is \"2\", real length is \"%zu\".",
														str.len);
						CHECK_CMP_MEM(reference2, str.str, 2,
													"StringView expected to have \"cd\", but it "
													"actually has \"%.2s\".",
													str.str);
					},
					"StringView cut by delimeter");
			TEST_CASE(
					{
						const char*		reference2 = "abc";
						const char*		reference3 = "dcba";
						CBuildStrView str0			 = { 0 };
						CBuildStrView str1			 = { 0 };
						int						ret;
						str0 = cbuild_sv_from_cstr(reference1);
						str1 = cbuild_sv_from_cstr(reference2);
						ret	 = cbuild_sv_cmp(str0, str1);
						CHECK_CMP_VALUE(ret, 2,
														"Wrong string size comparison, expected return "
														"\"2\", actual: \"%d\"",
														ret);
						ret = cbuild_sv_cmp(str1, str0);
						CHECK_CMP_VALUE(
								ret, -2,
								"Wrong string size comparison, expected return \"-2\", "
								"actual: \"%d\"",
								ret);
						str1 = cbuild_sv_from_cstr(reference1);
						ret	 = cbuild_sv_cmp(str1, str0);
						CHECK_CMP_VALUE(ret, 0,
														"Wrong string comparison, expected return \"0\", "
														"actual: \"%d\"",
														ret);
						str1 = cbuild_sv_from_cstr(reference3);
						ret	 = cbuild_sv_cmp(str0, str1);
						CHECK_CMP_VALUE(ret, -1,
														"Wrong string comparison, expected return \"-1\", "
														"actual: \"%d\"",
														ret);
						ret = cbuild_sv_cmp(str1, str0);
						CHECK_CMP_VALUE(ret, 1,
														"Wrong string comparison, expected return \"1\", "
														"actual: \"%d\"",
														ret);
					},
					"StringView comparisons");
			TEST_CASE(
					{
						const char*		reference2 = "abcdabcd";
						CBuildStrView str				 = cbuild_sv_from_parts(reference2, 4);
						char					buff[128];
						sprintf(buff, CBuildSVFmt, CBuildSVArg(str));
						CHECK_CMP_STR(buff, reference1,
													"Expected string \"abcd\", real: \"%s\"", buff);
					},
					"StringView formating for printf/sprintf style formaters");
		},
		"StringView library tests")
