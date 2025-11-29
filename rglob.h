/**
 * @file rglob.h
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Small glob implementation on top of POSIX regex.h for matching agains any string
 * Default glob.h can only match agains path. This can match agains
 * provided string or list of strings. Also, '/' is not a special character.
 *
 * @date 2025-11-28
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
#ifndef __GLOB_H__
#define __GLOB_H__
#include "regex.h"
#include "cbuild.h"
#define GLOB_CAPTURE_COUNT 10
typedef enum {
	GLOB_NOERROR      = 0,   // Overlap with REG_NOERROR
	GLOB_REGERROR_MAX = 128, // Lower values than these means error from a regex
	GLOB_NOMATCH      = 128, // No matches found in list.
	GLOB_EMPTY_PAT,          // Pattern is empty
} glob_errcode_t;
typedef struct {
	// Return
	size_t size;
	size_t capacity;
	struct __glob_res_t {
		const char* res;
		size_t res_idx;
	} *data;
	// Regex wrappers
	regex_t regex;
	regmatch_t capture[GLOB_CAPTURE_COUNT];
	// Internal state
	bool valid;
} glob_t;
typedef enum {
	// Use default glob syntax. Char substitution does not occur inside of '[...]'.
	// Substituted chars - '*' to '.*' and '?' to '.'. Some character are escaped
	// to not mess regex engine. This group consists of '(', ')', '.', '^', '$',
	// '|', '+', '{', '}'.
	// '#' is used for escaping. Supported chars:
	// '#', '*', '?'
	GLOB_SYNTAX_GLOB     = 0 << 0,
	// Use raw POSIX regex syntax. No transformation is performed.
	GLOB_SYNTAX_REGEX    = 1 << 0,
	// Compile glob into a regex
	GLOB_COMPILE         = 1 << 1
} glob_flags_t;
// Return -	either reg_errcode_t or glob_errcode_t or 0
// pglob.regex should be NULL to compile glob. pglob = {0} is fine.
int glob(const char *restrict pattern, int flags,	glob_t *restrict pglob,
	const char** list, size_t list_size);
void globfree(glob_t *pglob);
#ifdef GLOB_IMPLEMENTATION
	char* transform_glob(const char *restrict pattern, int syntax) {
		cbuild_sb_t regex = {0};
		if (syntax == GLOB_SYNTAX_GLOB) {
			enum {
				GPST_NORMAL,
				GPST_ESCAPE,
				GPST_CHAR_CLASS,
				GPST_CHAR_CLASS_FIRST,
			} state = GPST_NORMAL;
			cbuild_sb_append(&regex, '^');
			for(const char* pptr = pattern; *pptr != '\0'; pptr++) {
				if (state == GPST_ESCAPE) {
					cbuild_sb_append(&regex, *pptr);
					state = GPST_NORMAL;
					continue;
				}
				if (state == GPST_CHAR_CLASS_FIRST && *pptr == '!') {
					cbuild_sb_append(&regex, '^');
					state = GPST_CHAR_CLASS;
					continue;
				}
				if ((state == GPST_CHAR_CLASS || state == GPST_CHAR_CLASS_FIRST)
					&& *pptr != ']') {
					cbuild_sb_append(&regex, *pptr);
					state = GPST_CHAR_CLASS; // Clears GPST_CHAR_CLASS_FIRST
					continue;
				}
				switch(*pptr) {
				case '#': {
					state = GPST_ESCAPE;
				} break;
				case '[': {
					cbuild_sb_append(&regex, '[');
					state = GPST_CHAR_CLASS_FIRST;
				} break;
				case ']': {
					cbuild_sb_append(&regex, ']');
					state = GPST_NORMAL;
				} break;
				case '*': {
					cbuild_sb_append_cstr(&regex, ".*");
				} break;
				case '?': {
					cbuild_sb_append(&regex, '.');
				} break;
				case '(': __attribute__((fallthrough));
				case ')': __attribute__((fallthrough));
				case '{': __attribute__((fallthrough));
				case '}': __attribute__((fallthrough));
				case '^': __attribute__((fallthrough));
				case '$': __attribute__((fallthrough));
				case '.': __attribute__((fallthrough));
				case '|': __attribute__((fallthrough));
				case '+': {
					cbuild_sb_append(&regex, '\\');
					cbuild_sb_append(&regex, *pptr);
				} break;
				default: {
					cbuild_sb_append(&regex, *pptr);
				} break;
				}
			}
			cbuild_sb_append(&regex, '$');
		} else if (syntax == GLOB_SYNTAX_REGEX) {
			cbuild_sb_append_cstr(&regex, pattern);
		}
		cbuild_sb_append_null(&regex);
		return regex.data;
	}
	int glob(const char *restrict pattern, int flags,	glob_t *restrict pglob,
		const char** list, size_t list_size) { // TODO: kills	list elements
		if(strlen(pattern) == 0) return GLOB_EMPTY_PAT;
		if (flags & GLOB_COMPILE) {
			int syntax = flags & 0x1; // TODO: Maybe more syntaxes in future
			char* regex = transform_glob(pattern, syntax);
			int ecode = regcomp(&pglob->regex, regex, REG_EXTENDED | REG_NEWLINE);
			free(regex);
			if(ecode) return ecode;
			pglob->valid = 1;
		}
		cbuild_da_clear(pglob);
		for (size_t i = 0; i < list_size; i++) {
			int ecode = regexec(&pglob->regex, list[i], 10, pglob->capture, 0);
			struct __glob_res_t res = {.res = list[i], .res_idx = i};
			if (!ecode) cbuild_da_append(pglob, res);
		}
		if (pglob->size == 0) return GLOB_NOMATCH;
		return GLOB_NOERROR;
	}
	void globfree(glob_t *pglob) {
		regfree(&pglob->regex);
		pglob->valid = 0;
		cbuild_da_clear(pglob);
		memset(pglob->capture, 0, sizeof(regmatch_t) * GLOB_CAPTURE_COUNT);
	}
#endif // GLOB_IMPLEMENTATION
#endif // __GLOB_H__
