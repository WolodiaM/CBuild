#include "Proc.h"
#include "Common.h"
#include "Log.h"
#include "DynArray.h"
#if defined(CBUILD_API_POSIX) || defined(CBUILD_API_STRICT_POSIX)
	CBUILDDEF int cbuild_proc_wait_code(cbuild_proc_t proc) {
		if(proc == CBUILD_INVALID_PROC) {
			return INT_MIN;
		}
		while(true) {
			int status = 0;
			errno = 0;
			if(waitpid(proc, &status, 0) < 0) {
				if(errno ==	ECHILD) {
					return INT_MAX;
				} else if (errno == EINTR) {
					errno = 0;
				} else {
					cbuild_log_error("Could not wait for child process (pid %d), error: \"%s\"",
						proc, strerror(errno));
				}
				errno = 0;
			} else {
				if(WIFEXITED(status)) {
					int code = WEXITSTATUS(status);
					return code;
				}
				if(WIFSIGNALED(status)) {
					cbuild_log_error("Process (pid %d) was terminated by signal \"%d\"",
						proc, WTERMSIG(status));
					return -WTERMSIG(status);
				}
			}
		}
	}
	CBUILDDEF ssize_t cbuild_procs_wait_any(cbuild_proclist_t procs, int* code) {
		if(procs.size == 0) {
			return -1;
		}
		while(true) {
			for(size_t i = 0; i < procs.size; i++) {
				cbuild_proc_t proc = procs.data[i];
				int status = 0;
				errno = 0;
				int ret = waitpid(proc, &status, WNOHANG);
				if(ret < 0) {
					if(errno ==	ECHILD) {
						return INT_MAX;
					} else {
						cbuild_log_error(
							"Could not wait for child process (pid %d), error: \"%s\"", proc,
							strerror(errno));
					}
					errno = 0;
				} else if(ret > 0) {
					if(WIFEXITED(status)) {
						if(code != NULL) *code = WEXITSTATUS(status);
						return (ssize_t)i;
					}
					if(WIFSIGNALED(status)) {
						cbuild_log_error(
							"Process (pid %d) was terminated by signal \"%d\"", proc,
							WTERMSIG(status));
						if(code != NULL) *code = -WTERMSIG(status);
						return (ssize_t)i;
					}
				}
			}
			const struct timespec duration = {
				.tv_sec = 0,
				.tv_nsec = 1000*100,
			};
			nanosleep(&duration, NULL);
		}
		return -1;
	}
	CBUILDDEF bool cbuild_proc_is_running(cbuild_proc_t proc) {
		if(proc <= 0) return false;
		return kill(proc, 0) <= 0;
	}
	CBUILDDEF cbuild_proc_ptr_t cbuild_proc_malloc(size_t n) {
		// Reasoning: Tooo big benefits
		#if defined(MAP_ANONYMOUS) // Also used on strict POSIX
			void* ptr =
				mmap(NULL, n, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
		#elif defined(MAP_ANON) // Also used on strict POSIX
			void* ptr =
				mmap(NULL, n, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
		#else // Strict POSIX fallback
			int (*shm_open)(const char* name, int oflag, mode_t mode) =
				(int (*)(const char*, int, mode_t))dlsym(RTLD_DEFAULT, "shm_open");
			if(shm_open == NULL) {
				if(!dlopen("librt.so", RTLD_LAZY | RTLD_GLOBAL)) {
					if(!dlopen("librt.so.1", RTLD_LAZY | RTLD_GLOBAL)) {
						CBUILD_UNREACHABLE("libc dont expose 'shm_open' and librt not found!");
					}
				}
				shm_open = (int (*)(const char*, int, mode_t))dlsym(RTLD_DEFAULT, "shm_open");
			}
			int (*shm_unlink)(const char* name) =
				(int (*)(const char*))dlsym(RTLD_DEFAULT, "shm_unlink");
			int shmemfd = shm_open("/cbuild-proc-malloc-shmem", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
			if(shmemfd == -1) {
				return (cbuild_proc_ptr_t) {
					.ptr = NULL, .size = 0
				};
			}
			if(ftruncate(shmemfd, (long)n) == -1) {
				shm_unlink("/cbuild-proc-malloc-shmem");
				close(shmemfd);
				return (cbuild_proc_ptr_t) {
					.ptr = NULL, .size = 0
				};
			}
			void* ptr =
				mmap(NULL, n, PROT_READ | PROT_WRITE, MAP_SHARED, shmemfd, 0);
			if(ptr == MAP_FAILED) {
				close(shmemfd);
				shm_unlink("/cbuild-proc-malloc-shmem");
				return (cbuild_proc_ptr_t) {
					.ptr = NULL, .size = 0
				};
			} else {
				close(shmemfd);
				shm_unlink("/cbuild-proc-malloc-shmem");
				return (cbuild_proc_ptr_t) {
					.ptr = ptr, .size = n
				};
			}
		#endif // Extension check
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
	CBUILDDEF void cbuild_proc_free(cbuild_proc_ptr_t ptr) {
		if(ptr.ptr != NULL) {
			munmap(ptr.ptr, ptr.size);
		}
	}
	CBUILDDEF cbuild_proc_t cbuild_proc_start(int (*callback)(void* context),
		void* context) {
		cbuild_proc_t proc = fork();
		if(proc < 0) {
			cbuild_log_error("Could not create child process, error: \"%s\"",
				strerror(errno));
			return CBUILD_INVALID_PROC;
		}
		if(proc == 0) {
			int code = callback(context);
			exit(code);
		}
		return proc;
	}
#endif // CBUILD_API_*
CBUILDDEF bool cbuild_proc_wait(cbuild_proc_t proc) {
	return cbuild_proc_wait_code(proc) == 0;
}
CBUILDDEF bool cbuild_procs_wait(cbuild_proclist_t procs) {
	bool ret = true;
	cbuild_da_foreach(procs, proc) {
		if(!cbuild_proc_wait(*proc)) ret = false;
	}
	return ret;
}
#if defined(CBUILD_API_POSIX) || defined(CBUILD_API_STRICT_POSIX)
	CBUILDDEF int cbuild_nproc(void) {
		// Reasoning: no alternatives are available and benefits are high-enough to go slightly off-spec
		#if defined(_SC_NPROCESSORS_ONLN) // Also used on strict POSIX
			return abs((int)sysconf(_SC_NPROCESSORS_ONLN));
		#else
			return 1;
		#endif // Extension check
	}
#endif // CBUILD_API_*
