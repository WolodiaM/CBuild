#include "Command.h"
#include "Common.h"
#include "StringBuilder.h"
#include "DynArray.h"
#include "Log.h"
#include "FS.h"
#include "Proc.h" 
CBUILDDEF cbuild_sb_t cbuild_cmd_to_sb(cbuild_cmd_t cmd) {
	cbuild_sb_t sb = {0};
	if(cmd.size < 1) {
		return sb;
	}
	for(size_t i = 0; i < cmd.size; i++) {
		const char* tmp = cmd.data[i];
		if(!strchr(tmp, ' ')) {
			cbuild_sb_append_cstr(&sb, tmp);
		} else {
			cbuild_sb_appendf(&sb, "\'%s\'", tmp);
		}
		if(i < cmd.size - 1) {
			cbuild_sb_append(&sb, ' ');
		}
	}
	return sb;
}
#if defined(CBUILD_API_POSIX) || defined(CBUILD_API_STRICT_POSIX)
	// We needs opts here, because I dont want to bloat function signature when I will add more call-level flags
	CBUILDDEF cbuild_proc_t __cbuild_cmd_run_opt(cbuild_cmd_t* cmd,
		struct cbuild_cmd_opts_t* opts, cbuild_fd_t fdstdin, 
		cbuild_fd_t fdstdout, cbuild_fd_t fdstderr) {
		// Get args
		cbuild_cmd_t argv = {0};
		cbuild_cmd_append_arr(&argv, cmd->data, cmd->size);
		cbuild_cmd_append(&argv, (char*)NULL);
		cbuild_proc_t proc = fork();
		if(proc < 0) {
			cbuild_log_error( "Could not create child process, error: \"%s\"",
				strerror(errno));
			cbuild_cmd_clear(&argv);
			return CBUILD_INVALID_PROC;
		}
		if(proc == 0) {
			fflush(NULL);
			// Setup stdin, stdout and stderr
			if(fdstdin != CBUILD_INVALID_FD) {
				if(dup2(fdstdin, STDIN_FILENO) < 0) {
					cbuild_log_error(
						"Could not redirect stdin inside of a child process, error: \"%s\"",
						strerror(errno));
					cbuild_cmd_clear(&argv);
					exit(1);
				}
			}
			if(fdstdout != CBUILD_INVALID_FD) {
				if(dup2(fdstdout, STDOUT_FILENO) < 0) {
					cbuild_log_error(
						"Could not redirect stdout inside of a child process, error: \"%s\"",
						strerror(errno));
					cbuild_cmd_clear(&argv);
					exit(1);
				}
			}
			if(fdstderr != CBUILD_INVALID_FD) {
				if(dup2(fdstderr, STDERR_FILENO) < 0) {
					cbuild_log_error(
						"Could not redirect stderr inside of a child process, error: \"%s\"",
						strerror(errno));
					cbuild_cmd_clear(&argv);
					exit(1);
				}
			}
			// Autokill
			if(opts->autokill) {
				#if defined(CBUILD_API_POSIX) && defined(CBUILD_OS_LINUX)
					prctl(PR_SET_PDEATHSIG, SIGKILL);
				#else
					cbuild_log_warn("Autokill is supported only on POSIX with Linux extensions!");
				#endif // Extension check
			}
			// Call command
			if(execvp(argv.data[0], (char* const*)argv.data) < 0) {
				cbuild_log_error("Could not execute command in child process, error: \"%s\"",
					strerror(errno));
				exit(1);
			}
			exit(0);
		}
		cbuild_cmd_clear(&argv);
		return proc;
	}
#endif // CBUILD_API_*
CBUILDDEF bool cbuild_cmd_run_opt(cbuild_cmd_t* cmd, struct cbuild_cmd_opts_t opts) {
	if(cmd->size == 0) {
		cbuild_log_error( "Empty command requested to be executed!");
		return false;
	}
	if(!opts.no_print_cmd) {
		cbuild_sb_t cmd_sb = cbuild_cmd_to_sb(*cmd);
		cbuild_log_trace("Running command '"CBuildSBFmt"'", CBuildSBArg(cmd_sb));
		cbuild_sb_clear(&cmd_sb);
	}
	// Open files
	cbuild_fd_t fdstdin = CBUILD_INVALID_FD;
	cbuild_fd_t fdstdout = CBUILD_INVALID_FD;
	cbuild_fd_t fdstderr = CBUILD_INVALID_FD;
	cbuild_fd_t file_stdin = CBUILD_INVALID_FD;
	cbuild_fd_t file_stdout = CBUILD_INVALID_FD;
	cbuild_fd_t file_stderr = CBUILD_INVALID_FD;
	if(opts.fdstdin != NULL) fdstdin = *opts.fdstdin;
	if(opts.fdstdout != NULL) fdstdout = *opts.fdstdout;
	if(opts.fdstderr != NULL) fdstderr = *opts.fdstderr;
	if(opts.file_stdin) {
		file_stdin = cbuild_fd_open_read(opts.file_stdin);
		fdstdin = file_stdin;
	}
	if(opts.file_stdout) {
		file_stdout = cbuild_fd_open_write(opts.file_stdout);
		fdstdout = file_stdout;
	}
	if(opts.file_stderr) {
		file_stderr = cbuild_fd_open_write(opts.file_stderr);
		fdstderr = file_stderr;
	}
	// Check for limits
	size_t proc_idx = 0;
	int code = 0;
	if(opts.procs != NULL && opts.async_threads != -1) {
		if(opts.async_threads == 0) opts.async_threads = cbuild_nproc() + 1;
		if((size_t)opts.async_threads == opts.procs->size) {
			proc_idx = (size_t)cbuild_procs_wait_any(*opts.procs, &code);
			if(code != 0) {
				goto cleanup;
			}
		}
	}
	// Call app
	cbuild_proc_t proc = __cbuild_cmd_run_opt(cmd, &opts, fdstdin, fdstdout, fdstderr);
	if (proc == CBUILD_INVALID_PROC) {
		code = -1;
		goto cleanup;
	}
	// Finish process creation
	if(opts.proc != NULL) {
		*opts.proc = proc;
	} else if(opts.procs != NULL) {
		if(opts.async_threads != -1) {
			if((size_t)opts.async_threads == opts.procs->size) {
				cbuild_da_set(opts.procs, proc_idx, proc);
			} else {
				cbuild_da_append(opts.procs, proc);
			}
		} else {
			cbuild_da_append(opts.procs, proc);
		}
	} else {
		code = cbuild_proc_wait_code(proc);
	}
cleanup:
	// Cleanup
	if(file_stdin != CBUILD_INVALID_FD) {
		cbuild_fd_close(file_stdin);
		fdstdin = CBUILD_INVALID_FD;
	}
	if(file_stdout != CBUILD_INVALID_FD) {
		cbuild_fd_close(file_stdout);
		fdstdout = CBUILD_INVALID_FD;
	}
	if(file_stderr != CBUILD_INVALID_FD) {
		cbuild_fd_close(file_stderr);
		fdstderr = CBUILD_INVALID_FD;
	}
	if(!opts.no_reset) {
		cmd->size = 0;
		if(fdstdin != CBUILD_INVALID_FD) cbuild_fd_close(fdstdin);
		if(fdstdout != CBUILD_INVALID_FD) cbuild_fd_close(fdstdout);
		if(fdstderr != CBUILD_INVALID_FD) cbuild_fd_close(fdstderr);
	}
	return code == 0;
}
