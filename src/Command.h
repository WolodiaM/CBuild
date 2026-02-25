#pragma once // For LSP 
//! Command runner

#include "Common.h"
#include "StringBuilder.h"
#include "Proc.h"

/// Command structure - dynamic array of arguments.
///
/// ::: note
/// [f:data] has same format as `arg` in `execv` except NULL termination.
/// :::
typedef struct cbuild_cmd_t {
	const char** data;
	size_t size;
	size_t capacity;
} cbuild_cmd_t;
/// Append new argument to a command.
///
/// * [pl:cmd:cbuild_cmd_t*] Command object.
/// * [pl:val:const char*] Element to append.
#define	cbuild_cmd_append(cmd, val) cbuild_da_append(cmd, val)
/// Append new arguments to a command.
///
/// * [pl:cmd:cbuild_cmd_t*] Command object.
/// * [pl:vals:const char**] Elements to append.
/// * [pl:vals_cnt:size_t] Number of elements to append.
#define	cbuild_cmd_append_arr(cmd, vals, vals_cnt)                             \
	cbuild_da_append_arr(cmd, vals, vals_cnt)
/// Append new arguments to a command.
///
/// * [pl:cmd:cbuild_cmd_t*] Command object.
/// * [pl:...:const char*] Elements to append.
#define	cbuild_cmd_append_many(cmd, ...)                                       \
	cbuild_da_append_many(cmd, __VA_ARGS__)
/// Free command.
///
/// This should be used only if you want to free underlying memory, if you just
/// want to clear array and reuse it, setting `size` to `0` would be much faster.
///
/// * [pl:cmd:cbuild_cmd_t*] Dynamic array object.
#define	cbuild_cmd_clear(cmd) cbuild_da_clear(cmd)
/// Convert command to string builder.
///
/// ::: note
/// This function properly formats arguments with spaces by wrapping them in `'`.
/// But arguments that has both spaces and `'` are not handler properly.
/// :::
CBUILDDEF cbuild_sb_t cbuild_cmd_to_sb(cbuild_cmd_t cmd);
/// Convert command to string builder.
///
/// ::: note
/// This function properly formats arguments with spaces by wrapping them in `'`.
/// But arguments that has both spaces and `'` are not handler properly.
/// :::
#define	cbuild_sb_from_cmd(cmd) cbuild_cmd_to_sb(cmd)
/// Configuration for command.
///
/// This structure consists of a few separate blocks:
///
/// * Async - first two values are mutually exclusive:
///   - [fl:procs] Process list used for async execution. Non-`NULL`{.c} value enables async.
///   - [fl:proc] Process return value used for async execution. Non-`NULL`{.c} value enables async.
///   - [fl:async_threads] Control how many threads async runner can spawn (works in conjunction with [p:procs]). `0` means implementation-defined. `-1` means unlimited.
/// * IO redirection:
///   - [fl:fdstdin] Redirect `stdin` to some file descriptor.
///   - [fl:file_stdin] Redirect `stdin` to some file.
///   - [fl:fdstdout] Redirect `stdout` to some file descriptor.
///   - [fl:file_stdout] Redirect `stdout` to some file.
///   - [fl:fdstderr] Redirect `stderr` to some file descriptor.
///   - [fl:file_stderr] Redirect `stderr` to some file.
/// * Miscellaneous lags:
///   - [fl:no_reset] By default `size` filed of command is reset. This make it stay untouched.
///   - [fl:autokill] Works only on Linux. Automatically kills process if parent dies.
///   - [fl:no_print_cmd] By default command is printed as `TRACE` log. This flag disable this log message.
struct cbuild_cmd_opt_t {
	// Async
	cbuild_proclist_t* procs; // Non-null implies async
	cbuild_proc_t* proc;      // Non-null implies async
	// Redirect 'fdstd* ' and 'file_std*' is mutually exclusive.
	const cbuild_fd_t* fdstdin;
	const char* file_stdin;
	const cbuild_fd_t* fdstdout;
	const char* file_stdout;
	const cbuild_fd_t* fdstderr;
	const char* file_stderr;
	// Async
	int async_threads; // 0 means implementation-defined. -1 means unlimited. Expects 'procs' to be a valid empty array.
	// Flags
	union {
		uint32_t flags;
		struct {
			uint32_t no_reset     : 1;
			uint32_t autokill     : 1;
			uint32_t no_print_cmd : 1;
			uint32_t              : 29;
		};
	};
};
/// Run command. This function is semi-internal.
CBUILDDEF bool cbuild_cmd_run_opt(cbuild_cmd_t* cmd, struct cbuild_cmd_opt_t opts);
/// Run command.
///
/// * [pl:cmd:cbuild_cmd_t*] Command to execute.
/// * [pl:...:...cbuid_cmd_opt_t] Fields of configuration structure in initializer-list form.
#define cbuild_cmd_run(cmd, ...)                                               \
cbuild_cmd_run_opt(cmd, (struct cbuild_cmd_opt_t){ __VA_ARGS__ })
