/**
 * @file term.h
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Simple library for working with ANSI escape codes
 *
 * @date 2025-01-15
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
#ifndef __CBUILD_TERM_H__
#define __CBUILD_TERM_H__
// Project includes
#include "common.h"
// Code
// Escape
#define CBUILD_TERM_ESCAPE "\033["
#define CBUILD_TERM_CSI    CBUILD_TERM_ESCAPE // CSI - Control Sequence Introducer
#define CBUILD_TERM_SGR(n)                                                     \
	CBUILD_TERM_ESCAPE n "m"                    // SGR - Select Graphic Rendition
// Cursor
#define CBUILD_TERM_CURSOR_SET(x, y)                                           \
	CBUILD_TERM_CSI __CBUILD_STRINGIFY(x) ";" __CBUILD_STRINGIFY(y) "H"
#define CBUILD_TERM_CURSOR_SETH(x) CBUILD_TERM_CSI __CBUILD_STRINGIFY(x) "G"
#define CBUILD_TERM_CURSOR_MOVE(direction, num_steps)                          \
	CBUILD_TERM_CSI __CBUILD_STRINGIFY(num_steps) direction
#define CBUILD_TERM_UP              "A"
#define CBUILD_TERM_DOWN            "B"
#define CBUILD_TERM_RIGHT           "C"
#define CBUILD_TERM_LEFT            "D"
#define CBUILD_TERM_NEXT_LINE       "E"
#define CBUILD_TERM_PREV_LINE       "F"
#define CBUILD_TERM_CURSOR_GET      CBUILD_TERM_CSI "6n"
// Clean
#define CBUILD_TERM_ERASE_SCR(mode) CBUILD_TERM_CSI __CBUILD_STRINGIFY(mode) "J"
#define CBUILD_TERM_ERASE_LINE(mode)                                           \
	CBUILD_TERM_CSI __CBUILD_STRINGIFY(mode) "K"
#define CBUILD_TERM_CURSOR_TO_END    0
#define CBUILD_TERM_CURSOR_TO_BEGIN  1
#define CBUILD_TERM_FULL             2 // Resets cursor in screen mode
#define CBUILD_TERM_FULL_SCROLLBACK  3 // Dont work in line mode
// Style
#define CBUILD_TERM_RESET            CBUILD_TERM_SGR("0")
#define CBUILD_TERM_BOLD             CBUILD_TERM_SGR("1")
#define CBUILD_TERM_FAINT            CBUILD_TERM_SGR("2")
#define CBUILD_TERM_ITALIC           CBUILD_TERM_SGR("3")
#define CBUILD_TERM_NITALIC          CBUILD_TERM_SGR("23")
#define CBUILD_TERM_UNDERLINE        CBUILD_TERM_SGR("4")
#define CBUILD_TERM_NUNDERLINE       CBUILD_TERM_SGR("24")
#define CBUILD_TERM_BLINK            CBUILD_TERM_SGR("5")
#define CBUILD_TERM_NBLINK           CBUILD_TERM_SGR("25")
#define CBUILD_TERM_REVERSE          CBUILD_TERM_SGR("7")
#define CBUILD_TERM_NREVERSE         CBUILD_TERM_SGR("27")
#define CBUILD_TERM_CROSS_OUT        CBUILD_TERM_SGR("9")
#define CBUILD_TERM_NCROSS_OUT       CBUILD_TERM_SGR("29")
#define CBUILD_TERM_DOUBLE_UNDERLINE CBUILD_TERM_SGR("21")
#define CBUILD_TERM_OVERLINE         CBUILD_TERM_SGR("53")
#define CBUILD_TERM_NOVERLINE        CBUILD_TERM_SGR("55")
// Color - 4 bit
#define CBUILD_TERM_FG(color)        CBUILD_TERM_SGR(__CBUILD_STRINGIFY(color))
#define CBUILD_TERM_BG(color)                                                  \
	CBUILD_TERM_SGR(__CBUILD_XSTRINGIFY(color##addten))
#define CBUILD_TERM_BLACK           30
#define CBUILD_TERM_RED             31
#define CBUILD_TERM_GREEN           32
#define CBUILD_TERM_YELLOW          33
#define CBUILD_TERM_BLUE            34
#define CBUILD_TERM_MAGENTA         35
#define CBUILD_TERM_CYAN            36
#define CBUILD_TERM_WHITE           37
#define CBUILD_TERM_BRBLACK         90
#define CBUILD_TERM_BRRED           91
#define CBUILD_TERM_BRGREEN         92
#define CBUILD_TERM_BRYELLOW        93
#define CBUILD_TERM_BRBLUE          94
#define CBUILD_TERM_BRMAGENTA       95
#define CBUILD_TERM_BRCYAN          96
#define CBUILD_TERM_BRWHITE         97
#define CBUILD_TERM_BLACKaddten     40
#define CBUILD_TERM_REDaddten       41
#define CBUILD_TERM_GREENaddten     42
#define CBUILD_TERM_YELLOWaddten    43
#define CBUILD_TERM_BLUEaddten      44
#define CBUILD_TERM_MAGENTAaddten   45
#define CBUILD_TERM_CYANaddten      46
#define CBUILD_TERM_WHITEaddten     47
#define CBUILD_TERM_BRBLACKaddten   100
#define CBUILD_TERM_BRREDaddten     101
#define CBUILD_TERM_BRGREENaddten   102
#define CBUILD_TERM_BRYELLOWaddten  103
#define CBUILD_TERM_BRBLUEaddten    104
#define CBUILD_TERM_BRMAGENTAaddten 105
#define CBUILD_TERM_BRCYANaddten    106
#define CBUILD_TERM_BRWHITEaddten   107
// Color - 8 bit
#define CBUILD_TERM_FG8(color)                                                 \
	CBUILD_TERM_SGR("38;5;" __CBUILD_STRINGIFY(color))
#define CBUILD_TERM_BG8(color)                                                 \
	CBUILD_TERM_SGR("48;5;" __CBUILD_STRINGIFY(color))
#define CBUILD_TERM_UNDERLINE8(color)                                          \
	CBUILD_TERM_SGR("58;5;" __CBUILD_STRINGIFY(color))
// Color - 24 bit
#define CBUILD_TERM_FG24(r, g, b)                                              \
	CBUILD_TERM_SGR("38;2;" __CBUILD_STRINGIFY(r) ";" __CBUILD_STRINGIFY(        \
			g) ";" __CBUILD_STRINGIFY(b))
#define CBUILD_TERM_BG24(r, g, b)                                              \
	CBUILD_TERM_SGR("48;2;" __CBUILD_STRINGIFY(r) ";" __CBUILD_STRINGIFY(        \
			g) ";" __CBUILD_STRINGIFY(b))
#define CBUILD_TERM_UNDERLINE24(r, g, b)                                       \
	CBUILD_TERM_SGR("58;2;" __CBUILD_STRINGIFY(r) ";" __CBUILD_STRINGIFY(        \
			g) ";" __CBUILD_STRINGIFY(b))
#endif // __CBUILD_TERM_H__
