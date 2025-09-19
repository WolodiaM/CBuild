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
// Includes
#define CBUILD_LOG_MIN_LEVEL CBUILD_LOG_TRACE
#include "cbuild.h"
#include "tests/framework.h"
// Globals
#define BUILD_VERSION "v1.0"
#define BUILD_FOLDER "build"
#define TEST_FOLDER "tests"
// Test system globals
typedef enum test_status_t {
	TEST_SKIPPED = -1,
	TEST_SUCCEED = 0,
	TEST_FAILED = 1,
	TEST_COMP_FAILED = 2,
} test_status_t;
#define TPL_ALL 0xFFFFFFFF
typedef struct test_case_t {
	union {
		struct {
			uint32_t x86_64_linux_glibc_gcc   : 1;
			uint32_t x86_64_linux_glibc_clang : 1;
			uint32_t x86_64_linux_musl_gcc    : 1;
			uint32_t x86_64_posix_gcc         : 1; // Run on Linux under glibc
			uint32_t x86_64_posix_clang       : 1; // Run on Linux under glibc
			uint32_t                          : 27;
		} platform;
		uint32_t platforms;
	};
	uint32_t flags;
	const char* file;
	cbuild_arglist_t argv;
} test_case_t;
#define TEST_COUNT 1
test_case_t TESTS[TEST_COUNT] = {
	{
		.file = "cmd_to_sb",
		.platforms = TPL_ALL,
	}
};
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
test_status_t test_x86_64_linux_glibc_gcc(test_case_t test) {
	test_log_start("Running test case \"%s\"", test.file);
	cbuild_log_info("Platform: Linux/glibc, Arch: x86_64, Compiler: gcc");
	cbuild_log_trace("Building test \"%s\"...", test.file);
	cbuild_cmd_t cmd = {0};
	const char*	fname =	cbuild_temp_sprintf(TEST_FOLDER"/%s.c", test.file);
	const char*	oname =
		cbuild_temp_sprintf(BUILD_FOLDER"/test_x86_64_linux_glibc_gcc_%s.run", test.file);
	cbuild_cmd_append(&cmd, "gcc");
	cbuild_cmd_append_many(&cmd, CBUILD_CARGS_WARN, CBUILD_CARGS_WERROR);
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
test_status_t test_x86_64_linux_glibc_clang(test_case_t test) {
	test_log_start("Running test case \"%s\"", test.file);
	cbuild_log_info("Platform: Linux/glibc, Arch: x86_64, Compiler: clang");
	cbuild_log_trace("Building test \"%s\"...", test.file);
	cbuild_cmd_t cmd = {0};
	const char*	fname =	cbuild_temp_sprintf(TEST_FOLDER"/%s.c", test.file);
	const char*	oname =
		cbuild_temp_sprintf(BUILD_FOLDER"/test_x86_64_linux_glibc_clang_%s.run", test.file);
	cbuild_cmd_append(&cmd, "clang");
	cbuild_cmd_append_many(&cmd, CBUILD_CARGS_WARN, CBUILD_CARGS_WERROR);
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
test_status_t test_x86_64_linux_musl_gcc(test_case_t test) {
	test_log_start("Running test case \"%s\"", test.file);
	cbuild_log_info("Platform: Linux/musl, Arch: x86_64, Compiler: gcc");
	cbuild_log_trace("Building test \"%s\"...", test.file);
	cbuild_cmd_t cmd = {0};
	const char*	fname =	cbuild_temp_sprintf(TEST_FOLDER"/%s.c", test.file);
	const char*	oname =
		cbuild_temp_sprintf(BUILD_FOLDER"/test_x86_64_linux_musl_gcc_%s.run", test.file);
	cbuild_cmd_append(&cmd, "gcc");
	cbuild_cmd_append_many(&cmd, CBUILD_CARGS_WARN, CBUILD_CARGS_WERROR);
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
// Single-case runner
bool test_case(test_case_t test) {
	bool failed = false;
	if((test.platform.x86_64_linux_glibc_gcc)) {
		if(test_x86_64_linux_glibc_gcc(test) != TEST_SUCCEED) failed = true;
	}
	if((test.platform.x86_64_linux_glibc_clang)) {
		if(test_x86_64_linux_glibc_clang(test) != TEST_SUCCEED) failed = true;
	}
	if((test.platform.x86_64_linux_musl_gcc)) {
		if(test_x86_64_linux_musl_gcc(test) != TEST_SUCCEED) failed = true;
	}
	return !failed;
}
// Full runner
typedef struct test_da_status_t {
	test_status_t* data;
	size_t size;
	size_t capacity;
} test_da_status_t;
bool test(void) {
	test_da_status_t x86_64_linux_glibc_gcc = {0};
	// Silence output
	cbuild_fd_t dev_null = cbuild_fd_open_write("/dev/null");
	cbuild_fd_t fdstdout = dup(STDOUT_FILENO);
	cbuild_fd_t fdstderr = dup(STDERR_FILENO);
	fflush(stdout);
	fflush(stderr);
	dup2(dev_null, STDOUT_FILENO);
	dup2(dev_null, STDERR_FILENO);
	// Run tests
	for(size_t i = 0; i < TEST_COUNT; i++) {
		cbuild_da_append(&x86_64_linux_glibc_gcc,
			test_x86_64_linux_glibc_gcc(TESTS[i]));
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
	for(size_t i = 0; i < TEST_COUNT; i++) {
		printf("%s -> %d\n", TESTS[i].file, x86_64_linux_glibc_gcc.data[i]);
	}
	return true;
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
	printf("\ttest     Run test. If no argument is provided run all available tests.\n");
	for(size_t i = 0; i < TEST_COUNT; i++) {
		printf("\t\t%s\n", TESTS[i].file);
	}
	printf("\tclean    Clean all generated files\n");
	printf("\ttags     Generate CTags\n");
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
// Main
int main(int argc, char** argv) {
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
	char* cmd = cbuild_shift(pargs.data, pargs.size);
	if(strcmp(cmd, "docs") == 0) {
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
	} else if(strcmp(cmd, "test") == 0) {
		if(pargs.size == 0) {
			if(!test()) return 1;
		} else {
			bool failed = false;
			cbuild_da_foreach(&pargs, test_name) {
				for(size_t i = 0; i < TEST_COUNT; i++) {
					if(!test_case(TESTS[i])) failed = true;
				}
			}
			if(failed) return 1;
		}
	} else if(strcmp(cmd, "clean") == 0) {
		cbuild_dir_remove(BUILD_FOLDER);
		cbuild_dir_remove("wiki/doxygen/html");
		cbuild_dir_remove("wiki/out");
		cbuild_file_remove("wiki/wikimk/template/nav.html");
		cbuild_file_remove("TAGS");
		cbuild_file_remove("wikimk.run");
	} else if(strcmp(cmd, "tags") == 0) {
		cbuild_cmd_t cmd = {0};
		cbuild_cmd_append_many(&cmd, "ctags",
			"-e", "--languages=C", "--kinds-C=+px",
			"cbuild.h");
		if(!cbuild_cmd_run(&cmd)) {
			return 1;
		}
	} else {
		cbuild_log(CBUILD_LOG_ERROR, "Invalid subcommand specified: \"%s\"!", cmd);
		help(cbuild_flag_app_name());
		return 1;
	}
	return 0;
}
