#pragma once // For LSP
//! Parser for command-line flags with GNU-style format.
//!
//! # Features
//!
//! - Support for long and short options.
//! - Support for long option aliases.
//! - Support for adding `--` to list of long options (for custom parsing semantics).
//! - It support *terminated* arguments - argument list ends not only when size is
//!   reached (or when argument starts from `-`) but only when terminator reached.
//! - It support optional arguments.
//! - Flag grouping (to pretty-print them for help).
//! - Help formatter.
//! - Custom help and version hooks (parsing for that flags are built-in).
//!
//! # Limitations
//! 
//! - Short options can be only 1 character.
//! - Arguments are always returned as strings.

#include "Common.h"

/// Options for a new flag
///
/// * [fl:short_option] Character for short options. If `'\0'` (`0`) then disabled.
/// * [fl:optional_arg] Arguments to this flag are optional. This augments `num_arguments` field by allowing another option of passing no arguments (in addition to one provided by `num_arguments`).
/// * [fl:num_arguments] Number of arguments required. `-1` means `>0`.
/// * [fl:group_name] Name of arguments group. Used only for help message. Can be `NULL`{.c}.
/// * [fl:terminator] Terminator argument. Can be `NULL`{.c} (unset, non-terminated argument list).
/// * [fl:desc] Description. Can be `NULL`{.c}, then nothing will be printed.
struct cbuild_flag_new_opts_t {
	char short_option;
	bool optional_arg;
	// char __padding[2];
	int num_arguments;
	// char __padding[4];
	const char* group_name;
	const char* terminator;
	const char* desc;
};

/// Register new flag. Semi-internal function.
CBUILDDEF void cbuild_flag_new_opt(const char* option, struct cbuild_flag_new_opts_t opts);
//// Register a new flag.
///
/// * [pl:option:const char*] Long flag name. Will be used as its internal "ID'.
/// * [pl:...:...cbuild_flag_new_opts_t] Fields of configuration structure in initializer-list form.
#define cbuild_flag_new(option, ...)                                        \
cbuild_flag_new_opt(option, (struct cbuild_flag_new_opts_t){ __VA_ARGS__ })
/// Options for a flag parser
///
/// * [fl:CBUILD_FLAG_PASS_SEPARATOR] Pass `--` inside of pargs. No arguments.
/// * [fl:CBUILD_FLAG_ADD_ALIAS] Add new alias for a flag. Arguments are flag id (const char*) and NULL-terminated sequence of aliases (const char*).
/// * [fl:CBUILD_FLAG_SET_GROUP_DESCRIPTION] Set description for a flag group. Arguments are group ID (const char*) and description string (const char*)
/// * [fl:CBUILD_FLAG_HELP_HOOK] Set hook for `--help`. Argument - [`cbuild_flag_print_func_t`](#cbuild_flag_print_func_t)
/// * [fl:CBUILD_FLAG_VERSION_HOOK] Set hook for `--version`. Argument - [`cbuild_flag_print_func_t`](#cbuild_flag_print_func_t)
enum cbuild_flag_options_t {
	CBUILD_FLAG_PASS_SEPARATOR,
	CBUILD_FLAG_ADD_ALIAS,
	CBUILD_FLAG_SET_GROUP_DESCRIPTION,
	CBUILD_FLAG_HELP_HOOK,
	CBUILD_FLAG_VERSION_HOOK,
};
/// Function used for print-type hooks. Takes `argv[0]`{.c} as input.
typedef void (*cbuild_flag_print_func_t)(const char* app_name);
/// Set some option for a flag parser.
///
/// ::: note
/// Options can take some flags, they are passed via variadic arguments.
/// :::
CBUILDDEF void cbuild_flag_set_option(enum cbuild_flag_options_t option, ...);
/// List of arguments
typedef struct cbuild_arglist_t {
	char** data;
	size_t size;
	size_t capacity;
} cbuild_arglist_t;
/// Get element from argument list using its index.
///
/// * [pl:arglist:const cbuild_arglist_t*] Argument list.
/// * [pl:idx:size_t] Element index.
/// 
/// [r:char**] Pointer to an element or `NULL`{.c} on overflow.
#define cbuild_arglist_get(arglist, idx) cbuild_da_get(arglist, idx)
/// Parse flags.
///
/// ::: note
/// This function prints error and aborts on invalid flags and when parsed 
/// argument count differs from required (uses `exit(1)`).
/// :::
CBUILDDEF void cbuild_flag_parse(int argc, char** argv);
/// Print help for all flags via [`__CBUILD_PRINT`](Common.html#preprocessor-configuration)
CBUILDDEF void cbuild_flag_print_help(void);
/// Get list of positional arguments.
///
/// [r:] List of positional arguments (pointer to a global state).
CBUILDDEF cbuild_arglist_t* cbuild_flag_get_pargs(void);
/// Get arguments for a specific flag.
///
/// [r:] List of arguments for that flag (pointer to a global state) or NULL if flag was not provided.
CBUILDDEF cbuild_arglist_t* cbuild_flag_get_flag(const char* opt);
/// Get current app name (`argv[0]`{.c}).
CBUILDDEF const char* cbuild_flag_app_name(void);
