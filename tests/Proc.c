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
#include "../src/FS.h"
#include "../src/common.h"
#include "framework.h"
// Code
int thread(void* context) {
	CBuildFD*		pipe = (CBuildFD*)context;
	const char* text = "Hello, world!";
	int					ret	 = write(*pipe, text, 14);
	if (ret != -1) {
		return 0;
	} else {
		return 1;
	}
}
TEST_MAIN(
		{
			TEST_CASE(
					{
						CBuildFD rd = 0;
						CBuildFD wr = 0;
						cbuild_fd_open_pipe(&rd, &wr);
						CBuildProc subproc = cbuild_proc_start(thread, &wr);
						int				 code		 = cbuild_proc_wait_code(subproc);
						CHECK_CMP_VALUE(code, 0,
														"Either subprocess exited abnormally or CBuild is "
														"broken, expected return code 0 but received %d",
														code);
						if (code == 0) {
							char buff[14];
							read(rd, buff, 14);
							CHECK_CMP_STR(buff, "Hello, world!",
														"Expected string \"Hello, world!\", but got \"%s\"",
														buff);
						} else {
							err_code++;
						}
					},
					"cbuild_proc_ctrl");
		},
		"Process control module tests");
