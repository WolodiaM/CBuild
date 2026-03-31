#pragma once // For LSP 
//! File system access abstraction layer.
//!
//! License: `GPL-3.0-or-later`.

#include "Common.h"
#include "StringBuilder.h"

/// Dynamic array of paths
typedef struct cbuild_pathlist_t {
	char** data;
	size_t size;
	size_t capacity;
} cbuild_pathlist_t;
/// Get element from list of paths using its index.
///
/// * [pl:list:const cbuild_pathlist_t*] Path list object.
/// * [pl:idx:size_t] Element index.
/// 
/// [r:char**] Pointer to an element or `NULL`{.c} if index out of bounds.
#define cbuild_pathlist_get(list, idx) cbuild_da_get(list, idx)
/// Free pathlist.
/// 
/// ::: note
/// This function should *really* be used instead of simple `free(list->data)`{.c}
/// because pathlist allocated memory in two levels.
/// :::
///
/// * [pl:list:const cbuild_pathlist_t*] Path list object.
CBUILDDEF void cbuild_pathlist_clear(cbuild_pathlist_t* list);
/// Type of file. Mostly cross-platform because no special Unix files are included.
typedef enum {
	CBUILD_FTYPE_MISSING   = -1,
	CBUILD_FTYPE_REGULAR   = 0,
	CBUILD_FTYPE_DIRECTORY = 1,
	CBUILD_FTYPE_SYMLINK   = 2,
	CBUILD_FTYPE_OTHER     = 3,
} cbuild_filetype_t;
/// Close file descriptor.
CBUILDDEF bool cbuild_fd_close(cbuild_fd_t fd);
/// Open file for reading.
CBUILDDEF cbuild_fd_t cbuild_fd_open_read(const char* path);
/// Open file for writing.
CBUILDDEF cbuild_fd_t cbuild_fd_open_write(const char* path);
/// Create and open new pipe. File descriptors for both ends are provided.
CBUILDDEF bool cbuild_fd_open_pipe(cbuild_fd_t* read, cbuild_fd_t* write);
/// POSIX `read`{.c} wrapper.
///
/// * [pl:fd] File descriptor.
/// * [pl:buf] Buffer in which data will be read.
/// * [pl:nbytes] Number of bytes to read.
///
/// [r:] Number of bytes read or `-1`{.c} on error.
CBUILDDEF ssize_t cbuild_fd_read(cbuild_fd_t fd, void* buf, size_t nbytes);
/// POSIX `read`{.c} "wrapper".
/// This function uses [p:path] only for error-reporting.
///
/// * [pl:fd] File descriptor.
/// * [pl:buf] Buffer in which data will be read.
/// * [pl:nbytes] Number of bytes to read.
/// * [pl:path] File name used in error reporting.
///
/// [r:] Number of bytes read or `-1`{.c} on error.
CBUILDDEF ssize_t cbuild_fd_read_file(cbuild_fd_t fd, void* buf, size_t nbytes,
	const char* path);
/// POSIX `write`{.c} wrapper.
///
/// * [pl:fd] File descriptor.
/// * [pl:buf] Buffer from which data will be written.
/// * [pl:nbytes] Number of bytes to write.
///
/// [r:] Number of bytes written or `-1`{.c} on error.
CBUILDDEF ssize_t cbuild_fd_write(cbuild_fd_t fd, const void* buf, size_t nbytes);
/// POSIX `write`{.c} "wrapper".
/// This function uses [p:path] only for error-reporting.
///
/// * [pl:fd] File descriptor.
/// * [pl:buf] Buffer from which data will be written.
/// * [pl:nbytes] Number of bytes to write.
/// * [pl:path] File name used in error reporting.
///
/// [r:] Number of bytes written or `-1`{.c} on error.
CBUILDDEF ssize_t cbuild_fd_write_file(cbuild_fd_t fd, const void* buf,
	size_t nbytes, const char* path);
/// Get length of file in bytes.
///
/// [r:] Length of file in bytes or `-1` on error.
CBUILDDEF ssize_t cbuild_file_len(const char* path);
/// Read file into string builder.
CBUILDDEF bool cbuild_file_read(const char* path, cbuild_sb_t* data);
/// Overwrite file with string builder content.
CBUILDDEF bool cbuild_file_write(const char* path, cbuild_sb_t* data);
/// Copy file.
/// 
/// ::: note
/// This copies file by reading from [p:src] and writing to [p:dst].
/// "Chunked" copy is used to limit memory usage.
/// :::
CBUILDDEF bool cbuild_file_copy(const char* src, const char* dst);
/// Move file.
///
/// Uses optimal method for moving file, but in worst case falls back
/// to `cbuild_file_copy(src, dst); cbuild_file_remove(src)`{.c}.
CBUILDDEF bool cbuild_file_move(const char* src, const char* dst);
/// Rename file. Alias to [`cbuild_file_move`](#ID_cbuild_file_move).
///
/// Uses optimal method for moving file, but in worst case falls back
/// to `cbuild_file_copy(src, dst); cbuild_file_remove(src)`{.c}.
///
/// * [pl:src:const char*]
/// * [pl:dst:const char*]
///
/// [r:bool]
#define cbuild_file_rename(src, dst) cbuild_file_move(src, dst)
/// Check if file exists.
CBUILDDEF bool cbuild_file_check(const char* path);
/// Delete file.
CBUILDDEF bool cbuild_file_remove(const char* path);
/// Create symbolic link. Will overwrite [p:dst] if it exists.
CBUILDDEF bool cbuild_symlink(const char* src, const char* dst);
/// Recursively copy a directory.
CBUILDDEF bool cbuild_dir_copy(const char* src, const char* dst);
/// Move a directory.
///
/// Uses optimal method for moving directory, but in worst case falls back
/// to `cbuild_dir_copy(src, dst); cbuild_dir_remove(src)`{.c}.
CBUILDDEF bool cbuild_dir_move(const char* src, const char* dst);
/// Rename a directory. Alias to [`cbuild_dir_move`](#ID_cbuild_dir_move).
///
/// Uses optimal method for moving directory, but in worst case falls back
/// to `cbuild_dir_copy(src, dst); cbuild_dir_remove(src)`{.c}.
///
/// * [pl:src:const char*]
/// * [pl:dst:const char*]
///
/// [r:bool]
#define cbuild_dir_rename(src, dst) cbuild_dir_move(src, dst)
/// Delete directory.
CBUILDDEF bool cbuild_dir_remove(const char* path);
/// Check if directory exists.
CBUILDDEF bool cbuild_dir_check(const char* path);
/// List all files in directory.
///
/// * [pl:path] Path to a directory.
/// * [pl:elements] List of elements in directory.
CBUILDDEF bool cbuild_dir_list(const char* path, cbuild_pathlist_t* elements);
/// Get current working directory.
///
/// [r:] Current working directory values. Allocated `__CBUILD_MALLOC`.
CBUILDDEF char* cbuild_dir_current(void);
/// Set current working directory.
CBUILDDEF bool cbuild_dir_set_current(const char* path);
/// Create new directory.
CBUILDDEF bool cbuild_dir_create(const char* path);
/// Result for [`cbuild_dir_walk_func_t`](#ID_cbuild_dir_walk_func_t).
///
/// * [fl:CBUILD_DIR_WALK_CONTINUE] Default value. Just continue.
/// * [fl:CBUILD_DIR_WALK_NO_ENTER] Do not enter into a directory.
/// * [fl:CBUILD_DIR_WALK_STOP] Stop entirely (but does not return an error).
enum cbuild_dir_walk_result_t {
	CBUILD_DIR_WALK_CONTINUE = 0,
	CBUILD_DIR_WALK_NO_ENTER,
	CBUILD_DIR_WALK_STOP = -1,
};
/// Arguments for [`cbuild_dir_walk_func_t`](#ID_cbuild_dir_walk_func_t).
///
/// This structure can be divided into multiple sections:
///
/// * Current entry:
///   - [fl:path] Path to the file system object object (relative and including directory passed to [`cbuild_dir_walk`](#ID_cbuild_dir_walk).
///   - [fl:type] Type of current file-system object. 
/// * Iteration state:
///   - [fl:level] How deep iteration goes (`0` means base directory).
/// * Context:
///   - [fl:context] Some arguments passed by user. 
///   - [fl:result] Return value.
typedef struct cbuild_dir_walk_func_args_t {
	const char* path;
	cbuild_filetype_t type;
	size_t level;
	enum cbuild_dir_walk_result_t* result;
	void* context;
} cbuild_dir_walk_func_args_t;
/// Function used as callback by directory walker.
///
/// [r:] `false` means error.
typedef bool (*cbuild_dir_walk_func_t)(cbuild_dir_walk_func_args_t args);
/// Arguments to a directory walker.
///
/// * [fl:context] Some arguments that will be passed to callback.
/// * [fl:visit_dir_last] Inverts order. Now all child files are visited first and parent directory is visited last.
struct cbuild_dir_walk_opts_t {
	void* context;
	bool visit_dir_last;
};
/// Walk over dir. Takes path, callback and some optional options. Semi-internal function.
CBUILDDEF bool cbuild_dir_walk_opt(const char* path, cbuild_dir_walk_func_t func,
	struct cbuild_dir_walk_opts_t opts);
/// Walk over dir.
///
/// * [pl:path:const char*] Path to a file.
/// * [pl:callback:cbuild_dir_walk_func_t] Function that will be called for each file system object.
/// * [pl:...:...cbuild_dir_walk_opts_t] Fields of configuration structure in initializer-list form.
#define cbuild_dir_walk(path, callback, ...)                                        \
cbuild_dir_walk_opt(path, callback, (struct cbuild_dir_walk_opts_t){ __VA_ARGS__ })
/// Open directory.
CBUILDDEF cbuild_dir_t cbuild_dir_open(const char* path);
/// Get next element from directory.
/// 
/// * [pl:dir] Directory handle.
/// * [pl:element] Return value for filename.
///
/// [r:0] No error and directory iterator is still not empty.
/// [r:1] No error but directory iterator is empty. [p:element] is `NULL`.
/// [r:-1] Error happened.
CBUILDDEF int cbuild_dir_next(cbuild_dir_t dir, const char** element);
/// Close directory.
CBUILDDEF bool cbuild_dir_close(cbuild_dir_t dir);
/// Get type of file.
CBUILDDEF cbuild_filetype_t cbuild_path_filetype(const char* path);
/// Optional arguments for [`cbuild_path_ext`](#ID_cbuild_path_ext).
///
/// * [fl:dot] Get extensions after n-th dot from the end. `0` is default value
/// (for now it is same as `1`).
struct cbuild_path_ext_opts_t {
	int dot; // Get n-th dot from the end, 0 is last one (same as 1)
};
/// Get extension of specific file. Semi-internal.
/// File pointer by [p:path] is not checked in any way.
///
/// [r:] String allocated via [`cbuild_temp_malloc`](Temp.html#ID_cbuild_temp_malloc).
CBUILDDEF char* cbuild_path_ext_opt(const char* path, struct cbuild_path_ext_opts_t opts);
/// Get extension of specific file.
/// File pointer by [p:path:const char*] is not checked in any way.
///
/// * [pl:path:const char*] Path to a file.
/// * [pl:...:...cbuild_path_ext_opts_t] Fields of configuration structure in initializer-list form.
///
/// [r:] String allocated via [`cbuild_temp_malloc`](Temp.html#ID_cbuild_temp_malloc).
#define cbuild_path_ext(path, ...)                                    \
cbuild_path_ext_opt(path, (struct cbuild_path_ext_opts_t){ __VA_ARGS__ })
/// Get file name with extension (all after last `/`).
/// For directories special case exists that strips last `/` if it is last character.
/// File pointer by [p:path] is not checked in any way.
/// 
/// [r:] String allocated via [`cbuild_temp_malloc`](Temp.html#ID_cbuild_temp_malloc).
CBUILDDEF char* cbuild_path_name(const char* path);
/// Get base name (all before last `/`).
/// For directories special case exists that strips last `/` if it is last character.
/// File pointer by [p:path] is not checked in any way.
///
/// [r:] String allocated via [`cbuild_temp_malloc`](Temp.html#ID_cbuild_temp_malloc).
CBUILDDEF char* cbuild_path_base(const char* path);
/// Normalize path. All normalization is based on heuristics and does not involve
/// such steps as eg. symlink resolving.
/// File pointer by [p:path] is not checked in any way.
///
/// [r:] String allocated via [`cbuild_temp_malloc`](Temp.html#ID_cbuild_temp_malloc).
CBUILDDEF char* cbuild_path_normalize(const char* path);
