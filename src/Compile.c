//! License: `GPL-3.0-or-later`.

#include "Compile.h"
#include "Common.h"
#include "Log.h"
#include "StringBuilder.h"
#include "DynArray.h"
#include "FS.h"
#include "Command.h"
CBUILDDEF void __cbuild_int_compile_mark_exec(const char* file);
#if defined(CBUILD_API_POSIX) || defined(CBUILD_API_STRICT_POSIX)
	CBUILDDEF void __cbuild_int_compile_mark_exec(const char* file) {
		if(chmod(file, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH |
				S_IXOTH) != 0) {
			cbuild_log_error("Could not mark file \"%s\" as executable, error: \"%s\".",
				file, strerror(errno));
		}
	}
#endif // CBUILD_API_*
void (*cbuild_selfrebuild_hook)(cbuild_cmd_t* cmd) = NULL;
CBUILDDEF void __cbuild_selfrebuild(int argc, char** argv,
	const char* argv0_path, cbuild_cmd_t files) {
	// Prepare paths
	const char* source = files.data[0];
	const char* output = argv0_path;
	cbuild_sb_t backup = {0};
	cbuild_sb_append_cstr(&backup, output);
	cbuild_sb_append_cstr(&backup, ".old");
	cbuild_sb_append_null(&backup);
	// Check if recompilation is needed
	int cond = cbuild_compare_mtime_many(output, files.data, files.size);
	if(cond < 0) {
		cbuild_log_error("Error while performing self-rebuild.");
		cbuild_sb_clear(&backup);
		exit(1);
	}
	if(cond == 0) {
		cbuild_sb_clear(&backup);
		return;
	}
	cbuild_log_info("Rebuilding CBuild buildscript.");
	// Make backup of a binary
	if(!cbuild_file_rename(output, backup.data)) {
		if(!cbuild_file_check(output)) {
			cbuild_log_warn("File %s does not exists, skipping rename step", output);
		} else {
			cbuild_log_error("Could not rename old buildscript, aborting.");
			exit(1);
		}
	}
	// Actual rebuild
	cbuild_cmd_t cmd = {0};
	cbuild_cmd_append_many(&cmd, CBUILD_CC, CBUILD_SELFREBUILD_ARGS);
	if(cbuild_selfrebuild_hook != NULL) {
		cbuild_selfrebuild_hook(&cmd);
	}
	cbuild_cmd_append_many(&cmd, "-o", output, source);
	if(!cbuild_cmd_run(&cmd)) {
		cbuild_file_rename(backup.data, output);
		cbuild_log_error("----------------------------------------");
		cbuild_log_error("Rebuild failed.");
		cbuild_log_error("----------------------------------------");
		return; // If compilation failed the let old version run
	}
	// Re-exec into new binary
	cbuild_sb_clear(&backup);
	__cbuild_int_compile_mark_exec(output);
	// Here real argv[0] used because this can be executed (it was just executed)
	// and argv0_path can be absolute path, which will look bad inside a command
	cbuild_cmd_append_arr(&cmd, argv, (size_t)argc);
	if(!cbuild_cmd_run(&cmd)) {
		// NOTE: This hides app exit code. But this is unfixable in current API
		exit(1);
	}
	exit(0);
}
#if defined(CBUILD_API_POSIX) || defined(CBUILD_API_STRICT_POSIX)
	CBUILDDEF int cbuild_compare_mtime(const char* output, const char* input) {
		struct stat statbuff;
		if(stat(input, &statbuff) < 0) {
			cbuild_log_error("Could not stat file \"%s\", error: \"%s\"",
				input, strerror(errno));
			return -1;
		}
		time_t in_mtime = statbuff.st_mtime;
		if(stat(output, &statbuff) < 0) {
			if(errno == ENOENT) {
				return 1;
			}
			cbuild_log_error("Could not stat file \"%s\", error: \"%s\"",
				output, strerror(errno));
			return -1;
		}
		if(in_mtime > statbuff.st_mtime) {
			return 1;
		} else {
			return 0;
		}
	}
#endif //CBUILD_API_*
CBUILDDEF int cbuild_compare_mtime_many(const char* output, const char** inputs,
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
