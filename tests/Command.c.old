/**
 * @file Command.c
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Tests for cbuild's command runner
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
TEST_MAIN({
		TEST_CASE(
			{
				cbuild_cmd_t c1 = {0};
				cbuild_cmd_append_many(&c1, "a", "b", "--flag", "-f",
					"val with spaces");
				cbuild_sb_t sb1 = cbuild_cmd_to_sb(c1);
				cbuild_sb_append_null(&sb1);
				char* cmp1 = "a b --flag -f \'val with spaces\'";
				TEST_ASSERT_STREQ(sb1.data, cmp1,
					"Get: \"" CBuildSBFmt "\", expected: \"%s\"",
					CBuildSBArg(sb1), cmp1);
				cbuild_cmd_clear(&c1);
				cbuild_cmd_append_many(&c1, CBUILD_CC,
					CBUILD_CARGS_DEFINE_VAL("VER", "1.0"),
					CBUILD_CARGS_INCLUDE("common.h"),
					"file with spaces.c", "-o", "file.run");
				cbuild_sb_clear(&sb1);
				sb1 = cbuild_cmd_to_sb(c1);
				cbuild_sb_append_null(&sb1);
				char* cmp2 =
				CBUILD_CC " -DVER=1.0 --include common.h 'file with spaces.c' -o file.run";
				TEST_ASSERT_STREQ(sb1.data, cmp2,
					"Get: \"" CBuildSBFmt "\", expected: \"%s\"",
					CBuildSBArg(sb1), cmp2);
				cbuild_sb_clear(&sb1);
				cbuild_cmd_clear(&c1);
			},
			"Command buffer to string buffer conversion");
		TEST_CASE(
			{
				cbuild_cmd_t cmd = {0};
				cbuild_cmd_append_many(&cmd, "printf", "ABCD");
				cbuild_fd_t rd;
				cbuild_fd_t wr;
				if(!cbuild_fd_open_pipe(&rd, &wr)) {
					printf("Test runner got error!\n");
					exit(1);
				}
				bool ret = cbuild_cmd_run(&cmd, .fdstdout = &wr);
				TEST_ASSERT_EQ(ret, true, "%s", "Function returned error!");
				// To be able to capture errors
				char str[1024];
				ssize_t num = cbuild_fd_read(rd, str, 1024);
				TEST_ASSERT_EQ(
					num, 4,
					"Wrong number of bytes read from a pipe, 4 expected but got %"PRId64,
					num);
				TEST_ASSERT_MEMEQ(
					str, "ABCD", 4,
					"Wrong string read from a pipe" TEST_EXPECT_MSG(.*s), 4, "ABCD",
					1024, str);
				cbuild_cmd_clear(&cmd);
			},
			"Check if command execution with redirect is working");
		TEST_CASE(
			{
				cbuild_cmd_t cmd = {0};
				cbuild_proclist_t procs = {0};
				cbuild_cmd_append_many(&cmd, "sh", "-c", "sleep 3; echo 1");
				TEST_ASSERT(cbuild_cmd_run(&cmd, .async_threads = 2, .procs = &procs), "%s", "Failed to run first command!");
				cbuild_cmd_append_many(&cmd, "sh", "-c", "sleep 1; echo 2");
				TEST_ASSERT(cbuild_cmd_run(&cmd, .async_threads = 2, .procs = &procs), "%s", "Failed to run first command!");
				cbuild_cmd_append_many(&cmd, "sh", "-c", "echo 3");
				TEST_ASSERT(cbuild_cmd_run(&cmd, .async_threads = 2, .procs = &procs), "%s", "Failed to run first command!");
				cbuild_procs_wait(procs);
				cbuild_cmd_clear(&cmd);
				cbuild_proclist_clear(&procs);
			},
			"Async command runner");
	},
	"Command runner")
