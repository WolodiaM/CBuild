/**
 * @file Proc.c
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Tests for some common and untructured modules
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
#include "../src/Proc.h"
#include "../src/common.h"
#include "framework.h"
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
TEST_MAIN(
		{
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
						if (code == 0) {
							TEST_ASSERT_STREQ(context.ptr, MSG,
			                          "Wring shared memory copy" TEST_EXPECT_MSG(s),
			                          MSG, (char*)context.ptr);
						} else {
							err_code++;
						}
						cbuild_proc_free(context);
					},
					"cbuild_proc_ctrl");
		},
		"Process control module tests")
