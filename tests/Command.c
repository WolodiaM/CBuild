/**
 * @file Command.c
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Tests for cbuild's command runner
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
#include <unistd.h>
#define CBUILD_COMPILE_HELPER
#include "../src/Command.h"
#include "../src/Compile.h"
#include "../src/StringBuilder.h"
#include "framework.h"
// Code
TEST_MAIN(
		{
			TEST_CASE(
					{
						cbuild_cmd_t c1 = cbuild_cmd;
						cbuild_cmd_append_many(&c1, "a", "b", "--flag", "-f",
		                               "\"val with spaces\"");
						cbuild_sb_t sb1 = cbuild_sb;
						cbuild_cmd_to_sb(c1, &sb1);
						char* cmp1 = "a b --flag -f \"val with spaces\"";
						TEST_ASSERT_STREQ(sb1.data, cmp1,
		                          "Get: \"" CBuildSBFmt "\", expected: \"%s\"",
		                          CBuildSBArg(sb1), cmp1);
						cbuild_cmd_clear(&c1);
						cbuild_cmd_append_many(&c1, CC,
		                               CBUILD_CARGS_DEFINE_VAL("VER", "1.0"),
		                               CBUILD_CARGS_INCLUDE("common.h"),
		                               "file\\ with\\ spaces.c", "-o", "file.run");
						cbuild_sb_clear(&sb1);
						cbuild_cmd_to_sb(c1, &sb1);
						char* cmp2 =
								"gcc -DVER=1.0 --include \"common.h\" file\\ with\\ spaces.c "
								"-o file.run";
						TEST_ASSERT_STREQ(sb1.data, cmp2,
		                          "Get: \"" CBuildSBFmt "\", expected: \"%s\"",
		                          CBuildSBArg(sb1), cmp2);
					},
					"Command buffer to string buffer conversion");
			TEST_CASE(
					{
						cbuild_cmd_t cmd     = cbuild_cmd;
						cbuild_cmd_append_many(&cmd, "printf", "ABCD");
						cbuild_fd_t fds[2]; // { [0] = read, [1] = write }
						int         pstatus = pipe(fds);
						if (pstatus < 0) {
							printf("Test runner got error!\n");
							exit(1);
						}
						bool ret = cbuild_cmd_sync_redirect(
								cmd, (CBuildCmdFDRedirect){ .fdstdout = fds[1],
		                                        .fdstdin  = CBUILD_INVALID_FD,
		                                        .fdstderr = CBUILD_INVALID_FD });
						TEST_ASSERT_EQ(ret, true, "%s", "Function returned error!");
						// TO be able to capture errors
						char str[1024];
						int  num = read(fds[0], str, 1024);
						TEST_ASSERT_EQ(
								num, 4,
								"Wrong number of bytes read from a pipe, 4 expected but got %d",
								num);
						TEST_ASSERT_MEMEQ(
								str, "ABCD", 4,
								"Wrong string read from a pipe" TEST_EXPECT_MSG(.*s), 4, "ABCD",
								1024, str);
					},
					"Check if command execution with redirect is working");
		},
		"Command runner")
