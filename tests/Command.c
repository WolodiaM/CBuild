/**
 * @file Command.c
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Tests for cbuild's command runner
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
#include "../src/Command.h"
#include "../src/Compile.h"
#include "../src/FS.h"
#include "../src/StringBuffer.h"
#include "framework.h"
// Code
TEST_MAIN(
		{
			TEST_CASE(
					{
						CBuildCmd c1 = { 0 };
						cbuild_cmd_append_many(&c1, "a", "b", "--flag", "-f",
																	 "\"val with spaces\"");
						CBuildStrBuff sb1 = { 0 };
						cbuild_cmd_to_sb(c1, &sb1);
						char* cmp1 = "a b --flag -f \"val with spaces\" ";
						CHECK_CMP_STR(sb1.data, cmp1,
													"Get: \"" CBuildSBFmt "\", expected: \"%s\"",
													CBuildSBArg(sb1), cmp1);
						CBuildCmd c2 = { 0 };
						cbuild_cmd_append_many(&c2, CC,
																	 CBUILD_CARGS_DEFINE_VAL("VER", "1.0"),
																	 CBUILD_CARGS_INCLUDE("common.h"),
																	 "file\\ with\\ spaces.c", "-o", "file.run");
						CBuildStrBuff sb2 = { 0 };
						cbuild_cmd_to_sb(c2, &sb2);
						char* cmp2 =
								"gcc -DVER=1.0 --include \"common.h\" file\\ with\\ spaces.c "
								"-o file.run ";
						CHECK_CMP_STR(sb2.data, cmp2,
													"Get: \"" CBuildSBFmt "\", expected: \"%s\"",
													CBuildSBArg(sb2), cmp2);
					},
					"Command buffer to string buffer conversion");
			TEST_CASE(
					{
						CBuildCmd cmd = { 0 };
						cbuild_cmd_append_many(&cmd, "printf", "ABCD");
						CBuildFD rd;
						CBuildFD wr;
						cbuild_fd_open_pipe(&rd, &wr);
						bool ret = cbuild_cmd_sync_redirect(
								cmd, (CBuildCmdFDRedirect){ .fdstdout = wr,
																						.fdstdin	= CBUILD_INVALID_FD,
																						.fdstderr = CBUILD_INVALID_FD });
						CHECK_CMP_VALUE(ret, true, "Function returened error%s", "");
						char str[64];
						int	 num = read(rd, str, 64);
						CHECK_CMP_VALUE(
								num, 4,
								"Wrong nuber of bytes read from a pipe, 4 expected but got %d",
								num);
						CHECK_CMP_MEM(str, "ABCD", 4,
													"Wrong string read from a pipe, \"ABCD\" expected "
													"but got \"%.*s\"",
													4, str);
					},
					"Check if command execution with redirect is working");
		},
		"Command runner")
