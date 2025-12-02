/**
 * @file cbuild.c
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Build script for CBuild
 * Replaces old build.sh. So this repo will becode C+Markdown.
 * And a lot of generated HTML ;)
 *
 * @date 2025-09-12
 * @copyright (C) 2025 WolodiaM
 * @license GPL-3.0-or-later
 *
 * Copyright (C) 2025 WolodiaM
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
// TODO: Use pre-compiled header for cbuild
// -x=c-header -o cbuild.h.pch
// Linking will be done automatically

// Includes
#define CBUILDDEF static inline
#define CBUILD_LOG_MIN_LEVEL CBUILD_LOG_TRACE
#include "cbuild.h"
#define GLOB_IMPLEMENTATION
#include "rglob.h"
#include "framework.h"
// Test system globals
typedef enum test_status_t {
	TEST_SUCCEED = 0,     // Should be 0
	TEST_FAILED,          // 1 is expected here
	TEST_SKIPPED,         // Should be placed somewhere
	TEST_MEMCHECK_FAILED, // Now errors can just continue
	TEST_COMP_FAILED,     // Compilation failed
	TEST_STATUS_COUNT = 5,
} test_status_t;
static const char* TEST_STATUS_REPORTS[] = {
	[TEST_SUCCEED]         = CBUILD_TERM_FG(CBUILD_TERM_GREEN)  "+"CBUILD_TERM_RESET,
	[TEST_FAILED]          = CBUILD_TERM_FG(CBUILD_TERM_RED)    "-"CBUILD_TERM_RESET,
	[TEST_SKIPPED]         = CBUILD_TERM_FG(CBUILD_TERM_BRBLACK)"."CBUILD_TERM_RESET,
	[TEST_MEMCHECK_FAILED] = CBUILD_TERM_FG(CBUILD_TERM_MAGENTA)"M"CBUILD_TERM_RESET,
	[TEST_COMP_FAILED]     = CBUILD_TERM_FG(CBUILD_TERM_YELLOW) "!"CBUILD_TERM_RESET,
};
static_assert(TEST_STATUS_COUNT == cbuild_arr_len(TEST_STATUS_REPORTS),
	"TEST_STATUS_REPORS should have one report per test_status_t entry");
enum {
	TPL_X86_64_LINUX_GLIBC_GCC = 0,
	TPL_X86_64_LINUX_GLIBC_CLANG,
	TPL_X86_64_LINUX_MUSL_GCC,
	TPL_X86_64_POSIX_GCC,
	TPL_X86_64_POSIX_CLANG,
	TPL_COUNT,
};
#define TPL_MASK(idx) (1u << (idx))
#define TPLM_ALL      ((1u << TPL_COUNT) - 1u)
enum {
	TPLM_X86_64_LINUX_GLIBC_GCC   = TPL_MASK(TPL_X86_64_LINUX_GLIBC_GCC),
	TPLM_X86_64_LINUX_GLIBC_CLANG = TPL_MASK(TPL_X86_64_LINUX_GLIBC_CLANG),
	TPLM_X86_64_LINUX_MUSL_GCC    = TPL_MASK(TPL_X86_64_LINUX_MUSL_GCC),
	TPLM_X86_64_POSIX_GCC         = TPL_MASK(TPL_X86_64_POSIX_GCC),
	TPLM_X86_64_POSIX_CLANG       = TPL_MASK(TPL_X86_64_POSIX_CLANG),
};
static_assert(TPL_COUNT == 5, "Enum TPLM_* expects 5 test platforms.");
typedef struct test_case_t {
	union {
		uint32_t platforms;
	};
	union {
		struct {
			uint32_t group : 1;
			uint32_t       : 31;
		};
		uint32_t flags;
	};
	const char* file;
	cbuild_arglist_t argv; // 'capacity' can be unset. Runner guarantee to not update this DA
	cbuild_cmd_t cargs;    // 'capacity' can be unset. Runner guarantee to not update this DA
} test_case_t;
volatile size_t TPL_RUN_REGISTERED_CURR = 0;
volatile const char* TPL_RUN_REGISTERED_GROUP = NULL;
#define	TPL_RUN_REGISTERED(run_callback, skip_callback)                        \
	for(size_t tpl_idx = 0; tpl_idx < TPL_COUNT; tpl_idx++) {                    \
		if(test.platforms & TPL_MASK(tpl_idx)) {                                   \
			cbuild_assert(tpl_idx < cbuild_arr_len(TPL_RUNNERS),                     \
				"Invalid test platform specified!");                                   \
			size_t checkpoint = cbuild_temp_checkpoint();                            \
			TPL_RUN_REGISTERED_CURR = tpl_idx;                                       \
			test_status_t status = TPL_RUNNERS[tpl_idx](test);                       \
			cbuild_temp_reset(checkpoint);                                           \
			run_callback;                                                            \
		} else {                                                                   \
			skip_callback;                                                           \
		}                                                                          \
	}
test_case_t TESTS[] = {
	{
		.file = "Command",
		.group = true,
	},
	{
		.file = "cmd_to_sb",
		.platforms = TPLM_ALL,
	},
	{
		.file = "run_sync",
		.platforms = TPLM_ALL,
	},
	{
		.file = "run_async",
		.platforms = TPLM_ALL,
	},
	{
		.file = "redirect",
		.platforms = TPLM_ALL,
	},
	{
		.file = "common",
		.group = true,
	},
	{
		.file = "arr",
		.platforms = TPLM_ALL,
	},
	{
		.file = "shift",
		.platforms = TPLM_ALL,
	},
	{
		.file = "Compile",
		.group = true,
	},
	{
		.file = "mtime_single",
		.platforms = TPLM_ALL,
	},
	{
		.file = "mtime_multi",
		.platforms = TPLM_ALL,
	},
	{
		.file = "DynArray",
		.group = true,
	},
	{
		.file = "alloc",
		.platforms = TPLM_ALL,
		.cargs = {.data = (const char*[]){
			"-DCBUILD_INIT_CAPACITY=2ul",
		}, .size = 1},
	},
	{
		.file = "append_single",
		.platforms = TPLM_ALL,
	},
	{
		.file = "append_arr",
		.platforms = TPLM_ALL,
	},
	{
		.file = "append_multi",
		.platforms = TPLM_ALL,
	},
	{
		.file = "set",
		.platforms = TPLM_ALL,
	},
	{
		.file = "get",
		.platforms = TPLM_ALL,
	},
	{
		.file = "remove_ordered",
		.platforms = TPLM_ALL,
	},
	{
		.file = "remove_unordered",
		.platforms = TPLM_ALL,
	},
	{
		.file = "foreach",
		.platforms = TPLM_ALL,
	},
	{
		.file = "StringBuilder",
		.group = true,
	},
	{
		.file = "alloc",
		.platforms = TPLM_ALL,
		.cargs = {.data = (const char*[]){
			"-DCBUILD_INIT_CAPACITY=2ul",
		}, .size = 1},
	},
	{
		.file = "append_single",
		.platforms = TPLM_ALL,
	},
	{
		.file = "append_arr",
		.platforms = TPLM_ALL,
	},
	{
		.file = "append_multi",
		.platforms = TPLM_ALL,
	},
	{
		.file = "append_cstr",
		.platforms = TPLM_ALL,
	},
	{
		.file = "append_utf8",
		.platforms = TPLM_ALL,
	},
	{
		.file = "append_fmt",
		.platforms = TPLM_ALL,
	},
	{
		.file = "set",
		.platforms = TPLM_ALL,
	},
	{
		.file = "get",
		.platforms = TPLM_ALL,
	},
	{
		.file = "remove",
		.platforms = TPLM_ALL,
	},
	{
		.file = "foreach",
		.platforms = TPLM_ALL,
	},
	{
		.file = "cmp",
		.platforms = TPLM_ALL,
	},
	{
		.file = "SB-SV",
		.group = true,
	},
	{
		.file = "interop",
		.platforms = TPLM_ALL,
	},
	{
		.file = "Stack",
		.group = true,
	},
	{
		.file = "alloc",
		.platforms = TPLM_ALL,
		.cargs = {.data = (const char*[]){
			"-DCBUILD_INIT_CAPACITY=2ul",
		}, .size = 1},
	},
	{
		.file = "push",
		.platforms = TPLM_ALL,
	},
	{
		.file = "pop",
		.platforms = TPLM_ALL,
	},
	{
		.file = "StringView",
		.group = true,
	},
	{
		.file = "create",
		.platforms = TPLM_ALL,
	},
	{
		.file = "trim",
		.platforms = TPLM_ALL,
	},
	{
		.file = "cmp",
		.platforms = TPLM_ALL,
	},
	{
		.file = "chop",
		.platforms = TPLM_ALL,
	},
	{
		.file = "search",
		.platforms = TPLM_ALL,
	},
	{
		.file = "utf8_valid",
		.platforms = TPLM_ALL,
	},
	{
		.file = "utf8_len",
		.platforms = TPLM_ALL,
	},
	{
		.file = "utf8_chop",
		.platforms = TPLM_ALL,
	},
	{
		.file = "Proc",
		.group = true,
	},
	{
		.file = "ctrl",
		.platforms = TPLM_ALL,
	},
	{
		.file = "Map",
		.group = true,
	},
	{
		.file = "alloc",
		.platforms = TPLM_ALL,
	},
	{
		.file = "write_ii",
		.platforms = TPLM_ALL,
	},
	{
		.file = "write_ci",
		.platforms = TPLM_ALL,
	},
	{
		.file = "read_ii",
		.platforms = TPLM_ALL,
	},
	{
		.file = "read_ci",
		.platforms = TPLM_ALL,
	},
	{
		.file = "remove_ii",
		.platforms = TPLM_ALL,
	},
	{
		.file = "remove_ci",
		.platforms = TPLM_ALL,
	},
	{
		.file = "iter_ii",
		.platforms = TPLM_ALL,
	},
	{
		.file = "iter_ci",
		.platforms = TPLM_ALL,
	},
	{
		.file = "FS",
		.group = true,
	},
	{
		.file = "file_read",
		.platforms = TPLM_ALL,
	},
	{
		.file = "file_write",
		.platforms = TPLM_ALL,
	},
	{
		.file = "file_copy",
		.platforms = TPLM_ALL,
		.cargs = {.data = (const char*[]){
			"-DCBUILD_TMP_BUFF_SIZE=64",
		}, .size = 1},
	},
	{
		.file = "file_copy_big",
		.platforms = TPLM_ALL,
		.cargs = {.data = (const char*[]){
			"-DCBUILD_TMP_BUFF_SIZE=64",
		}, .size = 1},
	},
	{
		.file = "file_check",
		.platforms = TPLM_ALL,
	},
	{
		.file = "file_move",
		.platforms = TPLM_ALL,
	},
	{
		.file = "file_rename",
		.platforms = TPLM_ALL,
	},
	{
		.file = "file_remove",
		.platforms = TPLM_ALL,
	},
};
static const char* TPL_NAMES[] = {
	[TPL_X86_64_LINUX_GLIBC_GCC]   = "x86_64-linux-glibc-gcc",
	[TPL_X86_64_LINUX_GLIBC_CLANG] = "x86_64-linux-glibc-clang",
	[TPL_X86_64_LINUX_MUSL_GCC]    = "x86_64-linux-musl-gcc",
	[TPL_X86_64_POSIX_GCC]         = "x86_64-posix-gcc",
	[TPL_X86_64_POSIX_CLANG]       = "x86_64-posix-clang",
};
static_assert(TPL_COUNT == cbuild_arr_len(TPL_NAMES),
	"TPL_NAMES expect 5 test platforms.");
// Test runner
#define test_log_failed(msg, ...)                                              \
	__cbuild_int_log(CBUILD_TERM_FG(CBUILD_TERM_RED)"[FAILED]"                   \
		CBUILD_TERM_RESET" ",                                                      \
		msg __VA_OPT__(,) __VA_ARGS__)
#define test_log_success(msg, ...)                                             \
	__cbuild_int_log(CBUILD_TERM_FG(CBUILD_TERM_GREEN)"[SUCCESS]"                \
		CBUILD_TERM_RESET" "   ,                                                   \
		msg __VA_OPT__(,) __VA_ARGS__)
#define test_log_start(msg, ...)                                               \
	__cbuild_int_log(CBUILD_TERM_FG(CBUILD_TERM_MAGENTA)"[START]"                \
		CBUILD_TERM_RESET" "   ,                                                   \
		msg __VA_OPT__(,) __VA_ARGS__)
void test_cmd_append_memcheck(cbuild_cmd_t* cmd) {
	cbuild_cmd_append(cmd, "valgrind");
	cbuild_cmd_append(cmd, "--error-exitcode=255");
	cbuild_cmd_append(cmd, "--track-origins=yes");
	cbuild_cmd_append_many(cmd, "--leak-check=full", "--show-leak-kinds=all");
	cbuild_cmd_append(cmd, "--errors-for-leak-kinds=definite,indirect,possible");
}
void test_cmd_append_cc_base(test_case_t test, cbuild_cmd_t* cmd) {
	cbuild_cmd_append_many(cmd, CBUILD_CARGS_WARN, CBUILD_CARGS_WERROR);
	cbuild_cmd_append_many(cmd, CBUILD_CARGS_INCLUDE("framework.h"));
	cbuild_cmd_append_many(cmd, "-fmacro-prefix-map=tests/=");
	cbuild_cmd_append_many(cmd,
		cbuild_temp_sprintf("-DTEST_RUN_PLATFORM=\"%s\"",
			TPL_NAMES[TPL_RUN_REGISTERED_CURR]));
	cbuild_cmd_append(cmd, "-ggdb");
	cbuild_cmd_append_arr(cmd, test.cargs.data, test.cargs.size);
}
static_assert(TEST_STATUS_COUNT == 5, "Enum test_status_t expected to hold 5 statuses.");
test_status_t test_case_run_memcheck(test_case_t test, const char* oname) {
	cbuild_cmd_t cmd = {0};
	cbuild_log_trace("Running test \"%s\"...", test.file);
	test_cmd_append_memcheck(&cmd);
	cbuild_cmd_append(&cmd, oname);
	cbuild_cmd_append_arr(&cmd, test.argv.data, test.argv.size);
	cbuild_proc_t proc;
	cbuild_cmd_run(&cmd, .proc = &proc);
	cbuild_cmd_clear(&cmd);
	int code = cbuild_proc_wait_code(proc);
	if(code == 255) {
		test_log_failed("Test \"%s\" leaks memory.", test.file);
		return TEST_MEMCHECK_FAILED;
	} else if(code != 0) {
		test_log_failed("Test \"%s\" failed.", test.file);
		cbuild_file_remove(cbuild_temp_sprintf("vgcore.%d", proc));
		return TEST_FAILED;
	} else {
		test_log_success("Test \"%s\" succeed.", test.file);
		return TEST_SUCCEED;
	}
}
static_assert(TEST_STATUS_COUNT == 5, "Enum test_status_t expected to hold 5 statuses.");
test_status_t test_x86_64_linux_glibc_gcc(test_case_t test) {
	test_log_start("Running test case \"%s\"", test.file);
	cbuild_log_info("Platform: Linux/glibc, Arch: x86_64, Compiler: gcc");
	cbuild_log_trace("Building test \"%s\"...", test.file);
	cbuild_cmd_t cmd = {0};
	const char*	fname =	cbuild_temp_sprintf(TEST_FOLDER"/%s_%s.c",
		TPL_RUN_REGISTERED_GROUP, test.file);
	const char*	oname =
		cbuild_temp_sprintf(BUILD_FOLDER"/test_x86_64_linux_glibc_gcc_%s.run", test.file);
	cbuild_cmd_append(&cmd, "gcc");
	test_cmd_append_cc_base(test, &cmd);
	cbuild_cmd_append_many(&cmd, CBUILD_CC_OUT, oname);
	cbuild_cmd_append(&cmd, fname);
	if(!cbuild_cmd_run(&cmd)) {
		test_log_failed("Test \"%s\" failed to build.", test.file);
		cbuild_cmd_clear(&cmd);
		return TEST_COMP_FAILED;
	}
	cbuild_log_trace("Test \"%s\" built successfully.", test.file);
	cbuild_cmd_clear(&cmd);
	return test_case_run_memcheck(test, oname);
}
static_assert(TEST_STATUS_COUNT == 5, "Enum test_status_t expected to hold 5 statuses.");
test_status_t test_x86_64_linux_glibc_clang(test_case_t test) {
	test_log_start("Running test case \"%s\"", test.file);
	cbuild_log_info("Platform: Linux/glibc, Arch: x86_64, Compiler: clang");
	cbuild_log_trace("Building test \"%s\"...", test.file);
	cbuild_cmd_t cmd = {0};
	const char*	fname =	cbuild_temp_sprintf(TEST_FOLDER"/%s_%s.c",
		TPL_RUN_REGISTERED_GROUP, test.file);
	const char*	oname =
		cbuild_temp_sprintf(BUILD_FOLDER"/test_x86_64_linux_glibc_clang_%s.run", test.file);
	cbuild_cmd_append(&cmd, "clang");
	test_cmd_append_cc_base(test, &cmd);
	cbuild_cmd_append_many(&cmd, CBUILD_CC_OUT, oname);
	cbuild_cmd_append(&cmd, fname);
	if(!cbuild_cmd_run(&cmd)) {
		test_log_failed("Test \"%s\" failed to build.", test.file);
		cbuild_cmd_clear(&cmd);
		return TEST_COMP_FAILED;
	}
	cbuild_log_trace("Test \"%s\" built successfully.", test.file);
	cbuild_cmd_clear(&cmd);
	return test_case_run_memcheck(test, oname);
}
static_assert(TEST_STATUS_COUNT == 5, "Enum test_status_t expected to hold 5 statuses.");
test_status_t test_x86_64_linux_musl_gcc(test_case_t test) {
	test_log_start("Running test case \"%s\"", test.file);
	cbuild_log_info("Platform: Linux/musl, Arch: x86_64, Compiler: gcc");
	cbuild_log_trace("Building test \"%s\"...", test.file);
	cbuild_cmd_t cmd = {0};
	const char*	fname =	cbuild_temp_sprintf(TEST_FOLDER"/%s_%s.c",
		TPL_RUN_REGISTERED_GROUP, test.file);
	const char*	oname =
		cbuild_temp_sprintf(BUILD_FOLDER"/test_x86_64_linux_musl_gcc_%s.run", test.file);
	cbuild_cmd_append(&cmd, "musl-gcc");
	test_cmd_append_cc_base(test, &cmd);
	cbuild_cmd_append_many(&cmd, CBUILD_CC_OUT, oname);
	cbuild_cmd_append(&cmd, fname);
	if(!cbuild_cmd_run(&cmd)) {
		test_log_failed("Test \"%s\" failed to build.", test.file);
		cbuild_cmd_clear(&cmd);
		return TEST_COMP_FAILED;
	}
	cbuild_log_trace("Test \"%s\" built successfully.", test.file);
	cbuild_log_trace("Running test \"%s\"...", test.file);
	cbuild_cmd_append(&cmd, oname);
	cbuild_cmd_append_arr(&cmd, test.argv.data, test.argv.size);
	if(!cbuild_cmd_run(&cmd)) {
		test_log_failed("Test \"%s\" failed.", test.file);
		cbuild_cmd_clear(&cmd);
		return TEST_FAILED;
	}
	test_log_success("Test \"%s\" succeed.", test.file);
	cbuild_cmd_clear(&cmd);
	return TEST_SUCCEED;
}
static_assert(TEST_STATUS_COUNT == 5, "Enum test_status_t expected to hold 5 statuses.");
test_status_t test_x86_64_posix_gcc(test_case_t test) {
	test_log_start("Running test case \"%s\"", test.file);
	cbuild_log_info("Platform: Posix, Arch: x86_64, Compiler: gcc");
	cbuild_log_trace("Building test \"%s\"...", test.file);
	cbuild_cmd_t cmd = {0};
	const char*	fname =	cbuild_temp_sprintf(TEST_FOLDER"/%s_%s.c",
		TPL_RUN_REGISTERED_GROUP, test.file);
	const char*	oname =
		cbuild_temp_sprintf(BUILD_FOLDER"/test_x86_64_posix_gcc_%s.run", test.file);
	cbuild_cmd_append(&cmd, "gcc");
	test_cmd_append_cc_base(test, &cmd);
	cbuild_cmd_append_many(&cmd, "-DCBUILD_API_DEFINED", "-DCBUILD_API_STRICT_POSIX");
	cbuild_cmd_append_many(&cmd, CBUILD_CC_OUT, oname);
	cbuild_cmd_append(&cmd, fname);
	if(!cbuild_cmd_run(&cmd)) {
		test_log_failed("Test \"%s\" failed to build.", test.file);
		cbuild_cmd_clear(&cmd);
		return TEST_COMP_FAILED;
	}
	cbuild_log_trace("Test \"%s\" built successfully.", test.file);
	cbuild_cmd_clear(&cmd);
	return test_case_run_memcheck(test, oname);
}
static_assert(TEST_STATUS_COUNT == 5, "Enum test_status_t expected to hold 5 statuses.");
test_status_t test_x86_64_posix_clang(test_case_t test) {
	test_log_start("Running test case \"%s\"", test.file);
	cbuild_log_info("Platform: Posix, Arch: x86_64, Compiler: clang");
	cbuild_log_trace("Building test \"%s\"...", test.file);
	cbuild_cmd_t cmd = {0};
	const char*	fname =	cbuild_temp_sprintf(TEST_FOLDER"/%s_%s.c",
		TPL_RUN_REGISTERED_GROUP, test.file);
	const char*	oname =
		cbuild_temp_sprintf(BUILD_FOLDER"/test_x86_64_posix_clang_%s.run", test.file);
	cbuild_cmd_append(&cmd, "clang");
	test_cmd_append_cc_base(test, &cmd);
	cbuild_cmd_append_many(&cmd, "-DCBUILD_API_DEFINED", "-DCBUILD_API_STRICT_POSIX");
	cbuild_cmd_append_many(&cmd, CBUILD_CC_OUT, oname);
	cbuild_cmd_append(&cmd, fname);
	if(!cbuild_cmd_run(&cmd)) {
		test_log_failed("Test \"%s\" failed to build.", test.file);
		cbuild_cmd_clear(&cmd);
		return TEST_COMP_FAILED;
	}
	cbuild_log_trace("Test \"%s\" built successfully.", test.file);
	cbuild_cmd_clear(&cmd);
	return test_case_run_memcheck(test, oname);
}
static test_status_t (*TPL_RUNNERS[])(test_case_t test) = {
	[TPL_X86_64_LINUX_GLIBC_GCC]   = test_x86_64_linux_glibc_gcc,
	[TPL_X86_64_LINUX_GLIBC_CLANG] = test_x86_64_linux_glibc_clang,
	[TPL_X86_64_LINUX_MUSL_GCC]    = test_x86_64_linux_musl_gcc,
	[TPL_X86_64_POSIX_GCC]         = test_x86_64_posix_gcc,
	[TPL_X86_64_POSIX_CLANG]       = test_x86_64_posix_clang,
};
static_assert(TPL_COUNT == cbuild_arr_len(TPL_RUNNERS),
	"TPL_RUNNERS expect 5 test platforms.");
// Single-case runner
bool test_case(test_case_t test) {
	bool failed = false;
	TPL_RUN_REGISTERED(if(status != TEST_SUCCEED) failed = true,);
	return failed;
}
// Full runner
bool test(void) {
	// Prepare output array
	size_t testcnt = 0;
	for(size_t i = 0; i < cbuild_arr_len(TESTS); i++) testcnt++;
	test_status_t* statuses = calloc(testcnt * TPL_COUNT,  sizeof(test_status_t));
	inline test_status_t* get_status(size_t test, size_t tpl) {
		return &statuses[test * TPL_COUNT + tpl];
	}
	// Failure marker
	bool failed = false;
	// Silence output
	cbuild_fd_t dev_null = cbuild_fd_open_write("/dev/null");
	cbuild_fd_t fdstdout = dup(STDOUT_FILENO);
	cbuild_fd_t fdstderr = dup(STDERR_FILENO);
	fflush(stdout);
	fflush(stderr);
	dup2(dev_null, STDOUT_FILENO);
	dup2(dev_null, STDERR_FILENO);
	// Run tests
	for(size_t test_idx = 0; test_idx < cbuild_arr_len(TESTS); test_idx++) {
		test_case_t test = TESTS[test_idx];
		if (test.group) {
			TPL_RUN_REGISTERED_GROUP = (char*)test.file;
			cbuild_fd_write(fdstdout, test.file, strlen(test.file));
			cbuild_fd_write(fdstdout, "\n", 1);
			// Just pad things
			for(size_t tpl_idx = 0; tpl_idx < TPL_COUNT; tpl_idx++) {
				*get_status(test_idx, tpl_idx) = TEST_SKIPPED;
			}
			continue;
		}
		// To see progress
		cbuild_fd_write(fdstdout, "  ", 2);
		cbuild_fd_write(fdstdout, test.file, strlen(test.file));
		cbuild_fd_write(fdstdout, "\n", 1);
		// Test platforms
		TPL_RUN_REGISTERED(*get_status(test_idx, tpl_idx) = status,
			*get_status(test_idx, tpl_idx) = TEST_SKIPPED);
	}
	// Restore output
	fflush(stdout);
	fflush(stderr);
	dup2(fdstdout, STDOUT_FILENO);
	dup2(fdstderr, STDERR_FILENO);
	close(dev_null);
	close(fdstdout);
	close(fdstderr);
	// Print report
	size_t name_len = 0;
	for(size_t i = 0; i < cbuild_arr_len(TESTS); i++) {
		if(TESTS[i].group) name_len = CBUILD_MAX(name_len, strlen(TESTS[i].file));
		else name_len = CBUILD_MAX(name_len, strlen(TESTS[i].file) + 2);
	}
	// Top platform names
	for(size_t i = 0; i < TPL_COUNT; i++) {
		printf("%*s", (int)name_len, "");
		for(size_t j = 0; j < i; j++) printf(" | ");
		printf(" /-");
		for(size_t j = 0; j < TPL_COUNT - i; j++) printf("---");
		printf(" "CBUILD_TERM_FG(CBUILD_TERM_BLUE)"%s"CBUILD_TERM_RESET"\n",
			TPL_NAMES[i]);
	}
	printf("%*s", (int)name_len, "");
	for(size_t i = 0; i < TPL_COUNT; i++) printf(" | ");
	printf("\n");
	// Test cases
	for(size_t i = 0; i < cbuild_arr_len(TESTS); i++) {
		if(TESTS[i].group) {
			printf(CBUILD_TERM_FG(CBUILD_TERM_BRCYAN)"%-*s"CBUILD_TERM_RESET,
				(int)name_len, TESTS[i].file);
			for(size_t j = 0; j < TPL_COUNT; j++) printf(" | ");
		} else {
			printf(CBUILD_TERM_FG(CBUILD_TERM_CYAN)"  %-*s"CBUILD_TERM_RESET,
				(int)name_len - 2, TESTS[i].file);
			for(size_t j = 0; j < TPL_COUNT; j++) {
				printf(" %s ", TEST_STATUS_REPORTS[*get_status(i, j)]);
				if(*get_status(i, j) != TEST_SUCCEED &&
					*get_status(i, j) != TEST_SKIPPED) {
					failed = true;
				}
			}
		}
		printf("\n");
	}
	// Bottom platform names
	printf("%*s", (int)name_len, "");
	for(size_t i = 0; i < TPL_COUNT; i++) printf(" | ");
	printf("\n");
	for(ssize_t i = (ssize_t)TPL_COUNT - 1; i >= 0; i--) {
		printf("%*s", (int)name_len, "");
		for(ssize_t j = 0; j < i; j++) printf(" | ");
		printf(" \\-");
		for(ssize_t j = 0; j < TPL_COUNT - i; j++) printf("---");
		printf(" "CBUILD_TERM_FG(CBUILD_TERM_BLUE)"%s"CBUILD_TERM_RESET"\n",
			TPL_NAMES[i]);
	}
	// Help
	static_assert(TEST_STATUS_COUNT == 5, "Enum test_status_t expected to hold 5 statuses.");
	printf("'%s' - Test succeed.\n", TEST_STATUS_REPORTS[TEST_SUCCEED]);
	printf("'%s' - Test assertion failed.\n", TEST_STATUS_REPORTS[TEST_FAILED]);
	printf("'%s' - Test was skipped.\n", TEST_STATUS_REPORTS[TEST_SKIPPED]);
	printf("'%s' - Test leaks memory.\n", TEST_STATUS_REPORTS[TEST_MEMCHECK_FAILED]);
	printf("'%s' - Test failed to compile.\n", TEST_STATUS_REPORTS[TEST_COMP_FAILED]);
	// Exit
	free(statuses);
	return failed;
}
// Hooks
void help(const char* app_name) {
	printf("Usage: %s [OPTIONS] <subcommand> [argument]\n", app_name);
	cbuild_flag_print_help();
	printf("Commands:\n");
	printf("\tdocs     Build documentation. Requires argument\n");
	printf("\t\twikimk     Build wikimk.run\n");
	printf("\t\twiki       Build wiki\n");
	printf("\t\tdoxygen    Build doxygen\n");
	printf("\t\tserve      Host local copy of wiki on localhost:\n");
	printf("\ttest     Run test. Format of argument is <group>:<test>[/platform]. If no platform is specified that test is run for all registered platforms.\n");
	printf("\tclean    Clean all generated files\n");
	printf("\ttags     Generate CTags\n");
	printf("Tests:\n");
	for(size_t i = 0; i < cbuild_arr_len(TESTS); i++) {
		if(TESTS[i].group) {
			printf("\tGroup %s\n", TESTS[i].file);
		} else {
			printf("\t\t- %s\n", TESTS[i].file);
		}
	}
	printf("Test platforms:\n");
	for(size_t i = 0; i < TPL_COUNT; i++) {
		printf("\t%s\n", TPL_NAMES[i]);
	}
}
void version(const char* app_name) {
	CBUILD_UNUSED(app_name);
	printf("CBuild version "CBUILD_VERSION"\n");
	printf("Buildscript version "BUILD_VERSION"\n");
	printf("License: GPL-3.0-or-later\n");
	printf("Author: WolodiaM\n");
}

pid_t MAIN_PID = 0;
void temp_profiler(void) {
	if(getpid() != MAIN_PID) return;
	cbuild_log(CBUILD_LOG_TRACE, "Used %zu/%zu bytes of temp.",
		__cbuild_int_temp_size, CBUILD_TEMP_ARENA_SIZE);
}
void add_selfrebuild_args(cbuild_cmd_t* cmd) {
	cbuild_cmd_append(cmd, "-fopenmp");
}
// Main
int main(int argc, char** argv) {
	cbuild_selfrebuild_hook = add_selfrebuild_args;
	cbuild_selfrebuild(argc, argv);
	MAIN_PID = getpid();
	atexit(temp_profiler);
	cbuild_flag_help = help;
	cbuild_flag_version = version;
	cbuild_flag_parse(argc, argv);
	cbuild_arglist_t* pargs_orig =	cbuild_flag_get_pargs();
	cbuild_arglist_t pargs = *pargs_orig;
	if(pargs.size == 0) {
		cbuild_log(CBUILD_LOG_ERROR, "No subcommand specified!");
		help(cbuild_flag_app_name());
		return 1;
	}
	if(!cbuild_dir_check(BUILD_FOLDER)) {
		if(!cbuild_dir_create(BUILD_FOLDER)) {
			cbuild_log(CBUILD_LOG_ERROR, "Cannot create \""BUILD_FOLDER"\" folder!");
			return 1;
		}
	}
	char* subcommand = cbuild_shift(pargs.data, pargs.size);
	if(strcmp(subcommand, "docs") == 0) {
		if(pargs.size == 0) {
			cbuild_log(CBUILD_LOG_ERROR, "No argument specified!");
			help(cbuild_flag_app_name());
			return 1;
		}
		char* arg = cbuild_shift(pargs.data, pargs.size);
		if(strcmp(arg, "doxygen") == 0) {
			cbuild_cmd_t cmd = {0};
			cbuild_cmd_append_many(&cmd, "doxygen", "doxygen.conf");
			if(!cbuild_cmd_run(&cmd)) return 1;
			if(!cbuild_dir_remove("wiki/out/doxygen")) return 1;
			if(!cbuild_dir_move("wiki/doxygen/html", "wiki/out/doxygen")) return 1;
		} else if(strcmp(arg, "wikimk") == 0) {
			cbuild_cmd_t cmd = {0};
			cbuild_cmd_append_many(&cmd,
				CBUILD_CC, CBUILD_CARGS_WARN,
				CBUILD_CC_OUT, "wikimk.run",
				"wikimk.c");
			if(!cbuild_cmd_run(&cmd)) return 1;
		} else if(strcmp(arg, "wiki") == 0) {
			cbuild_cmd_t cmd = {0};
			cbuild_cmd_append_many(&cmd, "./wikimk.run", "build");
			if(!cbuild_cmd_run(&cmd)) return 1;
		} else if(strcmp(arg, "serve") == 0) {
			cbuild_cmd_t cmd = {0};
			cbuild_cmd_append_many(&cmd, "./wikimk.run", "serve");
			if(!cbuild_cmd_run(&cmd)) return 1;
		} else {
			cbuild_log(CBUILD_LOG_ERROR, "Invalid argument specified: \"%s\"!", arg);
			help(cbuild_flag_app_name());
			return 1;
		}
	} else if(strcmp(subcommand, "test") == 0) {
		if(pargs.size == 0) {
			if(test()) return 1;
		} else {
			bool failed = false;
			cbuild_da_foreach (&pargs, test_group) {
				// Parse spec
				char* test_name = strchr(*test_group, ':');
				if(test_name != NULL) {
					*test_name = '\0';
					test_name++;
				} else {
					cbuild_log_error("Test group is required!");
					help(cbuild_flag_app_name());
				}
				char*	tpl_name = strchr(test_name, '/');
				if(tpl_name != NULL) {
					*tpl_name = '\0';
					tpl_name++;
				}
				// Check platform
				bool platform_found = false;
				uint32_t platform_mask = 0;
				if(tpl_name != NULL) {
					for(int j = 0; j < TPL_COUNT; j++) {
						if(strcmp(tpl_name, TPL_NAMES[j]) == 0) {
							platform_mask = TPL_MASK(j);
							platform_found = true;
							break;
						}
					}
					if(!platform_found) {
						cbuild_log_error("Invalid test platform specified: \"%s\"!",
							tpl_name);
						help(cbuild_flag_app_name());
						return 1;
					}
				}
				// Setup test group and run test
				bool test_found = false;
				glob_t glob_state = {0};
				int cres = glob(test_name, GLOB_COMPILE, &glob_state, NULL, 0);
				if(cres != 0 && cres < GLOB_REGERROR_MAX) return 1;
				for(size_t i = 0; i < cbuild_arr_len(TESTS); i++) {
					test_case_t test = TESTS[i];
					if(test.group) {
						if(TPL_RUN_REGISTERED_GROUP != NULL) break;
						if(strcmp(test.file, *test_group) == 0) {
							TPL_RUN_REGISTERED_GROUP = *test_group;
						}
					} else {
						if(TPL_RUN_REGISTERED_GROUP != NULL) {
							int mres = glob(test_name, 0, &glob_state, &test.file, 1);
							if (mres == GLOB_NOERROR && glob_state.size > 0) {
								if(platform_found) test.platforms = platform_mask;
								test_found = true;
								if(test_case(test)) failed = true;
							}
						}
					}
				}
				globfree(&glob_state);
				if(!test_found) {
					cbuild_log_error("Invalid test specified: \"%s:%s\"!",
						*test_group, test_name);
					help(cbuild_flag_app_name());
					return 1;
				}
				if(failed) return 1;
				// Reset group
				TPL_RUN_REGISTERED_GROUP = NULL;
			}
		}
	} else if(strcmp(subcommand, "clean") == 0) {
		cbuild_dir_remove(BUILD_FOLDER);
		cbuild_dir_remove("wiki/doxygen/html");
		cbuild_dir_remove("wiki/out");
		cbuild_file_remove("wiki/wikimk/template/nav.html");
		cbuild_file_remove("TAGS");
		cbuild_file_remove("wikimk.run");
	} else if(strcmp(subcommand, "tags") == 0) {
		cbuild_cmd_t cmd = {0};
		cbuild_cmd_append_many(&cmd, "ctags",
			"-e", "--languages=C", "--kinds-C=+px",
			"cbuild.h");
		if(!cbuild_cmd_run(&cmd)) {
			return 1;
		}
	} else {
		cbuild_log(CBUILD_LOG_ERROR, "Invalid subcommand specified: \"%s\"!", subcommand);
		help(cbuild_flag_app_name());
		return 1;
	}
	return 0;
}
