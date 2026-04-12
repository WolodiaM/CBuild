#pragma once // For LSP
//! Glob syntax implementation over POSIX ERE regex.
//!
//! License: `GPL-3.0-or-later`.
//!
//! # Preprocessor
//!
//! This glob is implemented as a preprocessor and a series of wrappers around
//! POSIX regex API. This preprocessor translate glob patter to valid and equal
//! regex pattern. This includes both escaping regex's special characters and
//! translating glob's special characters to equal sequences of regexes.
//!
//! # Pattern format
//!
//! In general any valid glob patter can be used, it support this "operators":
//!
//! * `?` - match any single character. (Translates to `.`).
//! * `*` - match any sequence of characters. (Translates to `.*`).
//! * `[...]` - matches any character from character class. (Translates to
//!   itself literally).
//! * `!` as first element of character class to invert character class.
//!   (Translates to '^').
//!
//! It also supports some syntax extensions:
//!
//! * Often you want to match some specific character multiple characters. In
//!   regex you can just write `[...]*`. This glob engine support same syntax. If
//! `]*` sequence is found it will not be translated in any way.
//! * Capture groups are exposed in an API but have no special pattern syntax.
//! * POSIX ERE support `\` as escape character. Glob also support is so no
//!   preprocessing is needed here. But this preprocessor supports its own escape
//!   character too - `#`. Next character after it will be literally copied into
//!   output regex pattern.
//!
//! These characters are escaped with `\` by default in generated regex:
//!
//! * `(`.
//! * `)`.
//! * `{`.
//! * `}`.
//! * `^`.
//! * `$`.
//! * `.`.
//! * `|`.
//! * `+`.
//!
//! ::: note
//! By default pattern is wrapped in `^...$` to match full string.
//! :::

#include "Common.h"

/// Glob context.
///
/// * [fl:data] Data for dynamic array of matches.
///   - [fl:data.res] Match text. Points to original data.
///   - [fl:data.res_idx] Match index.
///   - [fl:data.captures] Copy of capture groups for that match.
/// * [fl:capacity] Capacity for dynamic array of matches.
/// * [fl:size] Count of elements for dynamic array of matches.
/// * [fl:regex] Cache for compiled regex.
/// * [fl:captures] Scratchpad captures for regex engine.
typedef struct cbuild_glob_t {
	struct __cbuild_glob_res_t {
		const char* res;
		size_t res_idx;
		regmatch_t captures[CBUILD_GLOB_CAPTURE_COUNT];
	} *data;
	size_t size;
	size_t capacity;
	regex_t regex;
	regmatch_t captures[CBUILD_GLOB_CAPTURE_COUNT];
} cbuild_glob_t;
/// Options for pattern compilation
///
/// * [fl:partial_match] Do not wrap pattern in `^...$`.
struct cbuild_glob_opts_t {
	bool partial_match;
};
/// Compile new glob patter.
///
/// This is a two-step process, first it will transform glob expression
/// into a regx one and then it will call `regcomp`. POSIX ERE will be used.
CBUILDDEF bool cbuild_glob_compile_opt(cbuild_glob_t* glob, const char* pattern,
	struct cbuild_glob_opts_t opts);
/// Compile new glob patter.
///
/// This is a two-step process, first it will transform glob expression
/// into a regx one and then it will call `regcomp`. POSIX ERE will be used.
///
/// * [pl:glob:cbuild_glob_t*] Glob object.
/// * [pl:pattern:const char*] Pattern to compile.
/// * [pl:...:...cbulid_glob_opts_t] Fields of configuration structure in initializer-list form.
#define cbuild_glob_compile(glob, pattern, ...)                                   \
cbuild_glob_compile_opt(glob, pattern, (struct cbuild_glob_opts_t){ __VA_ARGS__ })
/// Find matches in a list.
///
/// [r:] `true` if at least one match was found. `false` if no matches were found.
CBUILDDEF bool cbuild_glob_match(cbuild_glob_t* glob, const char** list, size_t size);
/// Match single element with a glob.
///
/// This function resets glob matches but does not set them. You should check
/// return value to know if glob matched. To get capture groups values you can
/// use outer `captures` array. 
///
/// [r:] `true` if at least one match was found. `false` if no matches were found.
CBUILDDEF bool cbuild_glob_match_single(cbuild_glob_t* glob, const char* elem);
/// Clean underlying regex and free matches array.
CBUILDDEF void cbuild_glob_free(cbuild_glob_t* glob);
