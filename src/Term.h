#pragma once // For LSP 
//! This file provides utilities to construct ANSII escape codes.
//!
//! ::: note
//! This only provides strings for escape codes and some macro to build that strings
//! :::

/// ANSII escape starter.
#define CBUILD_TERM_ESCAPE "\x1b["
/// CSI - Control Sequence Introducer.
#define CBUILD_TERM_CSI    CBUILD_TERM_ESCAPE
/// SGR - Select Graphic Rendition, meta-function to generate proper code.
#define CBUILD_TERM_SGR(n) CBUILD_TERM_ESCAPE n "m"

/// Set cursor to specific coordinates.
#define CBUILD_TERM_CURSOR_SET(x, y)                                           \
	CBUILD_TERM_CSI __CBUILD_STRINGIFY(x) ";" __CBUILD_STRINGIFY(y) "H"
/// Set cursor to specific column in current line
#define CBUILD_TERM_CURSOR_SETH(x) CBUILD_TERM_CSI __CBUILD_STRINGIFY(x) "G"
/// Move cursor in specific direction for specified number of steps.
///
/// * [pl:direction:direction] One of CBUILD_TERM_[UP|DOWN|RIGHT|LEFT].
/// * [pl:num_steps:unsigned int] Number of steps, should be either literal or define.
#define CBUILD_TERM_CURSOR_MOVE(direction, num_steps)                          \
	CBUILD_TERM_CSI __CBUILD_XSTRINGIFY(num_steps) direction
///
#define CBUILD_TERM_UP        "A"
///
#define CBUILD_TERM_DOWN      "B"
///
#define CBUILD_TERM_RIGHT     "C"
///
#define CBUILD_TERM_LEFT      "D"
///
#define CBUILD_TERM_NEXT_LINE "E"
///
#define CBUILD_TERM_PREV_LINE "F"
/// Get current position of a cursor.
#define CBUILD_TERM_CURSOR_GET CBUILD_TERM_CSI "6n"

/// Clean specific part of terminal
///
/// * [pl:mode:clear mode] One of CBUILD_TERM_[CURSOR_TO_END|CURSOR_TO_BEGIN|FULL|FULL_SCROLLBACK].
#define CBUILD_TERM_ERASE_SCR(mode) CBUILD_TERM_CSI __CBUILD_STRINGIFY(mode) "J"
/// Clean specific part of terminal
///
/// * [pl:mode:clear mode] One of CBUILD_TERM_[CURSOR_TO_END|CURSOR_TO_BEGIN|FULL].
#define CBUILD_TERM_ERASE_LINE(mode)                                           \
	CBUILD_TERM_CSI __CBUILD_STRINGIFY(mode) "K"
///
#define CBUILD_TERM_CURSOR_TO_END    0
///
#define CBUILD_TERM_CURSOR_TO_BEGIN  1
///
#define CBUILD_TERM_FULL             2
///
#define CBUILD_TERM_FULL_SCROLLBACK  3

/// Reset all styling
#define CBUILD_TERM_RESET            CBUILD_TERM_SGR("0")
///
#define CBUILD_TERM_BOLD             CBUILD_TERM_SGR("1")
///
#define CBUILD_TERM_FAINT            CBUILD_TERM_SGR("2")
///
#define CBUILD_TERM_ITALIC           CBUILD_TERM_SGR("3")
/// Disable italic
#define CBUILD_TERM_NITALIC          CBUILD_TERM_SGR("23")
///
#define CBUILD_TERM_UNDERLINE        CBUILD_TERM_SGR("4")
/// Disable underline
#define CBUILD_TERM_NUNDERLINE       CBUILD_TERM_SGR("24")
/// Enable blinking text
#define CBUILD_TERM_BLINK            CBUILD_TERM_SGR("5")
/// Disable blinking text
#define CBUILD_TERM_NBLINK           CBUILD_TERM_SGR("25")
/// Enable swapping of background and foreground color
#define CBUILD_TERM_REVERSE          CBUILD_TERM_SGR("7")
/// Disable reverse mode
#define CBUILD_TERM_NREVERSE         CBUILD_TERM_SGR("27")
/// Enable crossed-out text rendering
#define CBUILD_TERM_CROSS_OUT        CBUILD_TERM_SGR("9")
/// Disable crossed-out rendering
#define CBUILD_TERM_NCROSS_OUT       CBUILD_TERM_SGR("29")
///
#define CBUILD_TERM_DOUBLE_UNDERLINE CBUILD_TERM_SGR("21")
///
#define CBUILD_TERM_OVERLINE         CBUILD_TERM_SGR("53")
/// Disable overline
#define CBUILD_TERM_NOVERLINE        CBUILD_TERM_SGR("55")

/// Set foreground color based on 16 color palette.
///
/// * [pl:color:Color] One of CBUILD_TERM_<COLOR> defines.
#define CBUILD_TERM_FG(color) CBUILD_TERM_SGR(__CBUILD_XSTRINGIFY(color))
/// Set background color based on 16 color palette.
///
/// * [pl:color:Color] One of CBUILD_TERM_<COLOR> defines.
#define CBUILD_TERM_BG(color) CBUILD_TERM_SGR(__CBUILD_XSTRINGIFY(color##addten))
/// Set foreground color in terms of [8-bit palette](https://en.wikipedia.org/wiki/ANSI_escape_code#8-bit)
#define CBUILD_TERM_FG8(color)                                                 \
	CBUILD_TERM_SGR("38;5;" __CBUILD_STRINGIFY(color))
/// Set background color in terms of [8-bit palette](https://en.wikipedia.org/wiki/ANSI_escape_code#8-bit)
#define CBUILD_TERM_BG8(color)                                                 \
	CBUILD_TERM_SGR("48;5;" __CBUILD_STRINGIFY(color))
/// Set underline color in terms of [8-bit palette](https://en.wikipedia.org/wiki/ANSI_escape_code#8-bit)
#define CBUILD_TERM_UNDERLINE8(color)                                          \
	CBUILD_TERM_SGR("58;5;" __CBUILD_STRINGIFY(color))
/// Set foreground color to to some RGB888 color
#define CBUILD_TERM_FG24(r, g, b)                                              \
	CBUILD_TERM_SGR("38;2;" __CBUILD_STRINGIFY(r) ";" __CBUILD_STRINGIFY(g) ";"  \
		__CBUILD_STRINGIFY(b))
/// Set background color to to some RGB888 color
#define CBUILD_TERM_BG24(r, g, b)                                              \
	CBUILD_TERM_SGR("48;2;" __CBUILD_STRINGIFY(r) ";" __CBUILD_STRINGIFY(g) ";"  \
		__CBUILD_STRINGIFY(b))
/// Set underline color to to some RGB888 color
#define CBUILD_TERM_UNDERLINE24(r, g, b)                                       \
	CBUILD_TERM_SGR("58;2;" __CBUILD_STRINGIFY(r) ";" __CBUILD_STRINGIFY(g) ";"  \
		__CBUILD_STRINGIFY(b))

//! # Colors

///
#define CBUILD_TERM_BLACK           30
///
#define CBUILD_TERM_RED             31
///
#define CBUILD_TERM_GREEN           32
///
#define CBUILD_TERM_YELLOW          33
///
#define CBUILD_TERM_BLUE            34
///
#define CBUILD_TERM_MAGENTA         35
///
#define CBUILD_TERM_CYAN            36
///
#define CBUILD_TERM_WHITE           37
///
#define CBUILD_TERM_BRBLACK         90
///
#define CBUILD_TERM_BRRED           91
///
#define CBUILD_TERM_BRGREEN         92
///
#define CBUILD_TERM_BRYELLOW        93
///
#define CBUILD_TERM_BRBLUE          94
///
#define CBUILD_TERM_BRMAGENTA       95
///
#define CBUILD_TERM_BRCYAN          96
///
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
