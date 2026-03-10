#include "FS.h"
#include "Common.h"
#include "StringBuilder.h"
#include "DynArray.h"
#include "Stack.h"
#include "Log.h"
#include "Temp.h"
#if defined(CBUILD_API_POSIX) || defined(CBUILD_API_STRICT_POSIX)
	CBUILDDEF bool cbuild_fd_close(cbuild_fd_t fd) {
		if(fd == CBUILD_INVALID_FD) {
			cbuild_log_error("Invalid file descriptor passed to close, error: \"%s\"",
				strerror(errno));
			return false;
		}
		return close(fd) == 0;
	}
	CBUILDDEF cbuild_fd_t cbuild_fd_open_read(const char* path) {
		cbuild_fd_t fd = open(path, O_RDONLY);
		if(fd < 0) {
			cbuild_log_error("Could not open file \"%s\" for reading, error: \"%s\"",
				path, strerror(errno));
			return CBUILD_INVALID_FD;
		}
		fcntl(fd, F_SETFD, FD_CLOEXEC);
		return fd;
	}
	CBUILDDEF cbuild_fd_t cbuild_fd_open_write(const char* path) {
		cbuild_fd_t fd = open(path, O_WRONLY | O_CREAT | O_TRUNC,
			S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		if(fd < 0) {
			cbuild_log_error("Could not open file \"%s\" for writing, error: \"%s\"",
				path, strerror(errno));
			return CBUILD_INVALID_FD;
		}
		fcntl(fd, F_SETFD, FD_CLOEXEC);
		return fd;
	}
	CBUILDDEF bool cbuild_fd_open_pipe(cbuild_fd_t* read, cbuild_fd_t* write) {
		cbuild_fd_t fds[2];
		int ret = pipe(fds);
		if(ret < 0) {
			cbuild_log_error("Could not create pipe, error: \"%s\"",
				strerror(errno));
			*read = CBUILD_INVALID_FD;
			*write = CBUILD_INVALID_FD;
			return false;
		}
		*read = fds[0];
		*write = fds[1];
		fcntl(*read, F_SETFD, FD_CLOEXEC);
		fcntl(*write, F_SETFD, FD_CLOEXEC);
		return true;
	}
	CBUILDDEF ssize_t cbuild_fd_read(cbuild_fd_t fd, void* buf, size_t nbytes) {
		ssize_t len = read(fd, buf, nbytes);
		if(len < 0) {
			cbuild_log_error("Could not execute 'cbuild_fd_read', error: \"%s\"",
				strerror(errno));
		}
		return len;
	}
	CBUILDDEF ssize_t cbuild_fd_read_file(cbuild_fd_t fd, void* buf,
		size_t nbytes, const char* path) {
		ssize_t len = read(fd, buf, nbytes);
		if(len < 0) {
			cbuild_log_error("Could not read from file \"%s\", error: \"%s\"",
				path, strerror(errno));
		}
		return len;
	}
	CBUILDDEF ssize_t cbuild_fd_write(cbuild_fd_t fd, const void* buf,
		size_t nbytes) {
		ssize_t len = write(fd, buf, nbytes);
		if(len < 0) {
			cbuild_log_error("Could not execute 'cbuild_fd_write', error: \"%s\"",
				strerror(errno));
		}
		return len;
	}
	CBUILDDEF ssize_t cbuild_fd_write_file(cbuild_fd_t fd, const void* buf,
		size_t nbytes, const char* path) {
		ssize_t len = write(fd, buf, nbytes);
		if(len < 0) {
			cbuild_log_error("Could not write to file \"%s\", error: \"%s\"",
				path, strerror(errno));
		}
		return len;
	}
	CBUILDDEF ssize_t cbuild_file_len(const char* path) {
		struct stat statbuff;
		if(stat(path, &statbuff) < 0) {
			cbuild_log_error("Could not stat \"%s\", error: \"%s\"", path,
				strerror(errno));
			return -1;
		}
		return statbuff.st_size;
	}
#endif // CBUILD_API_*
CBUILDDEF bool cbuild_file_read(const char* path, cbuild_sb_t* data) {
	cbuild_fd_t fd = cbuild_fd_open_read(path);
	if(CBUILD_INVALID_FD == fd) {
		return false;
	}
	size_t size = (size_t)cbuild_file_len(path);
	cbuild_sb_resize(data, size); // Assert data != NULL
	ssize_t len = cbuild_fd_read_file(fd, data->data, size, path);
	if(len < 0) {
		cbuild_fd_close(fd);
		return false;
	}
	data->size = (size_t)len;
	cbuild_fd_close(fd);
	return true;
}
CBUILDDEF bool cbuild_file_write(const char* path, cbuild_sb_t* data) {
	cbuild_fd_t fd = cbuild_fd_open_write(path);
	if(CBUILD_INVALID_FD == fd) {
		return false;
	}
	char* buf = data->data;
	ssize_t cnt = (ssize_t)data->size;
	while(cnt > 0) {
		ssize_t written = cbuild_fd_write_file(fd, buf, (size_t)cnt, path);
		if(written < 0) {
			cbuild_fd_close(fd);
			return false;
		}
		cnt -= written;
		buf += written;
	}
	cbuild_fd_close(fd);
	return true;
}
CBUILDDEF bool cbuild_file_copy(const char* src, const char* dst) {
	cbuild_fd_t src_fd = cbuild_fd_open_read(src);
	cbuild_fd_t dst_fd = cbuild_fd_open_write(dst);
	if(src_fd == CBUILD_INVALID_FD) {
		return false;
	}
	if(dst_fd == CBUILD_INVALID_FD) {
		cbuild_fd_close(src_fd);
		return false;
	}
	char* tmp_buff = (char*)__CBUILD_MALLOC(CBUILD_FS_TMP_SIZE);
	cbuild_assert(tmp_buff != NULL, "Allocation failed.\n");
	while(true) {
		ssize_t cnt = cbuild_fd_read_file(src_fd, tmp_buff, CBUILD_FS_TMP_SIZE, src);
		if(cnt == 0) {
			break;
		}
		if(cnt < 0) {
			cbuild_fd_close(src_fd);
			cbuild_fd_close(dst_fd);
			__CBUILD_FREE(tmp_buff);
			return false;
		}
		char* buf = tmp_buff;
		while(cnt > 0) {
			ssize_t written = cbuild_fd_write_file(dst_fd, buf, (size_t)cnt, dst);
			if(written < 0) {
				cbuild_fd_close(src_fd);
				cbuild_fd_close(dst_fd);
				__CBUILD_FREE(tmp_buff);
				return false;
			}
			cnt -= written;
			buf += written;
		}
	}
	cbuild_fd_close(src_fd);
	cbuild_fd_close(dst_fd);
	__CBUILD_FREE(tmp_buff);
	return true;
}
#if defined(CBUILD_API_POSIX) || defined(CBUILD_API_STRICT_POSIX)
	CBUILDDEF bool cbuild_file_move(const char* src, const char* dst) {
		if(rename(src, dst) == 0) {
			return true;
		}
		if(errno == EXDEV) {
			if(!cbuild_file_copy(src, dst)) return false;
			return cbuild_file_remove(src);
		}
		return false;
	}
#endif // CBUILD_API_*
CBUILDDEF bool cbuild_file_check(const char* path) {
	struct stat statbuff;
	return stat(path, &statbuff) == 0;
}
#if defined(CBUILD_API_POSIX) || defined(CBUILD_API_STRICT_POSIX)
	CBUILDDEF bool cbuild_file_remove(const char* path) {
		if(unlink(path) < 0) {
			cbuild_log_error("Could not remove file \"%s\", error: \"%s\"",
				path, strerror(errno));
			return false;
		}
		return true;
	}
	CBUILDDEF bool cbuild_symlink(const char* src, const char* dst) {
		size_t checkpoint = cbuild_temp_checkpoint();
		char* base = cbuild_path_base(dst);
		if(*base) {
			if(!cbuild_dir_check(base)) {
				if(!cbuild_dir_create(base)) {
					cbuild_log_error("Destination base path \"%s\" is invalid!", base);
					cbuild_temp_reset(checkpoint);
					return false;
				}
			}
		}
		cbuild_temp_reset(checkpoint);
		int ret = symlink(src, dst);
		if(ret < 0) {
			if(errno == EEXIST) {
				cbuild_filetype_t type = cbuild_path_filetype(dst);
				switch(type) {
				case CBUILD_FTYPE_DIRECTORY: cbuild_dir_remove(dst); break;
				case CBUILD_FTYPE_REGULAR:
				case CBUILD_FTYPE_SYMLINK:
				case CBUILD_FTYPE_OTHER:
					cbuild_file_remove(dst);
					break;
				case CBUILD_FTYPE_MISSING:
					CBUILD_UNREACHABLE("File cannot be missing if it exists.");
					break;
				default: CBUILD_UNREACHABLE("Invalid filetype in create_symlink.");
				}
				if(symlink(src, dst) == 0) return true;
			}
			cbuild_log_error(
				"Could not create symbolic link \"%s\", error: \"%s\"",
				dst, strerror(errno));
			return false;
		}
		return true;
	}
#endif // CBUILD_API_*
CBUILDDEF bool cbuild_dir_copy(const char* src, const char* dst) {
	bool err = cbuild_dir_create(dst);
	if(err == false) {
		cbuild_log_error("Could not create destination directory \"%s\"",
			dst);
		return false;
	}
	cbuild_pathlist_t list = {0};
	err = cbuild_dir_list(src, &list);
	if(err == false) {
		cbuild_log_error("Could not list source directory \"%s\"", src);
		if(list.data != NULL) {
			cbuild_pathlist_clear(&list);
		}
		return false;
	}
	bool ret = true;
	size_t checkpoint = cbuild_temp_checkpoint();
	for(size_t i = 0; i < list.size; i++) {
		const char* tmpsrc = cbuild_temp_sprintf("%s/%s", src, list.data[i]);
		const char* tmpdst = cbuild_temp_sprintf("%s/%s", dst, list.data[i]);
		cbuild_filetype_t f = cbuild_path_filetype(tmpsrc);
		if (f == CBUILD_FTYPE_MISSING) {
			CBUILD_UNREACHABLE("cbuild_dir_list should not return invalid files.");
		} else if(f == CBUILD_FTYPE_DIRECTORY) {
			bool tmp = ret && cbuild_dir_copy(tmpsrc, tmpdst);
			ret = tmp;
		} else {
			bool tmp = ret && cbuild_file_copy(tmpsrc, tmpdst);
			ret = tmp;
		}
		cbuild_temp_reset(checkpoint);
	}
	cbuild_pathlist_clear(&list);
	return ret;
}
CBUILDDEF bool cbuild_dir_move(const char* src, const char* dst) {
	bool ret = cbuild_dir_copy(src, dst);
	if(ret == false) {
		return false;
	}
	return cbuild_dir_remove(src);
}
#if defined(CBUILD_API_POSIX) || defined(CBUILD_API_STRICT_POSIX)
	CBUILDDEF bool __cbuild_fs_rmdir(const char* path) {
		int stat = rmdir(path);
		if(stat < 0) {
			cbuild_log_error(
				"Could not remove directory directory \"%s\", error: \"%s\"",
				path, strerror(errno));
			return false;
		}
		return true;
	}
#endif // CBUILD_API_*
CBUILDDEF bool cbuild_dir_remove(const char* path) {
	cbuild_pathlist_t list = {0};
	bool err = cbuild_dir_list(path, &list);
	if(err == false) {
		cbuild_log_error(
			"Could not remove source directory \"%s\", error: \"%s\"", path,
			strerror(errno));
		if(list.data != NULL) {
			cbuild_pathlist_clear(&list);
		}
		return false;
	}
	bool ret = true;
	size_t checkpoint = cbuild_temp_checkpoint();
	for(size_t i = 0; i < list.size; i++) {
		const char* tmppath = cbuild_temp_sprintf("%s/%s", path, list.data[i]);
		cbuild_filetype_t f = cbuild_path_filetype(tmppath);
		if (f == CBUILD_FTYPE_MISSING) {
			CBUILD_UNREACHABLE("cbuild_dir_list should not return invalid files.");
		} else if(f == CBUILD_FTYPE_DIRECTORY) {
			if(!cbuild_dir_remove(tmppath)) ret = false;
		} else {
			if(!cbuild_file_remove(tmppath)) ret = false;
		}
		cbuild_temp_reset(checkpoint);
	}
	if(!__cbuild_fs_rmdir(path)) {
		cbuild_pathlist_clear(&list);
		return false;
	}
	cbuild_pathlist_clear(&list);
	return ret;
}
CBUILDDEF bool cbuild_dir_check(const char* path) {
	return cbuild_file_check(path);
}
CBUILDDEF int __cbuild_fs_compare(const void* a, const void* b) {
	return strcmp(*(const char**)a, *(const char**)b);
}
CBUILDDEF bool cbuild_dir_list(const char* path, cbuild_pathlist_t* elements) {
	cbuild_pathlist_clear(elements);
	cbuild_dir_t dir = cbuild_dir_open(path);
	if (dir == CBUILD_INVALID_DIR) return false;
	const char* file = NULL;
	int ret = 0;
	while ((ret = cbuild_dir_next(dir, &file)) == 0) {
		if (strcmp(file, ".")  == 0) continue;
		if (strcmp(file, "..")  == 0) continue;
		size_t len = strlen(file);
		char* file_copy = __CBUILD_MALLOC(len + 1);
		cbuild_assert(file_copy != NULL, "Allocation failed.\n");
		memcpy(file_copy, file, len);
		file_copy[len] = '\0';
		cbuild_da_append(elements, file_copy);
	}
	qsort(elements->data, elements->size, sizeof(char*), __cbuild_fs_compare);
	if (!cbuild_dir_close(dir)) return false;
	if (ret == -1) return false;
	return true;
}
#if defined(CBUILD_API_POSIX) || defined(CBUILD_API_STRICT_POSIX)
	CBUILDDEF cbuild_dir_t cbuild_dir_open(const char* path) {
		cbuild_dir_t dir = opendir(path);
		if (dir == CBUILD_INVALID_DIR) {
			cbuild_log_error("Failed to open directory '%s': %s",
				path, strerror(errno));
			return CBUILD_INVALID_DIR;
		}
		return dir;
	}
	CBUILDDEF int cbuild_dir_next(cbuild_dir_t dir, const char** element) {
		errno = 0;
		struct dirent* ent = readdir(dir);
		if (ent == NULL) {
			if (errno == 0) return 1;
			cbuild_log_error("Failed to get next element from directory: %s",
				strerror(errno));
			return -1;
		}
		*element = ent->d_name;
		return 0;
	}
	CBUILDDEF bool cbuild_dir_close(cbuild_dir_t dir) {
		if (closedir(dir) != 0) {
			cbuild_log_error("Failed to close directory: %s", strerror(errno));
			return false;
		}
		return true;
	}
#endif // CBUILD_API_*
CBUILDDEF bool __cbuild_dir_walk_opt(cbuild_sb_t* path, size_t level, bool* abort,
	cbuild_dir_walk_func_t func, struct cbuild_dir_walk_opts_t opts) {
	size_t curr_path_len = path->size;
	cbuild_sb_append_null(path);
	cbuild_filetype_t ftype = cbuild_path_filetype(path->data);
	if (!opts.visit_dir_last) {
		enum cbuild_dir_walk_result_t res = CBUILD_DIR_WALK_CONTINUE;
		struct cbuild_dir_walk_func_args_t args = {
			.path = path->data,
			.type = ftype,
			.level = level,
			.result = &res,
			.context = opts.context,
		};
		if (!func(args)) return false;
		switch (res) {
		case CBUILD_DIR_WALK_CONTINUE: break;
		case CBUILD_DIR_WALK_STOP: *abort = true; CBUILD_ATTR_FALLTHROUGH();
		case CBUILD_DIR_WALK_NO_ENTER: return true;
		}
	}
	if (ftype == CBUILD_FTYPE_DIRECTORY) {
		cbuild_dir_t dir = cbuild_dir_open(path->data);
		if (dir == CBUILD_INVALID_DIR) return false;
		const char* file = NULL;
		int ret = 0;
		while ((ret = cbuild_dir_next(dir, &file)) == 0) {
			if (strcmp(file, ".")  == 0) continue;
			if (strcmp(file, "..")  == 0) continue;
			path->size = curr_path_len;
			cbuild_sb_append(path, '/');
			cbuild_sb_append_cstr(path, file);
			if (!__cbuild_dir_walk_opt(path, level + 1, abort, func, opts)) {
				if (!cbuild_dir_close(dir)) return false;
				return false;
			}
			path->size = curr_path_len;
			if (*abort) {
				if (!cbuild_dir_close(dir)) return false;
				return true;
			}
		}
		if (!cbuild_dir_close(dir)) return false;
		if (ret == -1) return false;
	}
	if (opts.visit_dir_last) {
		enum cbuild_dir_walk_result_t res = CBUILD_DIR_WALK_CONTINUE;
		struct cbuild_dir_walk_func_args_t args = {
			.path = path->data,
			.type = ftype,
			.level = level,
			.result = &res,
			.context = opts.context,
		};
		if (!func(args)) return false;
		switch (res) {
		case CBUILD_DIR_WALK_CONTINUE: break;
		case CBUILD_DIR_WALK_STOP: *abort = true; CBUILD_ATTR_FALLTHROUGH();
		case CBUILD_DIR_WALK_NO_ENTER: return true;
		}
	}
	return true;
}
CBUILDDEF bool cbuild_dir_walk_opt(const char* path, cbuild_dir_walk_func_t func,
	struct cbuild_dir_walk_opts_t opts) {
	cbuild_sb_t tmp_path = {0};
	cbuild_sb_append_cstr(&tmp_path, path);
	bool abort = false;
	bool ret = __cbuild_dir_walk_opt(&tmp_path, 0, &abort, func, opts);
	cbuild_sb_clear(&tmp_path);
	return ret;
}
#if defined(CBUILD_API_POSIX) || defined(CBUILD_API_STRICT_POSIX)
	CBUILDDEF char* cbuild_dir_current(void) {
		#if defined(CBUILD_API_POSIX) && \
			(defined(CBUILD_OS_LINUX_GLIBC) || defined(CBUILD_OS_LINUX_MUSL))
			char* tmp = get_current_dir_name();
			size_t len = strlen(tmp);
			char* ret = __CBUILD_MALLOC(len + 1);
			memcpy(ret, tmp, len + 1);
			free(tmp);
			return ret;
		#else
			size_t size;
			long path_max = pathconf(".", _PC_PATH_MAX);
			if (path_max == -1) size = 1024;
			else if (path_max > 10240) size = 10240;
			else size = (size_t)path_max;
			while(true) {
				char* buf = __CBUILD_MALLOC(size);
				cbuild_assert(buf != NULL, "Allocation failed.\n");
				errno = 0;
				if(getcwd(buf, size) == NULL) {
					if(errno == ERANGE) {
						size = size*2;
					} else {
						free(buf);
						return NULL;
					}
				} else {
					return buf;
				}
			}
			return NULL;
		#endif // Extension check
	}
	CBUILDDEF bool cbuild_dir_set_current(const char* path) {
		if(chdir(path) < 0) {
			cbuild_log_error(
				"Could not set current working directory to \"%s\", error: \"%s\"",
				path, strerror(errno));
			return false;
		}
		return true;
	}
#endif // CBUILD_API_*
CBUILDDEF void cbuild_pathlist_clear(cbuild_pathlist_t* list) {
	for(size_t i = 0; i < list->size; i++) {
		__CBUILD_FREE((void*)list->data[i]);
	}
	cbuild_da_clear(list);
}
#if defined(CBUILD_API_POSIX) || defined(CBUILD_API_STRICT_POSIX)
	CBUILDDEF bool __cbuild_dir_create(const char* path_, bool inplace) {
		int ret = mkdir(path_, 0755);
		if(ret < 0) {
			if(errno == EEXIST) {
				cbuild_log(CBUILD_LOG_WARN, "Directory \"%s\" exist", path_);
				return false;
			} else if(errno == ENOENT) {
				char* path = (char*)path_;
				size_t checkpoint = cbuild_temp_checkpoint();
				if(!inplace) path = cbuild_path_normalize(path_);
				cbuild_log_level_t old_log_level = cbuild_log_get_min_level();
				if(!inplace) cbuild_log_set_min_level(CBUILD_LOG_ERROR);
				char* slash = strrchr(path, '/');
				if(slash) *slash = '\0';
				if(!__cbuild_dir_create(path, true)) {
					if(!inplace) {
						cbuild_temp_reset(checkpoint);
						cbuild_log_set_min_level(old_log_level);
					}
					return false;
				}
				if(slash) *slash = '/';
				ret = mkdir(path, 0755);
				if(ret == 0) {
					if(!inplace) {
						cbuild_temp_reset(checkpoint);
						cbuild_log_set_min_level(old_log_level);
					}
					return true;
				}
				if(!inplace) {
					cbuild_temp_reset(checkpoint);
					cbuild_log_set_min_level(old_log_level);
				}
			}
			cbuild_log_error(
				"Could not create directory \"%s\", error: \"%s\"", path_,
				strerror(errno));
			return false;
		}
		return true;
	}
	CBUILDDEF bool cbuild_dir_create(const char* path) {
		return __cbuild_dir_create(path, false);
	}
	CBUILDDEF cbuild_filetype_t cbuild_path_filetype(const char* path) {
		struct stat statbuff;
		if(lstat(path, &statbuff) < 0) {
			cbuild_log_error("Could not stat file \"%s\", error: \"%s\"", path,
				strerror(errno));
			return CBUILD_FTYPE_MISSING;
		}
		if(S_ISREG(statbuff.st_mode)) {
			return CBUILD_FTYPE_REGULAR;
		}
		if(S_ISDIR(statbuff.st_mode)) {
			return CBUILD_FTYPE_DIRECTORY;
		}
		if(S_ISLNK(statbuff.st_mode)) {
			return CBUILD_FTYPE_SYMLINK;
		}
		return CBUILD_FTYPE_OTHER;
	}
#endif // CBUILD_API_*
CBUILDDEF char* cbuild_path_ext_opt(const char* path, struct cbuild_path_ext_opts_t opts) {
	if (opts.dot == 0) opts.dot = 1;
	const char* dot = path + strlen(path);
	do {
		dot = __cbuild_memrchr(path, '.', (size_t)(dot - path));
		opts.dot--;
	} while (dot != NULL && opts.dot > 0);
	if(dot == NULL) {
		char* ret = cbuild_temp_malloc(1);
		cbuild_assert(ret != NULL, "Allocation failed.\n");
		memcpy(ret, "\0", 1);
		return ret;
	}
	ptrdiff_t i = dot - path;
	size_t len = strlen(path) - (size_t)i;
	char* ret = cbuild_temp_malloc(len);
	cbuild_assert(ret != NULL, "Allocation failed.\n");
	memcpy(ret, path + i + 1, len);
	ret[len - 1] = '\0';
	return ret;
}
CBUILDDEF char* cbuild_path_name(const char* path) {
	size_t i = strlen(path);
	if(path[i - 1] == '/') {
		i -= 2;
	}
	const char* slash = __cbuild_memrchr(path, '/', i);
	if (slash == NULL) {
		char* ret = cbuild_temp_malloc(i + 1);
		cbuild_assert(ret != NULL, "Allocation failed.\n");
		memcpy(ret, path, i);
		ret[i] = '\0';
		return ret;
	}
	ptrdiff_t base = slash - path;
	size_t len = strlen(path) - (size_t)base;
	char* ret = cbuild_temp_malloc(len);
	cbuild_assert(ret != NULL, "Allocation failed.\n");
	memcpy(ret, path + base + 1, len);
	ret[len - 1] = '\0';
	return ret;
}
CBUILDDEF char* cbuild_path_base(const char* path) {
	size_t i = strlen(path);
	if(path[i - 1] == '/') {
		i -= 2;
	}
	const char* slash = __cbuild_memrchr(path, '/', i);
	if(slash == NULL) {
		char* ret = cbuild_temp_malloc(1);
		cbuild_assert(ret != NULL, "Allocation failed.\n");
		memcpy(ret, "\0", 1);
		return ret;
	}
	slash++;
	size_t len = (size_t)(slash - path) + 1;
	char* ret = cbuild_temp_malloc(len);
	cbuild_assert(ret != NULL, "Allocation failed.\n");
	memcpy(ret, path, len - 1);
	ret[len - 1] = '\0';
	return ret;
}
typedef struct __cbuild_stack_cstr_t {
	cbuild_sv_t* data;
	size_t ptr;
	size_t capacity;
} __cbuild_stack_cstr_t;
CBUILDDEF char* cbuild_path_normalize(const char* path_) {
	cbuild_sb_t buff = {0};
	__cbuild_stack_cstr_t dirs = {0};
	cbuild_sv_t path = cbuild_sv_from_cstr(path_);
	// Windows paths can have drive letter
	// Drive letter is only one character
	if (isalpha(path.data[0]) && path.data[1] == ':') {
		cbuild_sb_append_arr(&buff, path.data, 2);
		path.data += 2;
		path.size -= 2;
	}
	if(*path.data == '/') {
		cbuild_sb_append(&buff, '/');
	}
	// Unix paths threat double slash differently
	// POSIX threats paths starting with '//' specially.
	if(cbuild_sv_prefix(path, cbuild_sv_from_lit("//")) &&
		!cbuild_sv_prefix(path, cbuild_sv_from_lit("///"))) {
		cbuild_sb_append(&buff, '/');
	}
	do {
		cbuild_sv_t dir = cbuild_sv_chop_by_delim(&path, '/');
		if(dir.size == 0) continue;
		if(cbuild_sv_cmp(dir, cbuild_sv_from_lit(".")) == 0) {
			// Do nothing
		} else if(cbuild_sv_cmp(dir, cbuild_sv_from_lit("..")) == 0) {
			if(dirs.ptr == 0) { // Underflow
				cbuild_sb_append_cstr(&buff, "../");
				// Underflow on absolute path is undefined anyway
				// and on relative path we can guarantee that this will be fine
				// (we have nothing in directory stack anyway)
			} else {
				cbuild_stack_pop(&dirs);
			}
		} else {
			cbuild_stack_push(&dirs, dir);
		}
	} while(path.size > 0);
	for(size_t i = 0; i <	dirs.ptr; i++) {
		cbuild_sb_appendf(&buff, CBuildSVFmt"/", CBuildSVArg(dirs.data[i]));
	}
	if(buff.size == 0) cbuild_sb_append(&buff, '.');
	if(!((buff.size == 1 && buff.data[0] == '/') ||
			(buff.size == 2 && buff.data[0] == '/' && buff.data[1] == '/') ||
			(buff.size == 3 && isalpha(buff.data[0]) && 
				buff.data[1] == ':' && buff.data[2] == '/')) &&
		(buff.data[buff.size - 1] == '/')) buff.size--;
	cbuild_stack_clear(&dirs);
	char* ret = cbuild_temp_sprintf(CBuildSBFmt, CBuildSBArg(buff));
	cbuild_sb_clear(&buff);
	return ret;
}
