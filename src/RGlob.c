#include "RGlob.h"
#include "Common.h"
#include "StringBuilder.h"
#include "DynArray.h"

CBUILDDEF bool cbuild_glob_compile_opt(cbuild_glob_t* glob, const char* pattern,
	struct cbuild_glob_opts_t opts) {
	cbuild_sb_t regex = {0};
	enum {
		PARSE_NORMAL,
		PARSE_ESCAPE,
		PARSE_CHAR_CLASS,
		PARSE_CHAR_CLASS_FIRST,
		PARSE_CHAR_CLASS_AFTER_LAST,
	} state = PARSE_NORMAL;
	if (!opts.partial_match) cbuild_sb_append(&regex, '^');
	for(const char* pptr = pattern; *pptr != '\0'; pptr++) {
		if (state == PARSE_CHAR_CLASS_AFTER_LAST) {
			state = PARSE_NORMAL;
			if (*pptr == '*') {
				cbuild_sb_append(&regex, *pptr);
				continue;
			}
		}
		if (state == PARSE_ESCAPE) {
			cbuild_sb_append(&regex, *pptr);
			state = PARSE_NORMAL;
			continue;
		}
		if (state == PARSE_CHAR_CLASS_FIRST && *pptr == '!') {
			cbuild_sb_append(&regex, '^');
			state = PARSE_CHAR_CLASS;
			continue;
		}
		if ((state == PARSE_CHAR_CLASS || state == PARSE_CHAR_CLASS_FIRST)
			&& *pptr != ']') {
			cbuild_sb_append(&regex, *pptr);
			state = PARSE_CHAR_CLASS; // Clears GPST_CHAR_CLASS_FIRST
			continue;
		}
		switch(*pptr) {
		case '#': {
			state = PARSE_ESCAPE;
		} break;
		case '[': {
			cbuild_sb_append(&regex, '[');
			state = PARSE_CHAR_CLASS_FIRST;
		} break;
		case ']': {
			cbuild_sb_append(&regex, ']');
			state = PARSE_CHAR_CLASS_AFTER_LAST;
		} break;
		case '*': {
			cbuild_sb_append_cstr(&regex, ".*");
		} break;
		case '?': {
			cbuild_sb_append(&regex, '.');
		} break;
		case '(': CBUILD_ATTR_FALLTHROUGH();
		case ')': CBUILD_ATTR_FALLTHROUGH();
		case '{': CBUILD_ATTR_FALLTHROUGH();
		case '}': CBUILD_ATTR_FALLTHROUGH();
		case '^': CBUILD_ATTR_FALLTHROUGH();
		case '$': CBUILD_ATTR_FALLTHROUGH();
		case '.': CBUILD_ATTR_FALLTHROUGH();
		case '|': CBUILD_ATTR_FALLTHROUGH();
		case '+': {
			cbuild_sb_append(&regex, '\\');
			cbuild_sb_append(&regex, *pptr);
		} break;
		default: {
			cbuild_sb_append(&regex, *pptr);
		} break;
		}
	}
	if (!opts.partial_match) cbuild_sb_append(&regex, '$');
	cbuild_sb_append_null(&regex);
	int ecode = regcomp(&glob->regex, regex.data, REG_EXTENDED | REG_NEWLINE);
	cbuild_sb_clear(&regex);
	return ecode == 0;
}
CBUILDDEF bool cbuild_glob_match(cbuild_glob_t* glob, const char** list, size_t size) {
	for (size_t i = 0; i < size; i++) {
		int ecode = regexec(&glob->regex, list[i], 
			CBUILD_GLOB_CAPTURE_COUNT, glob->captures, 0);
		if (ecode == 0) {
			struct __cbuild_glob_res_t res = {0};
			res.res = list[i];
			res.res_idx = i;
			memcpy(res.captures, glob->captures, sizeof(glob->captures));
			cbuild_da_append(glob, res);
		}
	}
	return glob->size != 0;
}
CBUILDDEF bool cbuild_glob_match_single(cbuild_glob_t* glob, const char* elem) {
	int ecode = regexec(&glob->regex, elem,
		CBUILD_GLOB_CAPTURE_COUNT, glob->captures, 0);
	return ecode == 0;
}
CBUILDDEF void cbuild_glob_free(cbuild_glob_t* glob) {
	cbuild_da_clear(glob);
	regfree(&glob->regex);
}
