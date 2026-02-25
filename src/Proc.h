#pragma once // For LSP 
//! Abstractions for process control.

#include "Common.h"

/// Dynamic array of processes.
typedef struct cbuild_proclist_t {
	cbuild_proc_t* data;
	size_t size;
	size_t capacity;
} cbuild_proclist_t;
/// Append new element to list of processes.
///
/// * [pl:proclist:cbuild_proclist_t*] Dynamic array object.
/// * [pl:proc:cbuild_proc_t] Process handle.
#define cbuild_proclist_append(proclist, proc) cbuild_da_append(proclist, proc)
/// Clears list of processes.
///
/// * [pl:proclist:cbuild_proclist_t*] Dynamic array object.
#define cbuild_proclist_clear(proclist) cbuild_da_clear(proclist)
/// Set element in process list using its index.
///
/// * [pl:proclist:cbuild_proclist_t*] List of processes.
/// * [pl:idx:size_t] Element index.
/// * [pl:proc:cbuild_proc_t] New element's value.
/// 
/// [r:bool] False on overflow.
#define cbuild_proclist_set(proclist, idx, proc) cbuild_da_set(proclist, idx, proc)
/// Get element from process list using its index.
///
/// * [pl:proclist:cbuild_proclist_t*] List of processes.
/// * [pl:idx:size_t] Slement index.
/// 
/// [r:cbuild_proc_t*] Pointer to an element or `NULL`{.c} on overflow.
#define cbuild_proclist_get(proclist, idx) cbuild_da_get(proclist, idx)
/// Blocking wait until specific process finishes.
///
/// [r:] `false`{.c} if process returned with an error.
CBUILDDEF bool cbuild_proc_wait(cbuild_proc_t proc);
/// Blocking wait until all processes from list exits.
///
/// [r:] `false`{.c} if at least one process returned with an error.
CBUILDDEF bool cbuild_procs_wait(cbuild_proclist_t procs);
/// Blocking wait until specific process finishes.
///
/// [r:] Process exit code or `INT_MIN`{.c} if [p:proc] is invalid or negative signal value if process exited due to a signal.
CBUILDDEF int cbuild_proc_wait_code(cbuild_proc_t proc);
/// Blocking wait until any process from list exits.
///
/// * [pl:procs] List of processess.
/// * [pl:code] Return value for exit code of exited process, Could be `NULL`{.c}.
///
/// [r:] Index of exited process in [p:procs] array.
CBUILDDEF ssize_t cbuild_procs_wait_any(cbuild_proclist_t procs, int* code);
/// Check if specific process is still running.
CBUILDDEF bool cbuild_proc_is_running(cbuild_proc_t proc);
/// Start a new process.
///
/// * [pl:func] Function that will be used as process' entry point. Function should return exit code.
/// * [pl:args] Arguments that will be passed to that function
///
/// [r:] Process handle of new process.
CBUILDDEF cbuild_proc_t cbuild_proc_start(int (*func)(void* args), void* args);
/// Pointer wrapper for shared memory.
///
/// * [fl:ptr] Pointer to a memory.
/// * [fl:size] Length of memory region in bytes.
typedef struct cbuild_proc_ptr_t {
	void*  ptr;
	size_t size;
} cbuild_proc_ptr_t;
/// Allocate memory that will be shared between child processes. Same as `malloc`.
CBUILDDEF cbuild_proc_ptr_t cbuild_proc_malloc(size_t n);
/// Free memory that was allocated via [`cbuild_proc_malloc`](#cbuild_proc_malloc).
/// Same as`free`.
CBUILDDEF void cbuild_proc_free(cbuild_proc_ptr_t ptr);
/// Get amount of CPU cores (or threads).
CBUILDDEF int cbuild_nproc(void);
