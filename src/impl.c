#include "Command.h"
#include "DynArray.h"
#include "Log.h"
#include "Proc.h"
#include "StringBuilder.h"
#include "StringView.h"
#include "Term.h"
#include "common.h"
/* misc code */
#if defined(CBUILD_OS_BSD) || defined(CBUILD_OS_LINUX) ||                      \
		defined(CBUILD_OS_WINDOWS_CYGWIN)
// GlibC, musl, BSD, Cygwin
extern const char* __progname;
#endif
const char* __cbuild_progname(void) {
#if defined(CBUILD_OS_BSD) || defined(CBUILD_OS_LINUX) ||                      \
		defined(CBUILD_OS_WINDOWS_CYGWIN)
	return __progname;
#elif defined(CBUILD_OS_MACOS)
	return getprogname();
#elif defined(CBUILD_OS_UNIX)
	// TODO: If not available leave this to user to resolve
	return __progname;
#endif
}
/* common.h impl */
void __cbuild_assert(const char* file, unsigned int line, const char* func,
                     const char* expr, ...) {
	printf("%s: %s:%u: %s: Assertion \"%s\" failed with message:\n",
	       __cbuild_progname(), file, line, func, expr);
	va_list args;
	va_start(args, expr);
	const char* fmt = va_arg(args, char*);
	vprintf(fmt, args);
	va_end(args);
	fflush(stdout);
	abort();
}
/* StringBuilder.h impl */
cbuild_da_t_impl(char, CBuildSBchar);
int cbuild_sb_cmp(cbuild_sb_t* a, cbuild_sb_t* b) {
	if (a->size < b->size) {
		return -2;
	}
	if (a->size > b->size) {
		return 2;
	}
	int ret = memcmp(a->data, b->data, a->size);
	if (ret == 0) {
		return 0;
	} else if (ret < 0) {
		return -1;
	} else if (ret > 0) {
		return 1;
	}
	return 0;
}
int cbuild_sb_cmp_icase(cbuild_sb_t* a, cbuild_sb_t* b) {
	if (a->size < b->size) {
		return -2;
	}
	if (a->size > b->size) {
		return 2;
	}
	for (size_t i = 0; i < a->size; i++) {
		char ac =
				'A' <= a->data[i] && a->data[i] <= 'Z' ? a->data[i] + 32 : a->data[i];
		char bc =
				'A' <= b->data[i] && b->data[i] <= 'Z' ? b->data[i] + 32 : b->data[i];
		int diff = ac - bc;
		if (diff < 0) {
			return -1;
		} else if (diff > 0) {
			return 1;
		}
	}
	return 0;
}
cbuild_sb_t cbuild_sv_to_sb(cbuild_sv_t sv) {
	cbuild_sb_t ret = CBuildStringBuilder;
	cbuild_sb_append_arr(&ret, sv.data, sv.size);
	return ret;
}
cbuild_sv_t cbuild_sb_to_sv(cbuild_sb_t* sb) {
	return (cbuild_sv_t){ .data = sb->data, .size = sb->size };
}
int cbuild_sb_vappendf(cbuild_sb_t* sb, const char* fmt, va_list args) {
	va_list args_copy;
	va_copy(args_copy, args);
	char buff[CBUILD_SB_QUICK_SPRINTF_SIZE];
	int  ret = vsnprintf(buff, CBUILD_SB_QUICK_SPRINTF_SIZE, fmt, args);
	if (ret < 0) {
		va_end(args_copy);
		return ret;
	}
	if ((size_t)ret >= CBUILD_SB_QUICK_SPRINTF_SIZE) {
		char* buff1 = __CBUILD_MALLOC(ret + 1);
		if (buff1 == NULL) {
			__CBUILD_ERR_PRINT("error: (LIB_CBUILD_SB) Allocation failed\n");
			exit(1);
		}
		vsnprintf(buff1, ret + 1, fmt, args_copy);
		cbuild_sb_append_cstr(sb, buff1);
	} else {
		cbuild_sb_append_cstr(sb, buff);
	}
	va_end(args_copy);
	return ret;
}
int cbuild_sb_appendf(cbuild_sb_t* sb, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	int ret = cbuild_sb_vappendf(sb, fmt, args);
	va_end(args);
	return ret;
}
/* StringView.h impl */
cbuild_sv_t cbuild_sv_from_parts(const char* data, size_t size) {
	return (cbuild_sv_t){ .data = (char*)data, .size = size };
}
cbuild_sv_t cbuild_sv_from_cstr(const char* cstr) {
	return (cbuild_sv_t){ .data = (char*)cstr, .size = strlen(cstr) };
}
size_t cbuild_sv_trim_left(cbuild_sv_t* sv) {
	size_t i = 0;
	while (i < sv->size && isspace(sv->data[i])) { i++; }
	sv->data += i;
	sv->size -= i;
	return i;
}
size_t cbuild_sv_trim_right(cbuild_sv_t* sv) {
	size_t i = sv->size;
	while (i > 0 && isspace(sv->data[i - 1])) { i--; }
	size_t tmp  = sv->size - i;
	sv->size   -= tmp;
	return tmp;
}
size_t cbuild_sv_trim(cbuild_sv_t* sv) {
	size_t ret  = cbuild_sv_trim_left(sv);
	ret        += cbuild_sv_trim_right(sv);
	return ret;
}
cbuild_sv_t cbuild_sv_chop(cbuild_sv_t* sv, size_t size) {
	if (size > sv->size) {
		size = sv->size;
	}
	char* tmp  = sv->data;
	sv->data  += size;
	sv->size  -= size;
	return cbuild_sv_from_parts(tmp, size);
}
bool cbuild_sv___chop_by_delim_delim(const cbuild_sv_t* sv, size_t idx,
                                     void* args) {
	return *((char*)args) == sv->data[idx];
}
cbuild_sv_t cbuild_sv_chop_by_delim(cbuild_sv_t* sv, char delim) {
	return cbuild_sv_chop_by_func(sv, cbuild_sv___chop_by_delim_delim, &delim);
}
bool cbuild_sv___chop_by_sv_delim1(const cbuild_sv_t* sv, size_t idx,
                                   void* args) {
	cbuild_sv_t* svt  = args;
	size_t       size = idx < svt->size ? idx : svt->size;
	return cbuild_sv_cmp(cbuild_sv_from_parts(sv->data + idx - (size - 1), size),
	                     *svt) == 0;
}
bool cbuild_sv___chop_by_sv_delim2(const cbuild_sv_t* sv, size_t idx,
                                   void* args) {
	cbuild_sv_t* svt  = args;
	size_t       size = (svt->size - 1) + idx > sv->size ? 0 : svt->size - 1;
	return cbuild_sv_cmp(cbuild_sv_from_parts(sv->data + idx, size),
	                     cbuild_sv_from_parts(svt->data, svt->size - 1)) == 0;
}
cbuild_sv_t cbuild_sv_chop_by_sv(cbuild_sv_t* sv, cbuild_sv_t delim) {
	if (delim.size == 1) {
		return cbuild_sv_chop_by_delim(sv, delim.data[0]);
	}
	cbuild_sv_t tmp =
			cbuild_sv_chop_by_func(sv, cbuild_sv___chop_by_sv_delim1, &delim);
	return cbuild_sv_chop_by_func(&tmp, cbuild_sv___chop_by_sv_delim2, &delim);
}
cbuild_sv_t cbuild_sv_chop_by_func(cbuild_sv_t* sv, cbuild_sv_delim_func delim,
                                   void* args) {
	size_t i = 0;
	while (i <= sv->size && !delim(sv, i, args)) { i++; }
	if (i >= sv->size) {
		return cbuild_sv_chop(sv, i);
	}
	char* tmp  = sv->data;
	sv->data  += i + 1;
	sv->size  -= i + 1;
	return cbuild_sv_from_parts(tmp, i);
}
int cbuild_sv_cmp(cbuild_sv_t a, cbuild_sv_t b) {
	if (a.size < b.size) {
		return -2;
	}
	if (a.size > b.size) {
		return 2;
	}
	int ret = memcmp(a.data, b.data, a.size);
	if (ret == 0) {
		return 0;
	} else if (ret < 0) {
		return -1;
	} else if (ret > 0) {
		return 1;
	}
	return 0;
}
int cbuild_sv_cmp_icase(cbuild_sv_t a, cbuild_sv_t b) {
	if (a.size < b.size) {
		return -2;
	}
	if (a.size > b.size) {
		return 2;
	}
	for (size_t i = 0; i < a.size; i++) {
		char ac = 'A' <= a.data[i] && a.data[i] <= 'Z' ? a.data[i] + 32 : a.data[i];
		char bc = 'A' <= b.data[i] && b.data[i] <= 'Z' ? b.data[i] + 32 : b.data[i];
		int  diff = ac - bc;
		if (diff < 0) {
			return -1;
		} else if (diff > 0) {
			return 1;
		}
	}
	return 0;
}
bool cbuild_sv_prefix(cbuild_sv_t sv, cbuild_sv_t prefix) {
	if (sv.size < prefix.size) {
		return false;
	}
	return cbuild_sv_cmp(cbuild_sv_from_parts(sv.data, prefix.size), prefix) == 0;
}
bool cbuild_sv_suffix(cbuild_sv_t sv, cbuild_sv_t suffix) {
	if (sv.size < suffix.size) {
		return false;
	}
	return cbuild_sv_cmp(
						 cbuild_sv_from_parts(sv.data + sv.size - suffix.size, suffix.size),
						 suffix) == 0;
}
/* Command.h impl */
cbuild_da_t_impl(char*, CBuildCMDchar_ptr);
void cbuild_cmd_to_sb(cbuild_cmd_t cmd, cbuild_sb_t* sb) {
	if (cmd.size < 1) {
		cbuild_sb_clear(sb);
		return;
	}
	for (size_t i = 0; i < cmd.size; i++) {
		const char* tmp = cmd.data[i];
		cbuild_sb_append_cstr(sb, tmp);
		if (i < cmd.size - 1) {
			cbuild_sb_append(sb, ' ');
		}
	}
}
#if defined(CBUILD_API_POSIX)
#	if defined(CBUILD_OS_MACOS)
#		define environ (*_NSGetEnviron())
#	else
extern char** environ;
#	endif
cbuild_proc_t cbuild_cmd_async_redirect(cbuild_cmd_t        cmd,
                                        CBuildCmdFDRedirect fd) {
	if (cmd.size < 1) {
		cbuild_log(CBUILD_LOG_ERROR, "Empty command is requested to be executed!");
		return CBUILD_INVALID_PROC;
	}
	cbuild_proc_t proc = fork();
	if (proc < 0) {
		cbuild_log(CBUILD_LOG_ERROR, "Can not create child process, error: \"%s\"",
		           strerror(errno));
		return CBUILD_INVALID_PROC;
	}
	if (proc == 0) {
		fflush(NULL);
		if (fd.fdstdin != CBUILD_INVALID_FD) {
			if (dup2(fd.fdstdin, STDIN_FILENO) < 0) {
				cbuild_log(
						CBUILD_LOG_ERROR,
						"Cannot redirect stdin inside of a child process, error: \"%s\"",
						strerror(errno));
				exit(1);
			}
		}
		if (fd.fdstdout != CBUILD_INVALID_FD) {
			if (dup2(fd.fdstdout, STDOUT_FILENO) < 0) {
				cbuild_log(
						CBUILD_LOG_ERROR,
						"Cannot redirect stdout inside of a child process, error: \"%s\"",
						strerror(errno));
				exit(1);
			}
		}
		if (fd.fdstderr != CBUILD_INVALID_FD) {
			if (dup2(fd.fdstderr, STDERR_FILENO) < 0) {
				cbuild_log(
						CBUILD_LOG_ERROR,
						"Cannot redirect stderr inside of a child process, error: \"%s\"",
						strerror(errno));
				exit(1);
			}
		}
		cbuild_cmd_t argv = cbuild_cmd;
		cbuild_da_append_arr(&argv, (const char**)cmd.data, cmd.size);
		cbuild_cmd_append(&argv, NULL);
		cbuild_sb_t path = cbuild_sb;
		cbuild_cmd_resolve_path(argv.data[0], &path);
		cbuild_sb_append_null(&path);
		argv.data[0] = path.data;
		if (execve(argv.data[0], (char* const*)argv.data, environ) < 0) {
			cbuild_log(CBUILD_LOG_ERROR,
			           "Cannot execute command in child process, error: \"%s\"",
			           strerror(errno));
			exit(1);
		}
		exit(0);
	}
	return proc;
}
int cbuild_cmd_resolve_path(const char* name, cbuild_sb_t* out) {
	cbuild_sb_clear(out);
	if (strchr(name, '/') != NULL) {
		cbuild_sb_append_cstr(out, name);
		return 0;
	}
	const char* PATH = getenv("PATH");
	if (PATH == NULL || strlen(PATH) == 0) {
		// TODO: confstr
		PATH = "/usr/bin:/bin";
	}
	cbuild_sv_t path = cbuild_sv_from_cstr(PATH);
	cbuild_sb_t tmp  = cbuild_sb;
	cbuild_sb_resize(&tmp, 256);
	while (path.size > 0) {
		cbuild_sv_t dir = cbuild_sv_chop_by_delim(&path, ':');
		if (dir.size == 0) {
			cbuild_sb_append(&tmp, '.');
		} else {
			cbuild_sb_append_sv(&tmp, dir);
		}
		cbuild_sb_append(&tmp, '/');
		cbuild_sb_append_cstr(&tmp, name);
		cbuild_sb_append_null(&tmp);
		if (access(tmp.data, X_OK) == 0) {
			cbuild_sb_append_arr(out, tmp.data, tmp.size - 1);
			cbuild_sb_clear(&tmp);
			return 0;
		}
		tmp.size = 0;
	}
	cbuild_sb_clear(&tmp);
	return -1;
}
#endif // CBUILD_API_POSIX
bool cbuild_cmd_sync_redirect(cbuild_cmd_t cmd, CBuildCmdFDRedirect fd) {
	cbuild_proc_t proc = cbuild_cmd_async_redirect(cmd, fd);
	return cbuild_proc_wait(proc);
}
/* Log.h impl */
void __cbuild_log_fmt(CBuildLogLevel level) {
	time_t     t       = time(NULL);
	struct tm* tm_info = localtime(&t);
	__CBUILD_ERR_PRINTF("[%02d:%02d:%02d] ", tm_info->tm_hour, tm_info->tm_min,
	                    tm_info->tm_sec);
	switch (level) {
	case CBUILD_LOG_NO_LOGS: break;
	case CBUILD_LOG_ERROR:
		__CBUILD_ERR_PRINT(
				CBUILD_TERM_FG(CBUILD_TERM_RED) "[ERROR]" CBUILD_TERM_RESET " ");
		break;
	case CBUILD_LOG_WARN:
		__CBUILD_ERR_PRINT(
				CBUILD_TERM_FG(CBUILD_TERM_YELLOW) "[WARN]" CBUILD_TERM_RESET " ");
		break;
	case CBUILD_LOG_INFO: __CBUILD_ERR_PRINT("[INFO] "); break;
	case CBUILD_LOG_TRACE:
		__CBUILD_ERR_PRINT(
				CBUILD_TERM_FG(CBUILD_TERM_BRBLACK) "[TRACE]" CBUILD_TERM_RESET " ");
		break;
	case CBUILD_LOG_PRINT: break;
	default              : break;
	}
}
static CBuildLogLevel     __CBUILD_LOG_MIN_LEVEL = CBUILD_LOG_MIN_LEVEL;
static CBuildLogFormatter __CBUILD_LOG_FMT       = __cbuild_log_fmt;
void cbuild_log(CBuildLogLevel level, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	cbuild_vlog(level, fmt, args);
	va_end(args);
}
void cbuild_vlog(CBuildLogLevel level, const char* fmt, va_list args) {
	if (level == CBUILD_LOG_NO_LOGS) {
		return;
	}
	if (level > __CBUILD_LOG_MIN_LEVEL) {
		return;
	}
	if (level < CBUILD_LOG_PRINT) {
		__CBUILD_LOG_FMT(level);
		__CBUILD_ERR_VPRINTF(fmt, args);
		__CBUILD_ERR_PRINT("\n");
	} else {
		__CBUILD_VPRINTF(fmt, args);
		__CBUILD_PRINT("\n");
	}
}
void cbuild_log_set_min_level(CBuildLogLevel level) {
	__CBUILD_LOG_MIN_LEVEL = level;
}
void cbuild_log_set_fmt(CBuildLogFormatter fmt) { __CBUILD_LOG_FMT = fmt; }
/* Proc.h impl */
#if defined(CBUILD_API_POSIX)
int cbuild_proc_wait_code(cbuild_proc_t proc) {
	if (proc == CBUILD_INVALID_PROC) {
		return INT_MIN;
	}
	while (true) {
		int status = 0;
		if (waitpid(proc, &status, 0) < 0) {
			cbuild_log(CBUILD_LOG_ERROR,
			           "Cannot wait for child process (pid %d), error: \"%s\"", proc,
			           strerror(errno));
			abort();
		}
		if (WIFEXITED(status)) {
			int code = WEXITSTATUS(status);
			return code;
		}
		if (WIFSIGNALED(status)) {
			cbuild_log(CBUILD_LOG_ERROR,
			           "Process (pid %d) was terminated by signal \"%d\"", proc,
			           WTERMSIG(status));
			return INT_MAX;
		}
	}
}
cbuild_proc_ptr_t cbuild_proc_malloc(size_t n) {
	void* ptr =
			mmap(NULL, n, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (ptr == MAP_FAILED) {
		return (cbuild_proc_ptr_t){ .ptr = NULL, .size = 0 };
	} else {
		return (cbuild_proc_ptr_t){ .ptr = ptr, .size = n };
	}
}
void cbuild_proc_free(cbuild_proc_ptr_t ptr) {
	if (ptr.ptr != NULL) {
		munmap(ptr.ptr, ptr.size);
	}
}
// Maybe: RtlCloneUserProcess on WinAPI. To not link to ntdll.lib GetProcAddress
// could be used.
cbuild_proc_t cbuild_proc_start(int (*callback)(void* context), void* context) {
	cbuild_proc_t proc = fork();
	if (proc < 0) {
		cbuild_log(CBUILD_LOG_ERROR, "Can not create child process, error: \"%s\"",
		           strerror(errno));
		return CBUILD_INVALID_PROC;
	}
	if (proc == 0) {
		int code = callback(context);
		exit(code);
	}
	return proc;
}
#endif // CBUILD_API_POSIX
bool cbuild_proc_wait(cbuild_proc_t proc) {
	return cbuild_proc_wait_code(proc) == 0;
}
// /* FS.h */
// bool cbuild_fd_close(CBuildFD fd) {
// 	if (fd == CBUILD_INVALID_FD) {
// 		cbuild_log(CBUILD_LOG_ERROR,
// 		           "Invalid file descriptor passed to close, error: \"%s\"",
// 		           strerror(errno));
// 		return false;
// 	}
// 	return close(fd) < 0;
// }
// CBuildFD cbuild_fd_open_read(const char* path) {
// 	CBuildFD fd = open(path, O_RDONLY);
// 	if (fd < 0) {
// 		cbuild_log(CBUILD_LOG_ERROR,
// 		           "Cannot open file \"%s\" for reading, error: \"%s\"", path,
// 		           strerror(errno));
// 		return CBUILD_INVALID_FD;
// 	}
// 	return fd;
// }
// CBuildFD cbuild_fd_open_write(const char* path) {
// 	CBuildFD fd = open(path, O_WRONLY | O_CREAT | O_TRUNC,
// 	                   S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
// 	if (fd < 0) {
// 		cbuild_log(CBUILD_LOG_ERROR,
// 		           "Cannot open file \"%s\" for reading, error: \"%s\"", path,
// 		           strerror(errno));
// 		return CBUILD_INVALID_FD;
// 	}
// 	return fd;
// }
// bool cbuild_fd_open_pipe(CBuildFD* read, CBuildFD* write) {
// 	CBuildFD fds[2];
// 	int      ret = pipe(fds);
// 	if (ret < 0) {
// 		cbuild_log(CBUILD_LOG_ERROR, "Cannot create pipe, error: \"%s\"",
// 		           strerror(errno));
// 		*read  = CBUILD_INVALID_FD;
// 		*write = CBUILD_INVALID_FD;
// 		return false;
// 	}
// 	*read  = fds[0];
// 	*write = fds[1];
// 	return true;
// }
// bool cbuild_file_read(const char* path, CBuildStrBuff* data) {
// 	CBuildFD fd = cbuild_fd_open_read(path);
// 	if (CBUILD_INVALID_FD == fd) {
// 		return false;
// 	}
// 	struct stat statbuff;
// 	if (stat(path, &statbuff) < 0) {
// 		cbuild_log(CBUILD_LOG_ERROR, "Cannot stat file \"%s\", error: \"%s\"",
// path, 		           strerror(errno)); 		cbuild_fd_close(fd); 		return
// false;
// 	}
// 	size_t size = statbuff.st_size;
// 	data->data  = malloc(size);
// 	if (data->data == NULL) {
// 		cbuild_log(
// 				CBUILD_LOG_ERROR,
// 				"Cannot allocate memory for a file, %zu bytes requested, error:
// \"%s\"", 				size, strerror(errno)); 		cbuild_fd_close(fd); 		return
// false;
// 	}
// 	ssize_t len = read(fd, data->data, size);
// 	if (len < 0) {
// 		cbuild_log(CBUILD_LOG_ERROR, "Cannot read file, error: \"%s\"",
// 		           strerror(errno));
// 		cbuild_fd_close(fd);
// 		return false;
// 	}
// 	data->size     = len;
// 	data->capacity = len;
// 	cbuild_fd_close(fd);
// 	return true;
// }
// bool cbuild_file_write(const char* path, CBuildStrBuff* data) {
// 	CBuildFD fd = cbuild_fd_open_write(path);
// 	if (CBUILD_INVALID_FD == fd) {
// 		return false;
// 	}
// 	char*   buf = data->data;
// 	ssize_t cnt = data->size;
// 	while (cnt > 0) {
// 		ssize_t written = write(fd, buf, cnt);
// 		if (written < 0) {
// 			cbuild_log(CBUILD_LOG_ERROR,
// 			           "Wrror while writing to file \"%s\", error: \"%s\"", path,
// 			           strerror(errno));
// 			cbuild_fd_close(fd);
// 			return false;
// 		}
// 		cnt -= written;
// 		buf += written;
// 	}
// 	cbuild_fd_close(fd);
// 	return true;
// }
// bool cbuild_file_copy(const char* src, const char* dst) {
// 	CBuildFD src_fd = cbuild_fd_open_read(src);
// 	CBuildFD dst_fd = cbuild_fd_open_write(dst);
// 	if (src_fd < 0) {
// 		cbuild_log(CBUILD_LOG_ERROR,
// 		           "Cannot open source file \"%s\", error: \"%s\"", src,
// 		           strerror(errno));
// 		return false;
// 	}
// 	if (dst_fd < 0) {
// 		cbuild_log(CBUILD_LOG_ERROR,
// 		           "Cannot open destination file \"%s\", error: \"%s\"", dst,
// 		           strerror(errno));
// 		return false;
// 	}
// 	struct stat statbuff;
// 	if (stat(src, &statbuff) < 0) {
// 		cbuild_log(CBUILD_LOG_ERROR,
// 		           "Cannot stat source file \"%s\", error: \"%s\"", dst,
// 		           strerror(errno));
// 		return false;
// 	}
// 	char* tmp_buff = (char*)malloc(CBUILD_TMP_BUFF_SIZE);
// 	while (true) {
// 		ssize_t cnt = read(src_fd, tmp_buff, CBUILD_TMP_BUFF_SIZE);
// 		if (cnt == 0) {
// 			break;
// 		}
// 		if (cnt < 0) {
// 			cbuild_log(CBUILD_LOG_ERROR,
// 			           "Error while reading source file \"%s\", error: \"%s\"", src,
// 			           strerror(errno));
// 			return false;
// 		}
// 		char* buf = tmp_buff;
// 		while (cnt > 0) {
// 			ssize_t written = write(dst_fd, buf, cnt);
// 			if (written < 0) {
// 				cbuild_log(CBUILD_LOG_ERROR,
// 				           "Wrror while writing to file \"%s\", error: \"%s\"", dst,
// 				           strerror(errno));
// 				cbuild_fd_close(src_fd);
// 				cbuild_fd_close(dst_fd);
// 				free(tmp_buff);
// 				return false;
// 			}
// 			cnt -= written;
// 			buf += written;
// 		}
// 	}
// 	cbuild_fd_close(src_fd);
// 	cbuild_fd_close(dst_fd);
// 	free(tmp_buff);
// 	return true;
// }
// bool cbuild_file_move(const char* src, const char* dst) {
// 	bool ret = cbuild_file_copy(src, dst);
// 	if (ret == false) {
// 		return false;
// 	}
// 	return cbuild_file_remove(src);
// }
// bool cbuild_file_rename(const char* src, const char* dst) {
// 	return cbuild_file_move(src, dst);
// }
// bool cbuild_file_check(const char* path) {
// 	struct stat statbuff;
// 	return !(stat(path, &statbuff) < 0);
// }
// bool cbuild_file_remove(const char* path) {
// 	if (unlink(path) < 0) {
// 		cbuild_log(CBUILD_LOG_ERROR, "Cannot remove file \"%s\", error: \"%s\"",
// 		           path, strerror(errno));
// 		return false;
// 	}
// 	return true;
// }
// bool cbuild_dir_copy(const char* src, const char* dst) {
// 	bool err = cbuild_dir_create(dst);
// 	if (err == false) {
// 		cbuild_log(CBUILD_LOG_ERROR, "Cannot create destination directory \"%s\"",
// 		           dst);
// 		return false;
// 	}
// 	CBuildPathList list = { 0 };

// 	err                 = cbuild_dir_list(src, &list);
// 	if (err == false) {
// 		cbuild_log(CBUILD_LOG_ERROR, "Cannot list source directory \"%s\"", src);
// 		if (list.data != NULL) {
// 			cbuild_pathlist_clear(&list);
// 		}
// 		return false;
// 	}
// 	bool ret = true;
// 	for (size_t i = 0; i < list.size; i++) {
// 		if (strcmp(".", list.data[i]) == 0 || strcmp("..", list.data[i]) == 0) {
// 			continue;
// 		}
// 		size_t lensrc = strlen(src) + 1 + strlen(list.data[i]);
// 		char*  tmpsrc = malloc(lensrc + 1);
// 		sprintf(tmpsrc, "%s/%s", src, list.data[i]);
// 		size_t lendst = strlen(dst) + 1 + strlen(list.data[i]);
// 		char*  tmpdst = malloc(lendst + 1);
// 		sprintf(tmpdst, "%s/%s", dst, list.data[i]);
// 		CBuildFiletype f = cbuild_path_filetype(tmpsrc);
// 		if (f == CBUILD_FTYPE_DIRECTORY) {
// 			bool tmp = ret && cbuild_dir_copy(tmpsrc, tmpdst);
// 			ret      = tmp;
// 		} else {
// 			bool tmp = ret && cbuild_file_copy(tmpsrc, tmpdst);
// 			ret      = tmp;
// 		}
// 		free(tmpsrc);
// 		free(tmpdst);
// 	}
// 	cbuild_pathlist_clear(&list);
// 	return ret;
// }
// bool cbuild_dir_move(const char* src, const char* dst) {
// 	bool ret = cbuild_dir_copy(src, dst);
// 	if (ret == false) {
// 		return false;
// 	}
// 	return cbuild_dir_remove(src);
// }
// bool cbuild_dir_rename(const char* src, const char* dst) {
// 	return cbuild_dir_move(src, dst);
// }
// bool cbuild_dir_remove(const char* path) {
// 	CBuildPathList list = { 0 };

// 	bool           err  = cbuild_dir_list(path, &list);
// 	if (err == false) {
// 		cbuild_log(CBUILD_LOG_ERROR,
// 		           "Cannot list source directory \"%s\", error: \"%s\"", path,
// 		           strerror(errno));
// 		if (list.data != NULL) {
// 			cbuild_pathlist_clear(&list);
// 		}
// 		return false;
// 	}
// 	bool ret = true;
// 	for (size_t i = 0; i < list.size; i++) {
// 		if (strcmp(".", list.data[i]) == 0 || strcmp("..", list.data[i]) == 0) {
// 			continue;
// 		}
// 		size_t lenpath = strlen(path) + 1 + strlen(list.data[i]);
// 		char*  tmppath = malloc(lenpath + 1);
// 		sprintf(tmppath, "%s/%s", path, list.data[i]);
// 		CBuildFiletype f = cbuild_path_filetype(tmppath);
// 		if (f == CBUILD_FTYPE_DIRECTORY) {
// 			bool tmp = ret && cbuild_dir_remove(tmppath);
// 			ret      = tmp;
// 		} else {
// 			bool tmp = ret && cbuild_file_remove(tmppath);
// 			ret      = tmp;
// 		}
// 		free(tmppath);
// 	}
// 	int stat = rmdir(path);
// 	if (stat < 0) {
// 		cbuild_log(CBUILD_LOG_ERROR,
// 		           "Cannot remove directory directory \"%s\", error: \"%s\"",
// path, 		           strerror(errno)); 		cbuild_pathlist_clear(&list);
// return false;
// 	}
// 	cbuild_pathlist_clear(&list);
// 	return ret;
// }
// bool cbuild_dir_check(const char* path) { return cbuild_file_check(path); }
// bool cbuild_dir_list(const char* path, CBuildPathList* elements) {
// 	DIR* dir = opendir(path);
// 	if (dir == NULL) {
// 		cbuild_log(CBUILD_LOG_ERROR, "Cannot open dir \"%s\", error: \"%s\"",
// path, 		           strerror(errno)); 		return false;
// 	}
// 	struct dirent* list = readdir(dir);
// 	errno               = 0;
// 	while (list != NULL) {
// 		size_t len = strlen(list->d_name);
// 		char*  str = (char*)malloc(len + 1);
// 		memcpy(str, list->d_name, len);
// 		str[len] = '\0';
// 		cbuild_da_append(elements, str);
// 		list = readdir(dir);
// 	}
// 	if (errno != 0) {
// 		cbuild_log(CBUILD_LOG_ERROR,
// 		           "Error while reading directory \"%s\", error: \"%s\"", path,
// 		           strerror(errno));
// 		return false;
// 	}
// 	return true;
// }
// void cbuild_pathlist_clear(CBuildPathList* list) {
// 	for (size_t i = 0; i < list->size; i++) { free((void*)list->data[i]); }
// 	free(list->data);
// 	list->size     = 0;
// 	list->capacity = 0;
// }
// bool cbuild_dir_create(const char* path) {
// 	int ret = mkdir(path, 0755);
// 	if (ret < 0) {
// 		if (errno == EEXIST) {
// 			cbuild_log(CBUILD_LOG_WARN, "Directory \"%s\" exist", path);
// 			return true;
// 		}
// 		cbuild_log(CBUILD_LOG_ERROR,
// 		           "Cannot create directory \"%s\", error: \"%s\"", path,
// 		           strerror(errno));
// 	}
// 	return true;
// }
// CBuildFiletype cbuild_path_filetype(const char* path) {
// 	struct stat statbuff;
// 	if (stat(path, &statbuff) < 0) {
// 		cbuild_log(CBUILD_LOG_ERROR, "Cannot stat file \"%s\", error: \"%s\"",
// path, 		           strerror(errno)); 		return CBUILD_FTYPE_OTHER;
// 	}
// 	if (S_ISREG(statbuff.st_mode)) {
// 		return CBUILD_FTYPE_REGULAR;
// 	}
// 	if (S_ISDIR(statbuff.st_mode)) {
// 		return CBUILD_FTYPE_DIRECTORY;
// 	}
// 	if (S_ISLNK(statbuff.st_mode)) {
// 		return CBUILD_FTYPE_SYMLINK;
// 	}
// 	return CBUILD_FTYPE_OTHER;
// }
// const char* cbuild_path_ext(const char* path) {
// 	ssize_t i     = strlen(path);
// 	bool    found = false;
// 	for (; i >= 0; i--) {
// 		if (path[i] == '.') {
// 			found = true;
// 			break;
// 		}
// 	}
// 	if (!found) {
// 		char* ret = malloc(1);
// 		memcpy(ret, "\0", 1);
// 		return ret;
// 	}
// 	size_t len = strlen(path) - i + 1;
// 	char*  ret = (char*)malloc(len);
// 	memcpy(ret, path + i + 1, len);
// 	return ret;
// }
// const char* cbuild_path_name(const char* path) {
// 	ssize_t i = strlen(path);
// 	if (path[i - 1] == '/') {
// 		i -= 2;
// 	}
// 	for (; i >= 0; i--) {
// 		if (path[i] == '/') {
// 			break;
// 		}
// 	}
// 	size_t len = strlen(path) - i + 1;
// 	char*  ret = (char*)malloc(len);
// 	memcpy(ret, path + i + 1, len);
// 	return ret;
// }
// const char* cbuild_path_base(const char* path) {
// 	ssize_t i = strlen(path);
// 	if (path[i - 1] == '/') {
// 		i -= 2;
// 	}
// 	bool found = false;
// 	for (; i >= 0; i--) {
// 		if (path[i] == '/') {
// 			found = true;
// 			break;
// 		}
// 	}
// 	if (!found) {
// 		char* ret = malloc(1);
// 		memcpy(ret, "\0", 1);
// 		return ret;
// 	}
// 	size_t len = i + 2;
// 	;
// 	char* ret = (char*)malloc(len);
// 	memcpy(ret, path, len - 1);
// 	ret[len - 1] = '\0';
// 	return ret;
// }
// /* Compile.h */
// void __cbuild_selfrebuild(int argc, char** argv, const char* spath) {
// 	char*  bname_new     = cbuild_shift(argv, argc);
// 	size_t bname_old_len = strlen(bname_new) + 4;
// 	char*  bname_old     = (char*)malloc(bname_old_len);
// 	memcpy(bname_old, bname_new, strlen(bname_new));
// 	bname_old[strlen(bname_new)]     = '.';
// 	bname_old[strlen(bname_new) + 1] = 'o';
// 	bname_old[strlen(bname_new) + 2] = 'l';
// 	bname_old[strlen(bname_new) + 3] = 'd';
// 	bname_old[strlen(bname_new) + 4] = '\0';
// 	int cond = cbuild_compare_mtime(bname_new, (char*)spath);
// 	if (cond < 0) {
// 		cbuild_log(CBUILD_LOG_ERROR, "Error while performing self-rebuild");
// 		free(bname_old);
// 		abort();
// 	}
// 	if (cond == 0) {
// 		free(bname_old);
// 		return;
// 	}
// 	cbuild_log(CBUILD_LOG_INFO, "Rebuilding CBuild buildscript");
// 	if (!cbuild_file_rename(bname_new, bname_old)) {
// 		cbuild_log(CBUILD_LOG_ERROR, "Cannot rename old buildscript!");
// 		free(bname_old);
// 		abort();
// 	}
// 	CBuildCmd cmd = { 0 };
// 	cbuild_cmd_append_many(&cmd, CC, "-o", (char*)bname_new, (char*)spath);
// 	if (!cbuild_cmd_sync(cmd)) {
// 		cbuild_file_rename(bname_old, bname_new);
// 	}
// 	cmd.size = 0;
// 	cbuild_cmd_append(&cmd, bname_new);
// 	cbuild_cmd_append_arr(&cmd, argv, (size_t)argc);
// 	if (!cbuild_cmd_sync(cmd)) {
// 		free(bname_old);
// 		exit(1);
// 	}
// 	free(bname_old);
// 	exit(0);
// }
// int cbuild_compare_mtime(const char* output, const char* input) {
// 	struct stat statbuff;
// 	if (stat(input, &statbuff) < 0) {
// 		cbuild_log(CBUILD_LOG_ERROR, "Cannot stat file \"%s\", error: \"%s\"",
// 		           input, strerror(errno));
// 		return -1;
// 	}
// 	int in_mtime = statbuff.st_mtime;
// 	if (stat(output, &statbuff) < 0) {
// 		if (errno == ENOENT) {
// 			return 1;
// 		}
// 		cbuild_log(CBUILD_LOG_ERROR, "Cannot stat file \"%s\", error: \"%s\"",
// 		           output, strerror(errno));
// 		return -1;
// 	}
// 	if (in_mtime > statbuff.st_mtime) {
// 		return 1;
// 	} else {
// 		return 0;
// 	}
// }
// int cbuild_compare_mtime_many(const char* output, const char** inputs,
//                               size_t num_inputs) {
// 	int ret = 0;
// 	for (size_t i = 0; i < num_inputs; i++) {
// 		int check = cbuild_compare_mtime(output, inputs[i]);
// 		if (check < 0) {
// 			return check;
// 		} else {
// 			ret += check;
// 		}
// 	}
// 	return ret;
// }
