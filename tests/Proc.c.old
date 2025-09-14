/**
 * @file Proc.c
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Tests for some common and untructured modules
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
const char* MSG = "Hello, world!";
int         thread(void* context) {
	setvbuf(stdout, NULL, _IONBF, 0);
	sleep(2);
	printf("\tHello from child proc [%d]\n", getpid());
	fflush(stdout);
	memcpy(context, MSG, strlen(MSG) + 1);
	return 0;
}
TEST_MAIN({
	TEST_CASE(
	{
		cbuild_proc_ptr_t context = cbuild_proc_malloc(strlen(MSG) + 1);
		TEST_ASSERT_EQ(
		  context.size, strlen(MSG) + 1,
		  "Wrong amount of elements set in ptr" TEST_EXPECT_MSG(zu),
		  strlen(MSG) + 1, context.size);

		TEST_ASSERT_NEQ(context.ptr, NULL, "%s",
		  "NULL returned from mmap!");
		cbuild_proc_t subproc = cbuild_proc_start(thread, context.ptr);
		printf("\tHello from main proc [%d]\n", getpid());
		fflush(stdout);
		int code = cbuild_proc_wait_code(subproc);
		TEST_ASSERT_EQ(code, 0,
		  "Either subprocess exited abnormally or CBuild is "
		  "broken" TEST_EXPECT_MSG(d),
		  0, code);
		if(code == 0) {
			TEST_ASSERT_STREQ(context.ptr, MSG,
			  "Wr0ng shared memory copy" TEST_EXPECT_MSG(s),
			  MSG, (char*)context.ptr);
		} else {
			err_code++;
		}
		cbuild_proc_free(context);
	},
	"cbuild_proc_ctrl");
},
"Process control module tests")
