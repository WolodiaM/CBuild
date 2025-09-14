/**
 * @file FlagParse.c
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief CLI flag parser tests
 *
 * @date 2025-06-26
 * @copyright (C) 2025 WolodiaM
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
int main(int argc, char** argv) {
	cbuild_flag_new("-separator");

	cbuild_flag_new("arg_req\ta\targ=arg;thint=int;group=Requires arguments\tOption with both long and short options.");
	cbuild_flag_new("list_req\tl\targ=list;thint=file;group=Requires arguments\tOption with both long and short options.");
	cbuild_flag_new("tlist_req\tt\targ=tlist;tdelim=^;thint=boolean;len=3;group=Requires arguments\tOption with both long and short options.");

	cbuild_flag_new("arg_opt\tA\targ=arg?;thint=float;group=Argument is optional\tOption with both long and short options.");
	cbuild_flag_new("list_opt\tL\targ=list?;thint=float;group=Argument is optional\tOption with both long and short options.");
	cbuild_flag_new("tlist_opt\tT\targ=tlist?;thint=file;tdelim=^;len=4;group=Argument is optional\tOption with both long and short options.");

	cbuild_flag_new("long\n\tOption with long option only.");

	cbuild_flag_new("-alias:arg_req:arg1,arg2");

	cbuild_flag_new("-group:Requires arguments:All flags from this group requires at least one argument.");
	cbuild_flag_new("-group:Argument is optional:Flags from this group can live even without any arguments!");

	cbuild_flag_parse(argc, argv);

	return 0;
}
