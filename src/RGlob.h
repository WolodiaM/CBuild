#pragma once // For LSP
//! Glob syntax implementation over POSIX regex.

#include "Common.h"
#include "FS.h"

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
		size_t res_tdx;
		regmatch_t captures[CBUILD_GLOB_CAPTURE_COUNT];
	} *data;
	size_t size;
	size_t capacity;
	regex_t regex;
	regmatch_t captures[CBUILD_GLOB_CAPTURE_COUNT];
} cbuild_glob_t;
/// Compile new glob patter.
///
/// This is a two-step process, first it will transform glob expression
/// into a regx one and then it will call `regcomp`. POSIX ERE will be used.
CBUILDDEF bool cbuild_glob_compile(cbuild_glob_t* glob, const char* pattern);
/// Find matches in a list.
CBUILDDEF bool cbuild_glob_match(cbuild_glob_t* glob, const char** list, size_t size);
/// Find glob matches in file system.
///
/// This will match against full path and put full path in a resulting array.
///
/// * [fl:glob] Glob context.
/// * [fl:pathlist] This will be used to store full list of scanned paths.
/// * [fl:base_path] Base search path.
CBUILDDEF bool cbuild_glob_match_fs(cbuild_glob_t* glob, cbuild_pathlist_t* pathlist, const char* base_path);
/// Find glob matches in file system.
///
/// This will match against filename+extension only but put full path in a resulting array.
///
/// * [fl:glob] Glob context.
/// * [fl:pathlist] This will be used to store full list of scanned paths.
/// * [fl:base_path] Base search path.
CBUILDDEF bool cbuild_glob_match_files(cbuild_glob_t* glob, cbuild_pathlist_t* pathlist, const char* base_path);
/// Clean underlying regex and free matches array.
CBUILDDEF void cbuild_glob_free(cbuild_glob_t* glob);
