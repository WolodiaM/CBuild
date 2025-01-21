/**
 * @file Compile.c
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Tests for cbuild's compilation helpers
 *
 *
 * @date 2024-12-09
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
#include "../src/Compile.h"
#include "../src/Command.h"
#include "../src/FS.h"
#include "framework.h"
// Code
TEST_MAIN(
		{
			TEST_CASE(
					{
						// Create test data
						CBuildCmd file_writer = { 0 };
						cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
						CBuildFD pattern_fd = cbuild_fd_open_write("build/Compile.c.f1");
						cbuild_cmd_sync_redirect(
								file_writer,
								(CBuildCmdFDRedirect){ .fdstdin	 = CBUILD_INVALID_FD,
																			 .fdstdout = pattern_fd,
																			 .fdstderr = CBUILD_INVALID_FD });
						cbuild_fd_close(pattern_fd);
						sleep(2);
						file_writer.size = 0;
						cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
						pattern_fd = cbuild_fd_open_write("build/Compile.c.f2");
						cbuild_cmd_sync_redirect(
								file_writer,
								(CBuildCmdFDRedirect){ .fdstdin	 = CBUILD_INVALID_FD,
																			 .fdstdout = pattern_fd,
																			 .fdstderr = CBUILD_INVALID_FD });
						// Do tests
						int r1 = cbuild_compare_mtime("build/Compile.c.f1",
																					"build/Compile.c.f2");
						CHECK_CMP_VALUE(
								r1, 1, "Output is older but function returned %d, expected 1",
								r1);
						int r2 = cbuild_compare_mtime("build/Compile.c.f2",
																					"build/Compile.c.f1");
						CHECK_CMP_VALUE(
								r2, 0, "Output is newer but function returned %d, expected 0",
								r2);
						int r3 = cbuild_compare_mtime("build/Compile.c.f3",
																					"build/Compile.c.f1");
						CHECK_CMP_VALUE(
								r3, 1,
								"Output does not exists but function returned %d, expected 1",
								r3);
						int r4 = cbuild_compare_mtime("build/Compile.c.f2",
																					"build/Compile.c.f3");
						CHECK_CMP_VALUE(
								r4, -1,
								"Input does not exists but function returned %d, expected -1",
								r4);
						int r5 = cbuild_compare_mtime("build/Compile.c.f3",
																					"build/Compile.c.f3");
						CHECK_CMP_VALUE(
								r5, -1,
								"Input does not exists and output does not exist ( error "
								"because of output) but function returned %d, expected -1",
								r5);
					},
					"Single-file mtime check");
			TEST_CASE(
					{
						// Create test data
						CBuildCmd file_writer = { 0 };
						cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
						CBuildFD pattern_fd = cbuild_fd_open_write("build/Compile.c.f4");
						cbuild_cmd_sync_redirect(
								file_writer,
								(CBuildCmdFDRedirect){ .fdstdin	 = CBUILD_INVALID_FD,
																			 .fdstdout = pattern_fd,
																			 .fdstderr = CBUILD_INVALID_FD });
						cbuild_fd_close(pattern_fd);
						sleep(2);
						file_writer.size = 0;
						cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
						pattern_fd = cbuild_fd_open_write("build/Compile.c.f5");
						cbuild_cmd_sync_redirect(
								file_writer,
								(CBuildCmdFDRedirect){ .fdstdin	 = CBUILD_INVALID_FD,
																			 .fdstdout = pattern_fd,
																			 .fdstderr = CBUILD_INVALID_FD });
						sleep(2);
						file_writer.size = 0;
						cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
						pattern_fd = cbuild_fd_open_write("build/Compile.c.f6");
						cbuild_cmd_sync_redirect(
								file_writer,
								(CBuildCmdFDRedirect){ .fdstdin	 = CBUILD_INVALID_FD,
																			 .fdstdout = pattern_fd,
																			 .fdstderr = CBUILD_INVALID_FD });
						// Do tests
						char* fs1[2];
						fs1[0] = "build/Compile.c.f5";
						fs1[1] = "build/Compile.c.f6";
						int r1 = cbuild_compare_mtime_many("build/Compile.c.f4",
																							 (const char**)fs1, 2);
						CHECK_CMP_VALUE(r1, 2,
														"Output is older than two files but function "
														"returned %d, expected 2",
														r1)
						char* fs2[2];
						fs2[0] = "build/Compile.c.f4";
						fs2[1] = "build/Compile.c.f5";
						int r2 = cbuild_compare_mtime_many("build/Compile.c.f6",
																							 (const char**)fs2, 2);
						CHECK_CMP_VALUE(
								r2, 0, "Output is newer but function returned %d, expected 0",
								r2)
						char* fs3[2];
						fs3[0] = "build/Compile.c.f5";
						fs3[1] = "build/Compile.c.f6";
						int r3 = cbuild_compare_mtime_many("build/Compile.c.f7",
																							 (const char**)fs3, 2);
						CHECK_CMP_VALUE(r3, 2,
														"Output is newer than 2 inputs but function "
														"returned %d, expected 2",
														r3)
						char* fs4[2];
						fs4[0] = "build/Compile.c.f5";
						fs4[1] = "build/Compile.c.f7";
						int r4 = cbuild_compare_mtime_many("build/Compile.c.f6",
																							 (const char**)fs4, 2);
						CHECK_CMP_VALUE(r4, -1,
														"One of inputs does not exist, but function "
														"returned %d, expected -1",
														r4)
						char* fs5[3];
						fs5[0] = "build/Compile.c.f5";
						fs5[1] = "build/Compile.c.f6";
						fs5[1] = "build/Compile.c.f8";
						int r5 = cbuild_compare_mtime_many("build/Compile.c.f7",
																							 (const char**)fs5, 3);
						CHECK_CMP_VALUE(r5, -1,
														"One of inputs does not exist and output does not "
														"exist (error because of inputs), but function "
														"returned %d, expected -1",
														r5)
					},
					"Multi-file mtime check");
		},
		"Compilation helper");
