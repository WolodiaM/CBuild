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
#define CBUILD_IMPLEMENTATION
#include "cbuild.h"
// Globals
#define BUILD_VERSION "v1.0"
#define BUILD_FOLDER "build"
#define TEST_FOLDER "tests"
// Test system globals
#define TEST_FILES_SIZE 1
const char* TEST_FILES[] = {
	"Command"
};
// Test runner
bool test_case(const char* name) {
	return true;
}
bool test(void) {
	bool failed = false;
	for(size_t i = 0; i < TEST_FILES_SIZE; i++) {
		if(!test_case(TEST_FILES[i])) failed = true;
	}
	return !failed;
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
	for(size_t i = 0; i < TEST_FILES_SIZE; i++) {
		printf("\t\t%s\n", TEST_FILES[i]);
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
				if(!test_case(*test_name)) failed = true;
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
