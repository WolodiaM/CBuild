/**
 * @file FlagParse.c
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief CLI flag parser tests
 *
 * @date 2025-06-26
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
// Library includes
#include "stdio.h"
// Project includes
#include "../src/FlagParse.h"
// Code
int main(int argc, char** argv) {
	cbuild_flag_new("-separator");

	cbuild_flag_new("arg_req\ta\targ=arg;thint=int\tOption with both long and short options.");
	cbuild_flag_new("list_req\tl\targ=list;thint=file\tOption with both long and short options.");
	cbuild_flag_new("tlist_req\tt\targ=tlist;tdelim=^;thint=boolean;len=3\tOption with both long and short options.");

	cbuild_flag_new("arg_opt\tA\targ=arg?;thint=float\tOption with both long and short options.");
	cbuild_flag_new("list_opt\tL\targ=list?;thint=float\tOption with both long and short options.");
	cbuild_flag_new("tlist_opt\tT\targ=tlist?;thint=file;tdelim=^;len=4\tOption with both long and short options.");

	cbuild_flag_new("long\n\tOption with long option only.");

	cbuild_flag_parse(argc, argv);
	return 0;
}
