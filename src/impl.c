#define CBUILD_COMPILE_HELPER
#include "Command.h"
#include "Compile.h"
#include "DynArray.h"
#include "FS.h"
#include "FlagParse.h"
#include "Log.h"
#include "Proc.h"
#include "StringBuilder.h"
#include "StringView.h"
#include "Term.h"
#include "Map.h"
#include "common.h"
#include "Arena.h"
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
	__CBUILD_ERR_PRINTF("%s: %s:%u: %s: Assertion \"%s\" failed with message:\n",
	  __cbuild_progname(), file, line, func, expr);
	va_list args;
	va_start(args, expr);
	const char* fmt = va_arg(args, char*);
	__CBUILD_ERR_VPRINTF(fmt, args);
	va_end(args);
	__CBUILD_ERR_FLUSH();
	abort();
}
/* StringBuilder.h impl */
int cbuild_sb_cmp(cbuild_sb_t* a, cbuild_sb_t* b) {
	if(a->size < b->size) {
		return -2;
	}
	if(a->size > b->size) {
		return 2;
	}
	int ret = memcmp(a->data, b->data, a->size);
	if(ret == 0) {
		return 0;
	} else if(ret < 0) {
		return -1;
	} else if(ret > 0) {
		return 1;
	}
	CBUILD_UNREACHABLE("cbuild_sb_cmp fallthrough");
}
int cbuild_sb_cmp_icase(cbuild_sb_t* a, cbuild_sb_t* b) {
	if(a->size < b->size) {
		return -2;
	}
	if(a->size > b->size) {
		return 2;
	}
	for(size_t i = 0; i < a->size; i++) {
		char ac =
		  'A' <= a->data[i] && a->data[i] <= 'Z' ? a->data[i] + 32 : a->data[i];
		char bc =
		  'A' <= b->data[i] && b->data[i] <= 'Z' ? b->data[i] + 32 : b->data[i];
		int diff = ac - bc;
		if(diff < 0) {
			return -1;
		} else if(diff > 0) {
			return 1;
		}
	}
	return 0;
}
cbuild_sb_t cbuild_sv_to_sb(cbuild_sv_t sv) {
	cbuild_sb_t ret = {0};
	cbuild_sb_append_arr(&ret, sv.data, sv.size);
	return ret;
}
cbuild_sv_t cbuild_sb_to_sv(cbuild_sb_t* sb) {
	return (cbuild_sv_t) {
		.data = sb->data, .size = sb->size
	};
}
int cbuild_sb_vappendf(cbuild_sb_t* sb, const char* fmt, va_list args) {
	va_list args_copy;
	va_copy(args_copy, args);
	char buff[CBUILD_SB_QUICK_SPRINTF_SIZE];
	int ret = vsnprintf(buff, CBUILD_SB_QUICK_SPRINTF_SIZE, fmt, args);
	if(ret < 0) {
		va_end(args_copy);
		return ret;
	}
	if((size_t)ret >= CBUILD_SB_QUICK_SPRINTF_SIZE) {
		char* buff1 = __CBUILD_MALLOC((size_t)ret + 1);
		cbuild_assert(buff1 != NULL, "(LIB_CBUILD_SB) Allocation failed.\n");
		vsnprintf(buff1, (size_t)ret + 1, fmt, args_copy);
		cbuild_sb_append_cstr(sb, buff1);
		__CBUILD_FREE(buff1);
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
size_t cbuild_sv_trim_left(cbuild_sv_t* sv) {
	size_t i = 0;
	while(i < sv->size && isspace(sv->data[i])) {
		i++;
	}
	sv->data += i;
	sv->size -= i;
	return i;
}
size_t cbuild_sv_trim_right(cbuild_sv_t* sv) {
	size_t i = sv->size;
	while(i > 0 && isspace(sv->data[i - 1])) {
		i--;
	}
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
	if(size > sv->size) {
		size = sv->size;
	}
	char* tmp  = sv->data;
	sv->data  += size;
	sv->size  -= size;
	return cbuild_sv_from_parts(tmp, size);
}
cbuild_sv_t cbuild_sv_chop_by_delim(cbuild_sv_t* sv, char delim) {
	char* chrptr = memchr(sv->data, delim, sv->size);
	if(chrptr != NULL) {
		size_t      i    = (size_t)(chrptr - sv->data);
		cbuild_sv_t ret  = cbuild_sv_from_parts(sv->data, i);
		sv->data        += i + 1;
		sv->size        -= i + 1;
		return ret;
	}
	return cbuild_sv_chop(sv, sv->size);
}
cbuild_sv_t cbuild_sv_chop_by_sv(cbuild_sv_t* sv, cbuild_sv_t delim) {
	if(delim.size == 0 || delim.size > sv->size) {
		return cbuild_sv_from_parts(sv->data, 0);
	}
	char*  chrptr = sv->data;
	size_t i      = 0;
	do {
		chrptr = memchr(chrptr + 1, delim.data[0], sv->size);
		if(chrptr != NULL && sv->size - i >= delim.size &&
		  memcmp(chrptr, delim.data, delim.size) == 0) {
			i                = (size_t)(chrptr - sv->data);
			cbuild_sv_t ret  = cbuild_sv_from_parts(sv->data, i);
			sv->data        += delim.size + i;
			sv->size        -= delim.size + i;
			return ret;
		}
	} while(chrptr != NULL);
	return cbuild_sv_chop(sv, sv->size);
}
cbuild_sv_t cbuild_sv_chop_by_func(cbuild_sv_t* sv, cbuild_sv_delim_func delim,
  void* args) {
	size_t i = 0;
	while(i <= sv->size && !delim(sv, i, args)) {
		i++;
	}
	if(i >= sv->size) {
		return cbuild_sv_chop(sv, i);
	}
	char* tmp  = sv->data;
	sv->data  += i + 1;
	sv->size  -= i + 1;
	return cbuild_sv_from_parts(tmp, i);
}
int cbuild_sv_cmp(cbuild_sv_t a, cbuild_sv_t b) {
	if(a.size < b.size) {
		return -2;
	}
	if(a.size > b.size) {
		return 2;
	}
	int ret = memcmp(a.data, b.data, a.size);
	if(ret == 0) {
		return 0;
	} else if(ret < 0) {
		return -1;
	} else if(ret > 0) {
		return 1;
	}
	return 0;
}
int cbuild_sv_cmp_icase(cbuild_sv_t a, cbuild_sv_t b) {
	if(a.size < b.size) {
		return -2;
	}
	if(a.size > b.size) {
		return 2;
	}
	for(size_t i = 0; i < a.size; i++) {
		char ac = 'A' <= a.data[i] && a.data[i] <= 'Z' ? a.data[i] + 32 : a.data[i];
		char bc = 'A' <= b.data[i] && b.data[i] <= 'Z' ? b.data[i] + 32 : b.data[i];
		int  diff = ac - bc;
		if(diff < 0) {
			return -1;
		} else if(diff > 0) {
			return 1;
		}
	}
	return 0;
}
bool cbuild_sv_prefix(cbuild_sv_t sv, cbuild_sv_t prefix) {
	if(sv.size < prefix.size) {
		return false;
	}
	return cbuild_sv_cmp(cbuild_sv_from_parts(sv.data, prefix.size), prefix) == 0;
}
bool cbuild_sv_suffix(cbuild_sv_t sv, cbuild_sv_t suffix) {
	if(sv.size < suffix.size) {
		return false;
	}
	return cbuild_sv_cmp(
	    cbuild_sv_from_parts(sv.data + sv.size - suffix.size, suffix.size),
	    suffix) == 0;
}
ssize_t cbuild_sv_find(cbuild_sv_t sv, char c) {
	char* chrptr = memchr(sv.data, c, sv.size);
	if(chrptr == NULL) {
		return -1;
	}
	return chrptr - sv.data;
}
ssize_t cbuild_sv_rfind(cbuild_sv_t sv, char c) {
	char* chrptr = sv.data;
#if defined(CBUILD_API_POSIX) &&                                               \
		(defined(_GNU_SOURCE) || defined(__musl__) || defined(CBUILD_OS_MACOS) ||  \
     defined(CBUILD_OS_BSD))
	chrptr = memrchr(sv.data, c, sv.size);
#else
	chrptr += sv.size;
	do {
		chrptr--;
		if(*chrptr == c) {
			goto loop_end;
		}
	} while(chrptr != sv.data);
	chrptr = NULL;
loop_end:
#endif
	if(chrptr == NULL) {
		return -1;
	}
	return chrptr - sv.data;
}
bool cbuild_sv_contains(cbuild_sv_t sv, char c) {
	return cbuild_sv_find(sv, c) != -1;
}
/* Command.h impl */
cbuild_sb_t cbuild_cmd_to_sb(cbuild_cmd_t cmd) {
	cbuild_sb_t sb = {0};
	if(cmd.size < 1) {
		return sb;
	}
	for(size_t i = 0; i < cmd.size; i++) {
		const char* tmp = cmd.data[i];
		cbuild_sb_append_cstr(&sb, tmp);
		if(i < cmd.size - 1) {
			cbuild_sb_append(&sb, ' ');
		}
	}
	return sb;
}
#if defined(CBUILD_API_POSIX)
cbuild_proc_t cbuild_cmd_async_redirect(cbuild_cmd_t cmd, cbuild_cmd_fd_t fd) {
	if(cmd.size < 1) {
		cbuild_log(CBUILD_LOG_ERROR, "Empty command requested to be executed!");
		return CBUILD_INVALID_PROC;
	}
	// Get args
	cbuild_cmd_t argv = {0};
	cbuild_cmd_append_arr(&argv, cmd.data, cmd.size);
	cbuild_cmd_append(&argv, (char*)NULL);
	cbuild_proc_t proc = fork();
	if(proc < 0) {
		cbuild_log(CBUILD_LOG_ERROR, "Could not create child process, error: \"%s\"",
		  strerror(errno));
		return CBUILD_INVALID_PROC;
	}
	if(proc == 0) {
		fflush(NULL);
		if(fd.fdstdin != CBUILD_INVALID_FD) {
			if(dup2(fd.fdstdin, STDIN_FILENO) < 0) {
				cbuild_log(
				  CBUILD_LOG_ERROR,
				  "Could not redirect stdin inside of a child process, error: \"%s\"",
				  strerror(errno));
				exit(1);
			}
		}
		if(fd.fdstdout != CBUILD_INVALID_FD) {
			if(dup2(fd.fdstdout, STDOUT_FILENO) < 0) {
				cbuild_log(
				  CBUILD_LOG_ERROR,
				  "Could not redirect stdout inside of a child process, error: \"%s\"",
				  strerror(errno));
				exit(1);
			}
		}
		if(fd.fdstderr != CBUILD_INVALID_FD) {
			if(dup2(fd.fdstderr, STDERR_FILENO) < 0) {
				cbuild_log(
				  CBUILD_LOG_ERROR,
				  "Could not redirect stderr inside of a child process, error: \"%s\"",
				  strerror(errno));
				exit(1);
			}
		}
		// Autokill
		if(cmd.flags & 0b1) {
#ifdef CBUILD_OS_LINUX
			prctl(PR_SET_PDEATHSIG, SIGKILL);
#endif
		}
		// Call command
		if(execvp(argv.data[0], (char* const*)argv.data) < 0) {
			cbuild_log(CBUILD_LOG_ERROR,
			  "Cannot execute command in child process, error: \"%s\"",
			  strerror(errno));
			exit(1);
		}
		exit(0);
	}
	cbuild_cmd_clear(&argv);
	return proc;
}
#endif // CBUILD_API_POSIX
bool cbuild_cmd_sync_redirect(cbuild_cmd_t cmd, cbuild_cmd_fd_t fd) {
	cbuild_proc_t proc = cbuild_cmd_async_redirect(cmd, fd);
	return cbuild_proc_wait(proc);
}
/* Log.h impl */
void __cbuild_log_fmt(CBuildLogLevel level) {
	time_t     t       = time(NULL);
	struct tm* tm_info = localtime(&t);
	__CBUILD_ERR_PRINTF("[%02d:%02d:%02d] ", tm_info->tm_hour, tm_info->tm_min,
	  tm_info->tm_sec);
	switch(level) {
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
	if(level == CBUILD_LOG_NO_LOGS) {
		return;
	}
	if(level > __CBUILD_LOG_MIN_LEVEL) {
		return;
	}
	if(level < CBUILD_LOG_PRINT) {
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
void cbuild_log_set_fmt(CBuildLogFormatter fmt) {
	__CBUILD_LOG_FMT = fmt;
}
/* Arena.h impl */
size_t __cbuild_int_temp_size = 0;
char __cbuild_int_temp[CBUILD_TEMP_ARENA_SIZE] = {0};
void* cbuild_temp_alloc(size_t size) {
	if(size > CBUILD_TEMP_ARENA_SIZE - __cbuild_int_temp_size) return NULL;
	void* ptr = (void*)(__cbuild_int_temp + __cbuild_int_temp_size);
	__cbuild_int_temp_size += size;
	return ptr;
}
char* cbuild_temp_sprintf(char* fmt, ...) {
	va_list va;
	va_start(va, fmt);
	char* ret = cbuild_temp_vsprintf(fmt, va);
	va_end(va);
	return ret;
}
char* cbuild_temp_vsprintf(char* fmt, va_list ap) {
	va_list va;
	va_copy(va, ap);
	int n = vsnprintf(NULL, 0, fmt, va);
	va_end(va);
	if(n >= 0) {
		char* ret =	cbuild_temp_alloc(n + 1);
		vsnprintf(ret, n + 1, fmt, ap);
		return ret;
	} else {
		return NULL;
	}
}
char* cbuild_temp_strdup(char* str) {
	size_t len = strlen(str) + 1;
	char* dup = (char*)cbuild_temp_alloc(len);
	if(dup == NULL) return NULL;
	memcpy(dup, str, len);
	return dup;
}
void* cbuild_temp_memdup(void* mem, size_t size) {
	char* dup = (char*)cbuild_temp_alloc(size);
	if(dup == NULL) return NULL;
	memcpy(dup, mem, size);
	return dup;
}
void cbuild_temp_reset() {
	__cbuild_int_temp_size = 0;
}
/* Proc.h impl */
#if defined(CBUILD_API_POSIX)
int cbuild_proc_wait_code(cbuild_proc_t proc) {
	if(proc == CBUILD_INVALID_PROC) {
		return INT_MIN;
	}
	while(true) {
		int status = 0;
		if(waitpid(proc, &status, 0) < 0) {
			cbuild_log(CBUILD_LOG_ERROR,
			  "Cannot wait for child process (pid %d), error: \"%s\"", proc,
			  strerror(errno));
			abort();
		}
		if(WIFEXITED(status)) {
			int code = WEXITSTATUS(status);
			return code;
		}
		if(WIFSIGNALED(status)) {
			cbuild_log(CBUILD_LOG_ERROR,
			  "Process (pid %d) was terminated by signal \"%d\"", proc,
			  WTERMSIG(status));
			return INT_MAX;
		}
	}
}
bool cbuild_proc_is_running(cbuild_proc_t proc) {
	if(proc <= 0) return false;
	return kill(proc, 0) <= 0;
}
cbuild_proc_ptr_t cbuild_proc_malloc(size_t n) {
	void* ptr =
	  mmap(NULL, n, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if(ptr == MAP_FAILED) {
		return (cbuild_proc_ptr_t) {
			.ptr = NULL, .size = 0
		};
	} else {
		return (cbuild_proc_ptr_t) {
			.ptr = ptr, .size = n
		};
	}
}
void cbuild_proc_free(cbuild_proc_ptr_t ptr) {
	if(ptr.ptr != NULL) {
		munmap(ptr.ptr, ptr.size);
	}
}
// Maybe: RtlCloneUserProcess on WinAPI. To not link to ntdll.lib GetProcAddress could be used.
cbuild_proc_t cbuild_proc_start(int (*callback)(void* context), void* context) {
	cbuild_proc_t proc = fork();
	if(proc < 0) {
		cbuild_log(CBUILD_LOG_ERROR, "Can not create child process, error: \"%s\"",
		  strerror(errno));
		return CBUILD_INVALID_PROC;
	}
	if(proc == 0) {
		int code = callback(context);
		exit(code);
	}
	return proc;
}
#endif // CBUILD_API_POSIX
bool cbuild_proc_wait(cbuild_proc_t proc) {
	return cbuild_proc_wait_code(proc) == 0;
}
/* FS.h impl */
bool cbuild_fd_close(cbuild_fd_t fd) {
	if(fd == CBUILD_INVALID_FD) {
		cbuild_log(CBUILD_LOG_ERROR,
		  "Invalid file descriptor passed to close, error: \"%s\"",
		  strerror(errno));
		return false;
	}
	return close(fd) < 0;
}
cbuild_fd_t cbuild_fd_open_read(const char* path) {
	cbuild_fd_t fd = open(path, O_RDONLY);
	if(fd < 0) {
		cbuild_log(CBUILD_LOG_ERROR,
		  "Cannot open file \"%s\" for reading, error: \"%s\"", path,
		  strerror(errno));
		return CBUILD_INVALID_FD;
	}
	fcntl(fd, F_SETFD, FD_CLOEXEC);
	return fd;
}
cbuild_fd_t cbuild_fd_open_write(const char* path) {
	cbuild_fd_t fd = open(path, O_WRONLY | O_CREAT | O_TRUNC,
	    S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if(fd < 0) {
		cbuild_log(CBUILD_LOG_ERROR,
		  "Cannot open file \"%s\" for writing, error: \"%s\"", path,
		  strerror(errno));
		return CBUILD_INVALID_FD;
	}
	fcntl(fd, F_SETFD, FD_CLOEXEC);
	return fd;
}
bool cbuild_fd_open_pipe(cbuild_fd_t* read, cbuild_fd_t* write) {
	cbuild_fd_t fds[2];
	int         ret = pipe(fds);
	if(ret < 0) {
		cbuild_log(CBUILD_LOG_ERROR, "Cannot create pipe, error: \"%s\"",
		  strerror(errno));
		*read  = CBUILD_INVALID_FD;
		*write = CBUILD_INVALID_FD;
		return false;
	}
	*read  = fds[0];
	*write = fds[1];
	fcntl(*read, F_SETFD, FD_CLOEXEC);
	fcntl(*write, F_SETFD, FD_CLOEXEC);
	return true;
}
ssize_t cbuild_fd_read(cbuild_fd_t fd, void* buf, size_t nbytes) {
	return read(fd, buf, nbytes);
}
ssize_t cbuild_fd_write(cbuild_fd_t fd, const void* buf, size_t nbytes) {
	return write(fd, buf, nbytes);
}
bool cbuild_file_read(const char* path, cbuild_sb_t* data) {
	cbuild_fd_t fd = cbuild_fd_open_read(path);
	if(CBUILD_INVALID_FD == fd) {
		return false;
	}
	struct stat statbuff;
	if(stat(path, &statbuff) < 0) {
		cbuild_log(CBUILD_LOG_ERROR, "Cannot stat file \"%s\", error: \"%s\"", path,
		  strerror(errno));
		cbuild_fd_close(fd);
		return false;
	}
	size_t size = (size_t)statbuff.st_size;
	cbuild_sb_resize(data, size + 1);
	if(data->data == NULL) {
		cbuild_log(
		  CBUILD_LOG_ERROR,
		  "Cannot allocate memory for a file, %zu bytes requested, error: \"%s\"",
		  size, strerror(errno));
		cbuild_fd_close(fd);
		return false;
	}
	ssize_t len = read(fd, data->data, size);
	if(len < 0) {
		cbuild_log(CBUILD_LOG_ERROR, "Cannot read file, error: \"%s\"",
		  strerror(errno));
		cbuild_fd_close(fd);
		return false;
	}
	data->size     = (size_t)len;
	data->capacity = (size_t)len;
	cbuild_fd_close(fd);
	return true;
}
bool cbuild_file_write(const char* path, cbuild_sb_t* data) {
	cbuild_fd_t fd = cbuild_fd_open_write(path);
	if(CBUILD_INVALID_FD == fd) {
		return false;
	}
	char*   buf = data->data;
	ssize_t cnt = (ssize_t)data->size;
	while(cnt > 0) {
		ssize_t written = write(fd, buf, (size_t)cnt);
		if(written < 0) {
			cbuild_log(CBUILD_LOG_ERROR,
			  "Wrror while writing to file \"%s\", error: \"%s\"", path,
			  strerror(errno));
			cbuild_fd_close(fd);
			return false;
		}
		cnt -= written;
		buf += written;
	}
	cbuild_fd_close(fd);
	return true;
}
bool cbuild_file_copy(const char* src, const char* dst) {
	cbuild_fd_t src_fd = cbuild_fd_open_read(src);
	cbuild_fd_t dst_fd = cbuild_fd_open_write(dst);
	if(src_fd < 0) {
		cbuild_log(CBUILD_LOG_ERROR,
		  "Cannot open source file \"%s\", error: \"%s\"", src,
		  strerror(errno));
		return false;
	}
	if(dst_fd < 0) {
		cbuild_log(CBUILD_LOG_ERROR,
		  "Cannot open destination file \"%s\", error: \"%s\"", dst,
		  strerror(errno));
		return false;
	}
	struct stat statbuff;
	if(stat(src, &statbuff) < 0) {
		cbuild_log(CBUILD_LOG_ERROR,
		  "Cannot stat source file \"%s\", error: \"%s\"", dst,
		  strerror(errno));
		return false;
	}
	char* tmp_buff = (char*)__CBUILD_MALLOC(CBUILD_TMP_BUFF_SIZE);
	cbuild_assert(tmp_buff != NULL, "(LIB_CBUILD_FS) Allocation failed.\n");
	while(true) {
		ssize_t cnt = read(src_fd, tmp_buff, CBUILD_TMP_BUFF_SIZE);
		if(cnt == 0) {
			break;
		}
		if(cnt < 0) {
			cbuild_log(CBUILD_LOG_ERROR,
			  "Error while reading source file \"%s\", error: \"%s\"", src,
			  strerror(errno));
			return false;
		}
		char* buf = tmp_buff;
		while(cnt > 0) {
			ssize_t written = write(dst_fd, buf, (size_t)cnt);
			if(written < 0) {
				cbuild_log(CBUILD_LOG_ERROR,
				  "Wrror while writing to file \"%s\", error: \"%s\"", dst,
				  strerror(errno));
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
bool cbuild_file_move(const char* src, const char* dst) {
	bool ret = cbuild_file_copy(src, dst);
	if(ret == false) {
		return false;
	}
	return cbuild_file_remove(src);
}
bool cbuild_file_rename(const char* src, const char* dst) {
	return cbuild_file_move(src, dst);
}
bool cbuild_file_check(const char* path) {
	struct stat statbuff;
	return !(stat(path, &statbuff) < 0);
}
bool cbuild_file_remove(const char* path) {
	if(unlink(path) < 0) {
		cbuild_log(CBUILD_LOG_ERROR, "Cannot remove file \"%s\", error: \"%s\"",
		  path, strerror(errno));
		return false;
	}
	return true;
}
bool cbuild_dir_copy(const char* src, const char* dst) {
	bool err = cbuild_dir_create(dst);
	if(err == false) {
		cbuild_log(CBUILD_LOG_ERROR, "Cannot create destination directory \"%s\"",
		  dst);
		return false;
	}
	cbuild_pathlist_t list = {0};
	err                    = cbuild_dir_list(src, &list);
	if(err == false) {
		cbuild_log(CBUILD_LOG_ERROR, "Cannot list source directory \"%s\"", src);
		if(list.data != NULL) {
			cbuild_pathlist_clear(&list);
		}
		return false;
	}
	bool ret = true;
	for(size_t i = 0; i < list.size; i++) {
		if(strcmp(".", list.data[i]) == 0 || strcmp("..", list.data[i]) == 0) {
			continue;
		}
		size_t lensrc = strlen(src) + 1 + strlen(list.data[i]);
		char*  tmpsrc = __CBUILD_MALLOC(lensrc + 1);
		cbuild_assert(tmpsrc != NULL, "(LIB_CBUILD_SB) Allocation failed.\n");
		sprintf(tmpsrc, "%s/%s", src, list.data[i]);
		size_t lendst = strlen(dst) + 1 + strlen(list.data[i]);
		char*  tmpdst = __CBUILD_MALLOC(lendst + 1);
		cbuild_assert(tmpdst != NULL, "(LIB_CBUILD_SB) Allocation failed.\n");
		sprintf(tmpdst, "%s/%s", dst, list.data[i]);
		cbuild_filetype_t f = cbuild_path_filetype(tmpsrc);
		if(f == CBUILD_FTYPE_DIRECTORY) {
			bool tmp = ret && cbuild_dir_copy(tmpsrc, tmpdst);
			ret      = tmp;
		} else {
			bool tmp = ret && cbuild_file_copy(tmpsrc, tmpdst);
			ret      = tmp;
		}
		__CBUILD_FREE(tmpsrc);
		__CBUILD_FREE(tmpdst);
	}
	cbuild_pathlist_clear(&list);
	return ret;
}
bool cbuild_dir_move(const char* src, const char* dst) {
	bool ret = cbuild_dir_copy(src, dst);
	if(ret == false) {
		return false;
	}
	return cbuild_dir_remove(src);
}
bool cbuild_dir_rename(const char* src, const char* dst) {
	return cbuild_dir_move(src, dst);
}
bool cbuild_dir_remove(const char* path) {
	cbuild_pathlist_t list = {0};
	bool              err  = cbuild_dir_list(path, &list);
	if(err == false) {
		cbuild_log(CBUILD_LOG_ERROR,
		  "Cannot list source directory \"%s\", error: \"%s\"", path,
		  strerror(errno));
		if(list.data != NULL) {
			cbuild_pathlist_clear(&list);
		}
		return false;
	}
	bool ret = true;
	for(size_t i = 0; i < list.size; i++) {
		if(strcmp(".", list.data[i]) == 0 || strcmp("..", list.data[i]) == 0) {
			continue;
		}
		size_t lenpath = strlen(path) + 1 + strlen(list.data[i]);
		char*  tmppath = __CBUILD_MALLOC(lenpath + 1);
		cbuild_assert(tmppath != NULL, "(LIB_CBUILD_SB) Allocation failed.\n");
		sprintf(tmppath, "%s/%s", path, list.data[i]);
		cbuild_filetype_t f = cbuild_path_filetype(tmppath);
		if(f == CBUILD_FTYPE_DIRECTORY) {
			bool tmp = ret && cbuild_dir_remove(tmppath);
			ret      = tmp;
		} else {
			bool tmp = ret && cbuild_file_remove(tmppath);
			ret      = tmp;
		}
		__CBUILD_FREE(tmppath);
	}
	int stat = rmdir(path);
	if(stat < 0) {
		cbuild_log(CBUILD_LOG_ERROR,
		  "Cannot remove directory directory \"%s\", error: \"%s\"", path,
		  strerror(errno));
		cbuild_pathlist_clear(&list);
		return false;
	}
	cbuild_pathlist_clear(&list);
	return ret;
}
bool cbuild_dir_check(const char* path) {
	return cbuild_file_check(path);
}
int __cbuild_int_fs_dir_list_no_dots(const struct dirent *d) {
	return !(strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0);
}
bool cbuild_dir_list(const char* path, cbuild_pathlist_t* elements) {
	cbuild_da_clear(elements);
	struct dirent** namelist;
	int n = scandir(path, &namelist, __cbuild_int_fs_dir_list_no_dots, alphasort);
	if(n < 0) {
		cbuild_log(CBUILD_LOG_ERROR, "Cannot list directory \"%s\", error: \"%s\"",
		  path, strerror(errno));
		return false;
	}
	for(int i = 0; i < n; i++) {
		size_t len = strlen(namelist[i]->d_name);
		char*  str = (char*)__CBUILD_MALLOC(len + 1);
		cbuild_assert(str != NULL, "(LIB_CBUILD_SB) Allocation failed.\n");
		__CBUILD_MEMCPY(str, namelist[i]->d_name, len);
		str[len] = '\0';
		cbuild_da_append(elements, str);
		free(namelist[i]);
	}
	free(namelist);
	return true;
}
void cbuild_pathlist_clear(cbuild_pathlist_t* list) {
	for(size_t i = 0; i < list->size; i++) {
		free((void*)list->data[i]);
	}
	cbuild_da_clear(list);
}
bool cbuild_dir_create(const char* path) {
	int ret = mkdir(path, 0755);
	if(ret < 0) {
		if(errno == EEXIST) {
			cbuild_log(CBUILD_LOG_WARN, "Directory \"%s\" exist", path);
			return false;
		}
		cbuild_log(CBUILD_LOG_ERROR,
		  "Cannot create directory \"%s\", error: \"%s\"", path,
		  strerror(errno));
		return false;
	}
	return true;
}
cbuild_filetype_t cbuild_path_filetype(const char* path) {
	struct stat statbuff;
	if(stat(path, &statbuff) < 0) {
		cbuild_log(CBUILD_LOG_ERROR, "Cannot stat file \"%s\", error: \"%s\"", path,
		  strerror(errno));
		return CBUILD_FTYPE_OTHER;
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
const char* cbuild_path_ext(const char* path) {
	ssize_t i     = (ssize_t)strlen(path);
	bool    found = false;
	for(; i >= 0; i--) {
		if(path[i] == '.') {
			found = true;
			break;
		}
	}
	if(!found) {
		char* ret = __CBUILD_MALLOC(1);
		cbuild_assert(ret != NULL, "(LIB_CBUILD_SB) Allocation failed.\n");
		__CBUILD_MEMCPY(ret, "\0", 1);
		return ret;
	}
	size_t len = strlen(path) - (size_t)i + 1;
	char*  ret = (char*)__CBUILD_MALLOC(len);
	cbuild_assert(ret != NULL, "(LIB_CBUILD_SB) Allocation failed.\n");
	__CBUILD_MEMCPY(ret, path + i + 1, len);
	return ret;
}
const char* cbuild_path_name(const char* path) {
	ssize_t i = (ssize_t)strlen(path);
	if(path[i - 1] == '/') {
		i -= 2;
	}
	for(; i >= 0; i--) {
		if(path[i] == '/') {
			break;
		}
	}
	size_t len = strlen(path) - (size_t)i + 1;
	char*  ret = (char*)__CBUILD_MALLOC(len);
	cbuild_assert(ret != NULL, "(LIB_CBUILD_SB) Allocation failed.\n");
	__CBUILD_MEMCPY(ret, path + i + 1, len);
	return ret;
}
const char* cbuild_path_base(const char* path) {
	ssize_t i = (ssize_t)strlen(path);
	if(path[i - 1] == '/') {
		i -= 2;
	}
	bool found = false;
	for(; i >= 0; i--) {
		if(path[i] == '/') {
			found = true;
			break;
		}
	}
	if(!found) {
		char* ret = __CBUILD_MALLOC(1);
		cbuild_assert(ret != NULL, "(LIB_CBUILD_SB) Allocation failed.\n");
		__CBUILD_MEMCPY(ret, "\0", 1);
		return ret;
	}
	size_t len = (size_t)i + 2;
	char*  ret = (char*)__CBUILD_MALLOC(len);
	cbuild_assert(ret != NULL, "(LIB_CBUILD_SB) Allocation failed.\n");
	__CBUILD_MEMCPY(ret, path, len - 1);
	ret[len - 1] = '\0';
	return ret;
}
/* Compile.h impl */
#ifdef CBUILD_API_POSIX
void __cbuild_compile_mark_exec(const char* file) {
	if(chmod(file, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH |
	  S_IXOTH) != 0) {
		cbuild_log(CBUILD_LOG_ERROR, "Cannot mark file as executable!");
	}
}
#endif // CBUILD_API_POSIX
void (*cbuild_selfrebuild_hook)(cbuild_cmd_t* cmd) = NULL;
void __cbuild_selfrebuild(int argc, char** argv, const char* spath) {
	char*       bname_new = cbuild_shift(argv, argc);
	cbuild_sb_t bname_old = {0};
	cbuild_sb_append_cstr(&bname_old, bname_new);
	cbuild_sb_append_cstr(&bname_old, ".old");
	cbuild_sb_append_null(&bname_old);

	int cond = cbuild_compare_mtime(bname_new, (char*)spath);
	if(cond < 0) {
		cbuild_log(CBUILD_LOG_ERROR, "Error while performing self-rebuild");
		cbuild_sb_clear(&bname_old);
		exit(1);
	}
	if(cond == 0) {
		cbuild_sb_clear(&bname_old);
		return;
	}
	cbuild_log(CBUILD_LOG_INFO, "Rebuilding CBuild buildscript");
	if(!cbuild_file_rename(bname_new, bname_old.data)) {
		cbuild_log(CBUILD_LOG_ERROR, "Cannot rename old buildscript!");
		cbuild_sb_clear(&bname_old);
		exit(1);
	}
	cbuild_cmd_t cmd = {0};
	cbuild_cmd_append_many(&cmd, CBUILD_CC, CBUILD_CARGS_WARN);
	if(cbuild_selfrebuild_hook != NULL) {
		cbuild_selfrebuild_hook(&cmd);
	}
	cbuild_cmd_append_many(&cmd, CBUILD_CC_OUT, (char*)bname_new, (char*)spath);
	if(!cbuild_cmd_sync(cmd)) {
		cbuild_file_rename(bname_old.data, bname_new);
	}
	__cbuild_compile_mark_exec(bname_new);
	cmd.size = 0;
	cbuild_cmd_append(&cmd, bname_new);
	cbuild_cmd_append_arr(&cmd, argv, (size_t)argc);
	if(!cbuild_cmd_sync(cmd)) {
		cbuild_sb_clear(&bname_old);
		exit(1);
	}
	cbuild_sb_clear(&bname_old);
	exit(0);
}
#ifdef CBUILD_API_POSIX
int cbuild_compare_mtime(const char* output, const char* input) {
	struct stat statbuff;
	if(stat(input, &statbuff) < 0) {
		cbuild_log(CBUILD_LOG_ERROR, "Cannot stat file \"%s\", error: \"%s\"",
		  input, strerror(errno));
		return -1;
	}
	long int in_mtime = statbuff.st_mtime;
	if(stat(output, &statbuff) < 0) {
		if(errno == ENOENT) {
			return 1;
		}
		cbuild_log(CBUILD_LOG_ERROR, "Cannot stat file \"%s\", error: \"%s\"",
		  output, strerror(errno));
		return -1;
	}
	if(in_mtime > statbuff.st_mtime) {
		return 1;
	} else {
		return 0;
	}
}
#endif // CBUILD_API_POSIX
int cbuild_compare_mtime_many(const char* output, const char** inputs,
  size_t num_inputs) {
	int ret = 0;
	for(size_t i = 0; i < num_inputs; i++) {
		int check = cbuild_compare_mtime(output, inputs[i]);
		if(check < 0) {
			return check;
		} else {
			ret += check;
		}
	}
	return ret;
}
/* Map.h impl */
size_t __cbuild_int_map_hash_func(const void* data, size_t len) {
	const unsigned char* ucPtr = (const unsigned char*)data;
	size_t         hash     = 5381;
	for(size_t i = 0; i < len; i++) {
		hash = ((hash << 5) + hash) + ucPtr[i]; // hash * 33 + data[i]
	}
	return hash;
}
size_t __cbuild_int_map_get_hash(cbuild_map_t* map, const void* key) {
	size_t hash = 0;
	if(map->hash_func == NULL) {
		if(map->key_size > 0) {
			hash = CBUILD_MAP_DEFAULT_HASH_FUNC(key, map->key_size)
			  % map->nbuckets;
		} else {
			hash = CBUILD_MAP_DEFAULT_HASH_FUNC(*((void**)key), strlen(*((char**)key)))
			  % map->nbuckets;
		}
	} else {
		hash = map->hash_func(map, key) % map->nbuckets;
	}
	return hash;
}
void* __cbuild_int_map_check_bucket(cbuild_map_t* map,
  const cbuild_map_bucket_t* bucket,  const void* key) {
	if(map->keycmp_func == NULL) {
		if(map->key_size > 0) {
			for(size_t i = 0; i < bucket->nvals; i++) {
				if(memcmp(key, ((char*)bucket->vals + (i * map->elem_size)),
				  map->key_size) == 0) {
					return ((char*)bucket->vals + (i * map->elem_size));
				}
			}
		} else {
			for(size_t i = 0; i < bucket->nvals; i++) {
				if(strcmp(*((char**)key),
				  *(char**)(void*)(((char*)bucket->vals + (i * map->elem_size)))) == 0) {
					return ((char*)bucket->vals + (i * map->elem_size));
				}
			}
		}
	} else {
		for(size_t i = 0; i < bucket->nvals; i++) {
			if(map->keycmp_func(map, key,
			  ((char*)bucket->vals + (i * map->elem_size)))) {
				return ((char*)bucket->vals + (i * map->elem_size));
			}
		}
	}
	return NULL;
}
void cbuild_map_init(cbuild_map_t* map, size_t nbuckets) {
	map->nbuckets = nbuckets;
	map->buckets = __CBUILD_MALLOC(map->nbuckets * sizeof(cbuild_map_bucket_t));
	cbuild_assert(map->buckets != NULL, "(LIB_CBUILD_MAP) Allocation failed.\n");
	__CBUILD_MEMSET(map->buckets, 0, nbuckets * sizeof(cbuild_map_bucket_t));
}
void* cbuild_map_get(cbuild_map_t* map, const void* key) {
	if(map->nbuckets == 0) {
		cbuild_log(CBUILD_LOG_ERROR,
		  "Trying to call 'cbuild_map_get' on an empty map!");
		return CBUILD_PTR_ERR;
	}
	size_t hash = __cbuild_int_map_get_hash(map, key);
	cbuild_map_bucket_t* bucket = &map->buckets[hash];
	return __cbuild_int_map_check_bucket(map, bucket, key);
}
void* cbuild_map_get_or_alloc(cbuild_map_t* map, const void* key) {
	if(map->nbuckets == 0) {
		cbuild_log(CBUILD_LOG_ERROR,
		  "Trying to call 'cbuild_map_get_or_alloc' with an empty map!");
		return NULL;
	}
	size_t hash = __cbuild_int_map_get_hash(map, key);
	cbuild_map_bucket_t* bucket = &map->buckets[hash];
	void* ret = __cbuild_int_map_check_bucket(map, bucket, key);
	if(ret != NULL) return ret;
	bucket->nvals++;
	bucket->vals = __CBUILD_REALLOC((char*)bucket->vals,
	  bucket->nvals * map->elem_size);
	cbuild_assert(bucket->vals != NULL, "(LIB_CBUILD_MAP) Allocation failed.\n");
	__CBUILD_MEMSET((char*)bucket->vals + (bucket->nvals - 1) * map->elem_size, 0,
	  map->elem_size);
	return (char*)bucket->vals + ((bucket->nvals - 1) * map->elem_size);
}
bool cbuild_map_remove_ex(cbuild_map_t* map, const void* key,
  cbuild_map_elem_clear_t elem_clear_func) {
	if(map->nbuckets == 0) {
		cbuild_log(CBUILD_LOG_ERROR,
		  "Trying to call 'cbuild_map_remove' with an empty map!");
		return NULL;
	}
	size_t hash = __cbuild_int_map_get_hash(map, key);
	cbuild_map_bucket_t* bucket = &map->buckets[hash];
	void* elem = __cbuild_int_map_check_bucket(map, bucket, key);
	if(elem == NULL) return false;
	if(elem_clear_func) elem_clear_func(map, elem);
	if(bucket->nvals == 1) {
		__CBUILD_FREE(bucket->vals);
		bucket->vals = NULL;
		bucket->nvals = 0;
	} else {
		char* last = ((char*)bucket->vals) +
		  ((bucket->nvals - 1) * map->elem_size);
		__CBUILD_MEMCPY(elem, last, map->elem_size);
		bucket->nvals--;
	}
	return true;
}
void cbuild_map_clear_ex(cbuild_map_t* map,
  cbuild_map_elem_clear_t elem_clear_func) {
	if(elem_clear_func == NULL) {
		for(size_t i = 0; i < map->nbuckets; i++) {
			cbuild_map_bucket_t* bucket = &map->buckets[i];
			__CBUILD_FREE(bucket->vals);
			bucket->vals = NULL;
			bucket->nvals = 0;
		}
		__CBUILD_FREE(map->buckets);
		map->buckets = NULL;
		map->nbuckets = 0;
	} else {
		for(size_t i = 0; i < map->nbuckets; i++) {
			cbuild_map_bucket_t* bucket = &map->buckets[i];
			for(size_t j = 0; j < bucket->nvals; j++) {
				elem_clear_func(map, bucket->vals + (j * map->elem_size));
			}
			__CBUILD_FREE(bucket->vals);
			bucket->vals = NULL;
			bucket->nvals = 0;
		}
		__CBUILD_FREE(map->buckets);
		map->buckets = NULL;
		map->nbuckets = 0;
	}
}
void cbuild_map_iter_reset(cbuild_map_t* map) {
	map->iter_buckets = 0;
	map->iter_vals = 0;
}
void* cbuild_map_iter_next(cbuild_map_t* map) {
	while(map->iter_buckets < map->nbuckets) {
		cbuild_map_bucket_t* bucket = &map->buckets[map->iter_buckets];
		while(map->iter_vals < bucket->nvals) {
			return bucket->vals + (map->iter_vals++ * map->elem_size);
		}
		map->iter_buckets++;
		map->iter_vals = 0;
	}
	return NULL;
}
/* FlagParse.h impl */
struct __cbuild_int_flag_spec_t {
	// Spec
	/* Bit-mask
	 * 0-1 -> Type
	 *    - 0b00 - lopt
	 *    - 0b01 - lopt+sopt
	 *    - 0b10 - reserved
	 *    - 0b11 - reserved
	 * 2-4 -> Argument type
	 *    - 0b000 - No arg
	 *    - 0b001 - Argument
	 *    - 0b010 - List
	 *    - 0b011 - Terminator list
	 *    - Other are reserved
	 * 5 -> Argument optionality marker
	 *    - 0b0 - Required
	 *    - 0b1 - Optional
	 * 8-15 - Reserved, always 0
	 * 16-23 - Param 1 (list length)
	 * 24-31 - Param 2 (tlist terminator)
	 */
	uint32_t          type;
	char              sopt;
	bool              found; // State
	char              _padding[2];
	size_t            aliases_len;
	cbuild_sv_t*      aliases;
	cbuild_sv_t       opt;
	cbuild_sv_t       description;
	cbuild_sv_t       type_hint;
	cbuild_sv_t       group_name; // If size is 0 then ungrouped
	cbuild_arglist_t  args; // State
};
#define __CBUILD_INT_FLAG_SET_TYPE(where, val) (where) |= (((val) & 3) << 0)
#define __CBUILD_INT_FLAG_SET_ARGT(where, val) (where) |= (((val) & 7) << 2)
#define __CBUILD_INT_FLAG_SET_ARGO(where, val) (where) |= (((val) & 1) << 5)
#define __CBUILD_INT_FLAG_SET_PRM1(where, val) (where) |= (((val) & 255) << 16)
#define __CBUILD_INT_FLAG_SET_PRM2(where, val) (where) |= (((val) & 255) << 24)
#define __CBUILD_INT_FLAG_GET_TYPE(from)       (((from) >> 0) & 0b11)
#define __CBUILD_INT_FLAG_GET_ARGT(from)       (((from) >> 2) & 0b111)
#define __CBUILD_INT_FLAG_GET_ARGO(from)       (((from) >> 5) & 0b1)
#define __CBUILD_INT_FLAG_GET_PRM1(from)       (((from) >> 16) & 0xFF)
#define __CBUILD_INT_FLAG_GET_PRM2(from)       (((from) >> 24) & 0xFF)
struct __cbuild_int_da_flag_spec_t {
	struct __cbuild_int_flag_spec_t* data;
	size_t size;
	size_t capacity;
};
struct __cbuild_int_flag_context_t {
	/* Bit-mask
	 * 0 -> Separator
	 *    - 0b0 - Don;t push
	 *    - 0b1 - Push
	 * 0-31 -> Reserved, always 0
	 */
	size_t                              metadata;
	const char*                         app_name;
	cbuild_sv_t*                        group_desc;
	size_t                              group_desc_len;
	cbuild_arglist_t                    pargs;
	struct __cbuild_int_da_flag_spec_t  flags;
};
#define __CBUILD_INT_FLGCTX_SET_SEPARATOR(val)                                 \
	(__cbuild_int_flag_context.metadata) |= (((val) & 1) << 0)
#define __CBUILD_INT_FLGCTX_GET_SEPARATOR()                                    \
	(((__cbuild_int_flag_context.metadata) >> 0) & 0b1)
static struct __cbuild_int_flag_context_t __cbuild_int_flag_context = {0};
struct __cbuild_int_flag_spec_t* __cbuild_int_flag_get_lopt(cbuild_sv_t opt) {
	cbuild_da_foreach(&__cbuild_int_flag_context.flags, spec) {
		if((__CBUILD_INT_FLAG_GET_TYPE(spec->type) == 0b00 ||
		  __CBUILD_INT_FLAG_GET_TYPE(spec->type) == 0b01) &&
		  cbuild_sv_cmp(spec->opt, opt) == 0) {
			return spec;
		}
	}
	return NULL;
}
struct __cbuild_int_flag_spec_t*
__cbuild_int_flag_get_lopt_aliased(cbuild_sv_t opt) {
	cbuild_da_foreach(&__cbuild_int_flag_context.flags, spec) {
		if((__CBUILD_INT_FLAG_GET_TYPE(spec->type) == 0b00 ||
		  __CBUILD_INT_FLAG_GET_TYPE(spec->type) == 0b01) &&
		  cbuild_sv_cmp(spec->opt, opt) == 0) {
			return spec;
		}
		for(size_t i = 0; i < spec->aliases_len; i++) {
			if(cbuild_sv_cmp(spec->aliases[i], opt) == 0) {
				return spec;
			}
		}
	}
	return NULL;
}
struct __cbuild_int_flag_spec_t* __cbuild_int_flag_get_sopt(char opt) {
	cbuild_da_foreach(&__cbuild_int_flag_context.flags, spec) {
		if(__CBUILD_INT_FLAG_GET_TYPE(spec->type) == 0b01 && spec->sopt == opt) {
			return spec;
		}
	}
	return NULL;
}
bool __cbuild_int_flag_first_delim_func(const cbuild_sv_t* sv, size_t idx,
  void* args) {
	if(sv->data[idx] == '\t' || sv->data[idx] == '\n' || sv->data[idx] == '\r') {
		*((char*)args) = sv->data[idx];
		return true;
	} else {
		return false;
	}
}
bool __cbuild_int_flag_metadata_delim_func(const cbuild_sv_t* sv, size_t idx,
  void* args) {
	if(sv->data[idx] == '\t' || sv->data[idx] == ';') {
		*((char*)args) = sv->data[idx];
		return true;
	} else {
		return false;
	}
}
void __cbuild_int_flag_parse_metadata_entry(
  struct __cbuild_int_flag_spec_t* new_spec, size_t parse_offset,
  cbuild_sv_t opt) {
	static const cbuild_sv_t key_arg    = cbuild_sv_from_lit("arg");
	static const cbuild_sv_t key_len    = cbuild_sv_from_lit("len");
	static const cbuild_sv_t key_thint  = cbuild_sv_from_lit("thint");
	static const cbuild_sv_t key_tdelim = cbuild_sv_from_lit("tdelim");
	static const cbuild_sv_t key_group  = cbuild_sv_from_lit("group");
	static const cbuild_sv_t arg_arg    = cbuild_sv_from_lit("arg");
	static const cbuild_sv_t arg_list   = cbuild_sv_from_lit("list");
	static const cbuild_sv_t arg_tlist  = cbuild_sv_from_lit("tlist");
	cbuild_sv_t              key        = cbuild_sv_chop_by_delim(&opt, '=');
	if(cbuild_sv_cmp(key, key_arg) == 0) {
		if(cbuild_sv_prefix(opt, arg_arg)) {
			__CBUILD_INT_FLAG_SET_ARGT(new_spec->type, 0b001);
		} else if(cbuild_sv_prefix(opt, arg_list)) {
			__CBUILD_INT_FLAG_SET_ARGT(new_spec->type, 0b010);
		} else if(cbuild_sv_prefix(opt, arg_tlist)) {
			__CBUILD_INT_FLAG_SET_ARGT(new_spec->type, 0b011);
		}
		if(opt.data[opt.size - 1] == '?') {
			__CBUILD_INT_FLAG_SET_ARGO(new_spec->type, 0b1);
		}
	} else if(cbuild_sv_cmp(key, key_len) == 0) {
		__CBUILD_INT_FLAG_SET_PRM1(new_spec->type, (unsigned int)atoi(opt.data));
	} else if(cbuild_sv_cmp(key, key_thint) == 0) {
		new_spec->type_hint = opt;
	} else if(cbuild_sv_cmp(key, key_tdelim) == 0) {
		__CBUILD_INT_FLAG_SET_PRM2(new_spec->type, (unsigned int)opt.data[0]);
	} else if(cbuild_sv_cmp(key, key_group) == 0) {
		new_spec->group_name = opt;
	} else {
		cbuild_log(CBUILD_LOG_ERROR,
		  "Syntax error [%zu]: Invalid metadata entry \"" CBuildSVFmt
		  "\"!",
		  parse_offset, CBuildSVArg(key));
		exit(1);
	}
}
void __cbuild_int_flag_parse_metadata_spec(
  struct __cbuild_int_flag_spec_t* new_spec, cbuild_sv_t* spec,
  size_t* parse_offset) {
	char        delim = '\t';
	cbuild_sv_t opt   = cbuild_sv_chop_by_func(
	    spec, __cbuild_int_flag_metadata_delim_func, &delim);
	while(delim != '\t') {
		if(opt.size > 0) {
			__cbuild_int_flag_parse_metadata_entry(new_spec, *parse_offset, opt);
		}
		// Parse next block
		opt = cbuild_sv_chop_by_func(spec, __cbuild_int_flag_metadata_delim_func,
		    &delim);
		(*parse_offset) += (opt.size + 1);
	}
	if(opt.size > 0) {
		__cbuild_int_flag_parse_metadata_entry(new_spec, *parse_offset, opt);
	}
}
void __cbuild_int_flag_parse_cmd(cbuild_sv_t spec) {
	static const cbuild_sv_t cmd_separator = cbuild_sv_from_lit("separator");
	static const cbuild_sv_t cmd_alias     = cbuild_sv_from_lit("alias");
	static const cbuild_sv_t cmd_group     = cbuild_sv_from_lit("group");
	if(cbuild_sv_prefix(spec, cmd_separator)) {
		__CBUILD_INT_FLGCTX_SET_SEPARATOR(1);
	} else if(cbuild_sv_prefix(spec, cmd_alias)) {
		cbuild_sv_chop_by_delim(&spec, ':');
		cbuild_sv_t flag = cbuild_sv_chop_by_delim(&spec, ':');
		struct __cbuild_int_flag_spec_t* flg = __cbuild_int_flag_get_lopt(flag);
		if(flg == NULL) {
			cbuild_log(CBUILD_LOG_ERROR,
			  "Syntax error: \"alias\" command require valid flag name "
			  "(long option should be used here!");
			exit(1);
		}
		cbuild_sv_t alias;
		do {
			alias = cbuild_sv_chop_by_delim(&spec, ',');
			if(alias.size == 0) {
				cbuild_log(CBUILD_LOG_ERROR,
				  "Syntax error: empty alias can not be defined!");
				exit(1);
			}
			flg->aliases_len++;
			flg->aliases = __CBUILD_REALLOC(flg->aliases,
			    sizeof(cbuild_sv_t) * flg->aliases_len);
			flg->aliases[flg->aliases_len - 1] = alias;
		} while(spec.size > 0);
	} else if(cbuild_sv_prefix(spec, cmd_group)) {
		cbuild_sv_chop_by_delim(&spec, ':');
		cbuild_sv_t group = cbuild_sv_chop_by_delim(&spec, ':');
		CBUILD_UNUSED(group);
		__cbuild_int_flag_context.group_desc_len += 2;
		__cbuild_int_flag_context.group_desc = __CBUILD_REALLOC(
		    __cbuild_int_flag_context.group_desc,
		    sizeof(cbuild_sv_t) * 2 * __cbuild_int_flag_context.group_desc_len);
		__cbuild_int_flag_context.group_desc[
		  __cbuild_int_flag_context.group_desc_len - 2] = group;
		__cbuild_int_flag_context.group_desc[
		  __cbuild_int_flag_context.group_desc_len - 1] = spec;
	}
}
void cbuild_flag_new(const char* spec_cstr) {
	cbuild_sv_t                     spec     = cbuild_sv_from_cstr(spec_cstr);
	struct __cbuild_int_flag_spec_t new_spec = { 0 };
	if(spec.data[0] == '-') {
		spec.data++;
		spec.size--;
		__cbuild_int_flag_parse_cmd(spec);
		return;
	}
	// Parse long options / positional arg ID
	char type_delim = '\0';
	new_spec.opt    = cbuild_sv_chop_by_func(
	    &spec, __cbuild_int_flag_first_delim_func, &type_delim);
	size_t parse_offset = new_spec.opt.size;
	switch(type_delim) {
	case '\t':
		if(spec.size < 2) {
			cbuild_log(
			  CBUILD_LOG_ERROR,
			  "Syntax error [%zu]: Expected short option name, but found nothing!",
			  parse_offset);
			exit(1);
		}
		__CBUILD_INT_FLAG_SET_TYPE(new_spec.type, 0b01);
		new_spec.sopt = spec.data[0];
		if(spec.data[1] != '\t') {
			cbuild_log(CBUILD_LOG_ERROR,
			  "Syntax error [%zu]: Expected short option name but found "
			  "more than 1 character!.",
			  parse_offset + 2);
			exit(1);
		}
		spec.data    += 2;
		spec.size    -= 2;
		parse_offset += 3; // Offset is one lower than real parse position
		__attribute__((fallthrough));
	case '\n':
		__CBUILD_INT_FLAG_SET_TYPE(new_spec.type, 0b00);
		__cbuild_int_flag_parse_metadata_spec(&new_spec, &spec, &parse_offset);
		break;
	default:
		cbuild_log(
		  CBUILD_LOG_ERROR,
		  "Syntax error [%zu]: Invalid type specifier, expected '\\t', '\\n' "
		  ", but got '%02x'!",
		  parse_offset, type_delim);
		exit(1);
		break;
	}
	new_spec.description = spec;
	new_spec.found       = false;
	new_spec.args        = (cbuild_arglist_t) {
		0
	};
	cbuild_da_append(&(__cbuild_int_flag_context.flags), new_spec);
}
void __cbuild_int_parse_flag_args(struct __cbuild_int_flag_spec_t* spec,
  int argc, char** argv, int* parse_ptr) {
	// no args
	if(__CBUILD_INT_FLAG_GET_ARGT(spec->type) == 0b000) {
		(*parse_ptr)--;
		return;
	}
	// No args but some required
	if(argc == *parse_ptr) {
		if(__CBUILD_INT_FLAG_GET_ARGO(spec->type) == 0) {
			char* type = "";
			if(__CBUILD_INT_FLAG_GET_ARGT(spec->type) == 0b001) {
				type = "one argument";
			} else if(__CBUILD_INT_FLAG_GET_ARGT(spec->type) == 0b010) {
				type = "list of arguments";
			} else if(__CBUILD_INT_FLAG_GET_ARGT(spec->type) == 0b011) {
				type = "list of arguments";
			}
			cbuild_log(CBUILD_LOG_ERROR,
			  "(CBUILD_FLAG_PARSE) Flag \"" CBuildSVFmt
			  "\" requires %s but none provided!",
			  CBuildSVArg(spec->opt), type);
			exit(1);
		} else {
			return;
		}
	}
	// One argument
	if(__CBUILD_INT_FLAG_GET_ARGT(spec->type) == 0b001) {
		char* arg = argv[*parse_ptr];
		if(arg[0] == '-') {
			if(__CBUILD_INT_FLAG_GET_ARGO(spec->type) == 0) {
				cbuild_log(CBUILD_LOG_ERROR,
				  "(CBUILD_FLAG_PARSE) Flag \"" CBuildSVFmt
				  "\" requires one argument but none provided!",
				  CBuildSVArg(spec->opt));
				exit(1);
			} else {
				(*parse_ptr)--;
				return;
			}
		} else {
			cbuild_da_clear(&spec->args);
			cbuild_da_append(&spec->args, arg);
			return;
		}
	}
	// List or TList arguments
	while(*parse_ptr < argc) {
		char* arg = argv[*parse_ptr];
		// Terminate list on argument
		if(__CBUILD_INT_FLAG_GET_ARGT(spec->type) == 0b010 && arg[0] == '-') {
			(*parse_ptr)--;
			break;
		}
		// Terminate tlist on terminator
		if(__CBUILD_INT_FLAG_GET_ARGT(spec->type) == 0b011 && strlen(arg) == 1 &&
		  (char)__CBUILD_INT_FLAG_GET_PRM2(spec->type) == arg[0]) {
			break;
		}
		// Append argument
		cbuild_da_append(&spec->args, arg);
		(*parse_ptr)++;
	}
	// Check args count
	if(__CBUILD_INT_FLAG_GET_PRM1(spec->type) != 0 &&
	  __CBUILD_INT_FLAG_GET_PRM1(spec->type) != spec->args.size) {
		cbuild_log(CBUILD_LOG_ERROR,
		  "(CBUILD_FLAG_PARSE) Flag \"" CBuildSVFmt
		  "\" requires list of %d arguments, but %zu provided!",
		  CBuildSVArg(spec->opt), __CBUILD_INT_FLAG_GET_PRM1(spec->type),
		  spec->args.size);
		exit(1);
	}
}
void cbuild_flag_parse(int argc, char** argv) {
	static const cbuild_sv_t arg_help    = cbuild_sv_from_cstr("help");
	static const cbuild_sv_t arg_version = cbuild_sv_from_cstr("version");
	static const cbuild_sv_t arg_lprefix = cbuild_sv_from_cstr("--");
	static const cbuild_sv_t arg_sprefix = cbuild_sv_from_cstr("-");
	__cbuild_int_flag_context.app_name   = argv[0];
	bool parse_no_flags                  = false;
	for(int i = 1; i < argc; i++) {
		cbuild_sv_t arg = cbuild_sv_from_cstr(argv[i]);
		if(!parse_no_flags && cbuild_sv_prefix(arg, arg_lprefix)) {
			arg.size -= 2;
			arg.data += 2;
			// --
			if(arg.size == 0) {
				parse_no_flags = true;
				if(__CBUILD_INT_FLGCTX_GET_SEPARATOR() == 1) {
					cbuild_da_append(&__cbuild_int_flag_context.pargs, "--");
				}
				continue;
			}
			if(cbuild_sv_cmp(arg, arg_help) == 0) {
				cbuild_flag_help(__cbuild_int_flag_context.app_name);
				exit(0);
			}
			if(cbuild_sv_cmp(arg, arg_version) == 0) {
				cbuild_flag_version(__cbuild_int_flag_context.app_name);
				exit(0);
			}
			struct __cbuild_int_flag_spec_t* spec =
			  __cbuild_int_flag_get_lopt_aliased(arg);
			if(spec == NULL) {
				cbuild_log(CBUILD_LOG_ERROR,
				  "(CBUILD_FLAG_PARSE) Invalid long flag \"" CBuildSVFmt "\"!",
				  CBuildSVArg(arg));
				cbuild_flag_help(__cbuild_int_flag_context.app_name);
				exit(1);
			}
			spec->found = true;
			i++;
			__cbuild_int_parse_flag_args(spec, argc, argv, &i);
		} else if(!parse_no_flags && cbuild_sv_prefix(arg, arg_sprefix)) {
			arg.size--;
			arg.data++;
			size_t sopts_len = strlen(arg.data);
			if(sopts_len == 0) {  // raw '-' option, stdin
				cbuild_da_append(&__cbuild_int_flag_context.pargs, argv[i]);
			}
			for(size_t j = 0; j < sopts_len; j++) {
				char opt = arg.data[j];
				if(opt == 'h') {
					cbuild_flag_help(__cbuild_int_flag_context.app_name);
					exit(0);
				}
				if(opt == 'v') {
					cbuild_flag_version(__cbuild_int_flag_context.app_name);
					exit(0);
				}
				struct __cbuild_int_flag_spec_t* spec = __cbuild_int_flag_get_sopt(opt);
				if(spec == NULL) {
					cbuild_log(CBUILD_LOG_ERROR,
					  "(CBUILD_FLAG_PARSE) Invalid short flag \"%c\"!", opt);
					cbuild_flag_help(__cbuild_int_flag_context.app_name);
					exit(1);
				}
				spec->found = true;
				i++;
				if(j == strlen(arg.data) - 1) {
					__cbuild_int_parse_flag_args(spec, argc, argv, &i);
				} else {
					if(__CBUILD_INT_FLAG_GET_ARGT(spec->type) != 0b000 &&
					  __CBUILD_INT_FLAG_GET_ARGO(spec->type) == 0b0) {
						char* type = "";
						if(__CBUILD_INT_FLAG_GET_ARGT(spec->type) == 0b001) {
							type = "one argument";
						} else if(__CBUILD_INT_FLAG_GET_ARGT(spec->type) == 0b010) {
							type = "list of arguments";
						} else if(__CBUILD_INT_FLAG_GET_ARGT(spec->type) == 0b011) {
							type = "list of arguments";
						}
						cbuild_log(CBUILD_LOG_ERROR,
						  "(CBUILD_FLAG_PARSE) Flag \"%c\" requires %s but none "
						  "provided!",
						  spec->sopt, type);
						exit(1);
					} else {
						i--;
					}
				}
			}
		} else {
			cbuild_da_append(&__cbuild_int_flag_context.pargs, argv[i]);
		}
	}
}
char* __cbuild_int_flag_help_fmt(struct __cbuild_int_flag_spec_t* spec) {
	cbuild_sb_t sb = {0};
	// Short opt
	if(__CBUILD_INT_FLAG_GET_TYPE(spec->type) == 0b01) {
		cbuild_sb_append_cstr(&sb, "\t-");
		cbuild_sb_append(&sb, spec->sopt);
		cbuild_sb_append_cstr(&sb, ", ");
	} else {
		cbuild_sb_append_cstr(&sb, "\t");
	}
	// Long opt
	cbuild_sb_append_cstr(&sb, "--");
	cbuild_sb_append_sv(&sb, spec->opt);
	// For normal args
	switch(__CBUILD_INT_FLAG_GET_ARGT(spec->type)) {
	case 0b000: break;
	case 0b001:
		if(spec->type_hint.size > 0) {
			cbuild_sb_append_cstr(&sb, " <");
			cbuild_sb_append_sv(&sb, spec->type_hint);
			if(__CBUILD_INT_FLAG_GET_ARGO(spec->type) == 0b1) {
				cbuild_sb_append(&sb, '?');
			}
			cbuild_sb_append(&sb, '>');
		}
		break;
	case 0b010:
		if(spec->type_hint.size > 0 ||
		  __CBUILD_INT_FLAG_GET_ARGO(spec->type) == 0b1 ||
		  __CBUILD_INT_FLAG_GET_PRM1(spec->type) != 0) {
			cbuild_sb_append_cstr(&sb, " <");
		}
		if(spec->type_hint.size > 0) {
			cbuild_sb_append_sv(&sb, spec->type_hint);
		}
		if(__CBUILD_INT_FLAG_GET_ARGO(spec->type) == 0b1) {
			cbuild_sb_append(&sb, '?');
		}
		if(__CBUILD_INT_FLAG_GET_PRM1(spec->type) != 0) {
			cbuild_sb_append(&sb, ':');
			cbuild_sb_appendf(&sb, "%d", (int)__CBUILD_INT_FLAG_GET_PRM1(spec->type));
		}
		if(spec->type_hint.size > 0 ||
		  __CBUILD_INT_FLAG_GET_ARGO(spec->type) == 0b1 ||
		  __CBUILD_INT_FLAG_GET_PRM1(spec->type) != 0) {
			cbuild_sb_append(&sb, '>');
		}
		break;
	case 0b011:
		cbuild_sb_append_cstr(&sb, " <");
		if(spec->type_hint.size > 0) {
			cbuild_sb_append_sv(&sb, spec->type_hint);
		}
		if(__CBUILD_INT_FLAG_GET_ARGO(spec->type) == 0b1) {
			cbuild_sb_append(&sb, '?');
		}
		if(__CBUILD_INT_FLAG_GET_PRM1(spec->type) != 0) {
			cbuild_sb_append(&sb, ':');
			cbuild_sb_appendf(&sb, "%d", (int)__CBUILD_INT_FLAG_GET_PRM1(spec->type));
		}
		cbuild_sb_append(&sb, '|');
		cbuild_sb_append(&sb, (char)__CBUILD_INT_FLAG_GET_PRM2(spec->type));
		cbuild_sb_append(&sb, '>');
		break;
	default: CBUILD_UNREACHABLE("Flag argument type help printer."); break;
	}
	// Null terminator
	cbuild_sb_append_null(&sb);
	return sb.data;
}
size_t __cbuild_int_flag_get_flgh_len(struct __cbuild_int_flag_spec_t* spec) {
	char*  str = __cbuild_int_flag_help_fmt(spec);
	size_t ret = strlen(str);
	__CBUILD_FREE(str);
	return ret;
}
void cbuild_flag_print_help() {
	// Get length of longest option
	size_t opt_len = strlen("\t-v, --version"); // minimal length
	cbuild_da_foreach(&__cbuild_int_flag_context.flags, spec) {
		size_t new_opt_len = __cbuild_int_flag_get_flgh_len(spec);
		opt_len            = new_opt_len > opt_len ? new_opt_len : opt_len;
	}
	// Help for flags
	__CBUILD_PRINT("Flags:\n");
	// Help
	int written = __CBUILD_PRINT("\t-h, --help");
	__CBUILD_PRINTF("%-*s", (int)(((int)opt_len + 2) - written), "");
	__CBUILD_PRINT("Shows app help (this message).\n");
	// Version
	written = __CBUILD_PRINT("\t-v, --version");
	__CBUILD_PRINTF("%-*s", (int)(((int)opt_len + 2) - written), "");
	__CBUILD_PRINT("Shows app version information.\n");
	// Extract groups and print ungrouped args
	cbuild_sv_t* groups = NULL;
	size_t groups_len = 0;
	cbuild_da_foreach(&__cbuild_int_flag_context.flags, spec) {
		if(spec->group_name.size == 0) {
			char* opt     = __cbuild_int_flag_help_fmt(spec);
			int   written = __CBUILD_PRINTF("%s", opt);
			__CBUILD_FREE(opt);
			__CBUILD_PRINTF("%-*s", (int)(((int)opt_len + 2) - written), "");
			__CBUILD_PRINTF(CBuildSVFmt, CBuildSVArg(spec->description));
			__CBUILD_PRINT("\n");
		} else {
			bool found = false;
			for(size_t i = 0; i < groups_len; i++) {
				if(cbuild_sv_cmp(spec->group_name, groups[i]) == 0) {
					found = true;
					break;
				}
			}
			if(!found) {
				groups_len++;
				groups = __CBUILD_REALLOC(groups, sizeof(cbuild_sv_t) * groups_len);
				groups[groups_len - 1] = spec->group_name;
			}
		}
	}
	cbuild_da_foreach(&__cbuild_int_flag_context.flags, spec) {
		if(spec->group_name.size == 0) {
			if(spec->aliases_len == 0) continue;
			__CBUILD_PRINT("\t");
			for(size_t i = 0; i < spec->aliases_len; i++) {
				__CBUILD_PRINTF("--"CBuildSVFmt, CBuildSVArg(spec->aliases[i]));
				if(i + 1 < spec->aliases_len) {
					__CBUILD_PRINT(", ");
				}
			}
			__CBUILD_PRINTF(" -> --"CBuildSVFmt"\n", CBuildSVArg(spec->opt));
		}
	}
	// Print grouped args
	for(size_t i = 0; i < groups_len; i++) {
		__CBUILD_PRINTF(CBuildSVFmt":\n", CBuildSVArg(groups[i]));
		for(size_t j = 0; j < __cbuild_int_flag_context.group_desc_len; j += 2) {
			if(cbuild_sv_cmp(__cbuild_int_flag_context.group_desc[j],
			  groups[i]) == 0) {
				__CBUILD_PRINTF(CBuildSVFmt"\n",
				  CBuildSVArg(__cbuild_int_flag_context.group_desc[j + 1]));
				break;
			}
		}
		cbuild_da_foreach(&__cbuild_int_flag_context.flags, spec) {
			if(cbuild_sv_cmp(spec->group_name, groups[i]) == 0) {
				char* opt     = __cbuild_int_flag_help_fmt(spec);
				int   written = __CBUILD_PRINTF("%s", opt);
				__CBUILD_FREE(opt);
				__CBUILD_PRINTF("%-*s", (int)(((int)opt_len + 2) - written), "");
				__CBUILD_PRINTF(CBuildSVFmt, CBuildSVArg(spec->description));
				__CBUILD_PRINT("\n");
			}
		}
		cbuild_da_foreach(&__cbuild_int_flag_context.flags, spec) {
			if(cbuild_sv_cmp(spec->group_name, groups[i]) == 0) {
				if(spec->aliases_len == 0) continue;
				__CBUILD_PRINT("\t");
				for(size_t i = 0; i < spec->aliases_len; i++) {
					__CBUILD_PRINTF("--"CBuildSVFmt, CBuildSVArg(spec->aliases[i]));
					if(i + 1 < spec->aliases_len) {
						__CBUILD_PRINT(", ");
					}
				}
				__CBUILD_PRINTF(" -> --"CBuildSVFmt"\n", CBuildSVArg(spec->opt));
			}
		}
	}
}
cbuild_arglist_t* cbuild_flag_get_pargs(void) {
	return &__cbuild_int_flag_context.pargs;
}
cbuild_arglist_t* cbuild_flag_get_flag(const char* opt) {
	struct __cbuild_int_flag_spec_t* spec =
	  __cbuild_int_flag_get_lopt(cbuild_sv_from_cstr(opt));
	if(spec == NULL) {
		return NULL;
	}
	if(spec->found == false) {
		return NULL;
	}
	return &spec->args;
}
char* cbuild_flag_app_name(void) {
	return (char*)__cbuild_int_flag_context.app_name;
}
void __cbuild_int_flag_help(const char* name) {
	__CBUILD_PRINTF("Usage: %s [OPTIONS]\n\n", name);
	cbuild_flag_print_help();
}
void (*cbuild_flag_help)(const char* app_name) = __cbuild_int_flag_help;
void __cbuild_int_flag_version(const char* name) {
	__CBUILD_PRINTF("%s - v1.0\n", name);
}
void (*cbuild_flag_version)(const char* app_name) = __cbuild_int_flag_version;
