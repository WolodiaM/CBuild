/**
 * @file FS.c
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Tests for cbuild's filesystem APIs
 *
 *
 * @date 2024-12-07
 * @copyright (C) 2024 WolodiaM
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
// Project includes
#include "../cbuild.h"
#include "framework.h"
#define CBUILD_IMPLEMENTATION
#include "../cbuild.h"
// Code
TEST_MAIN({
	TEST_CASE(
	{
		// Create test data
		cbuild_cmd_t file_writer = {0};
		cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
		cbuild_fd_t pattern_fd =
		  cbuild_fd_open_write("build/FS.c.read_test");
		cbuild_cmd_run(&file_writer, .fdstdout = &pattern_fd);
		cbuild_fd_close(pattern_fd);
		// Do testing
		cbuild_sb_t sb  = {0};
		bool        ret = cbuild_file_read("build/FS.c.read_test", &sb);
		TEST_ASSERT_EQ(ret, true, "Function returened error%s", "");
		cbuild_sb_append_null(&sb);
		TEST_ASSERT_STREQ(
		  sb.data, "ABCD",
		  "Wrong value read from a file: \"%s\", expected: \"%s\"",
		  sb.data, "ABCD");
		cbuild_cmd_clear(&file_writer);
		cbuild_sb_clear(&sb);
	},
	"File reading");
	TEST_CASE(
	{
		cbuild_sb_t sb = {0};
		cbuild_sb_append_cstr(&sb, "ABCD");
		cbuild_file_write("build/FS.c.write_test", &sb);
		cbuild_sb_t check = {0};
		bool        ret = cbuild_file_read("build/FS.c.write_test", &check);
		TEST_ASSERT_EQ(ret, true, "Function returened error%s", "");
		cbuild_sb_append_null(&sb);
		cbuild_sb_append_null(&check);
		TEST_ASSERT_STREQ(
		  check.data, sb.data,
		  "Wrong value read from a file: \"%s\", expected: \"%s\"",
		  check.data, sb.data);
		cbuild_sb_clear(&sb);
		cbuild_sb_clear(&check);
	},
	"File writing (and then reread)");
	TEST_CASE(
	{
		// Create test data
		cbuild_cmd_t file_writer = {0};
		cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
		cbuild_fd_t pattern_fd =
		  cbuild_fd_open_write("build/FS.c.copy_test.src");
		cbuild_cmd_run(&file_writer, .fdstdout = &pattern_fd);
		cbuild_fd_close(pattern_fd);
		// Do testing
		cbuild_sb_t f1  = {0};
		cbuild_sb_t f2  = {0};
		bool        ret = cbuild_file_copy("build/FS.c.copy_test.src",
		    "build/FS.c.copy_test.dst");
		TEST_ASSERT_EQ(ret, true, "Function returened error%s", "");
		cbuild_file_read("build/FS.c.copy_test.src", &f1);
		cbuild_file_read("build/FS.c.copy_test.dst", &f2);
		cbuild_sb_append_null(&f1);
		cbuild_sb_append_null(&f2);
		TEST_ASSERT_STREQ(
		  f1.data, f2.data,
		  "Wrong value read from a file copy: \"%s\", expected: \"%s\"",
		  f1.data, f2.data);
		cbuild_cmd_clear(&file_writer);
		cbuild_sb_clear(&f1);
		cbuild_sb_clear(&f2);
	},
	"File copy");
	TEST_CASE(
	{
		// Create test data
		cbuild_cmd_t file_writer = {0};
		cbuild_cmd_append_many(&file_writer, "dd", "bs=1024", "count=64",
		  "status=none", "if=/dev/urandom",
		  "of=build/FS.c.copy_big_test.src");
		cbuild_cmd_run(&file_writer);
		// Do testing
		cbuild_sb_t f1  = {0};
		cbuild_sb_t f2  = {0};
		bool        ret = cbuild_file_copy("build/FS.c.copy_big_test.src",
		    "build/FS.c.copy_big_test.dst");
		TEST_ASSERT_EQ(ret, true, "Function returened error%s", "");
		cbuild_file_read("build/FS.c.copy_big_test.src", &f1);
		cbuild_file_read("build/FS.c.copy_big_test.dst", &f2);
		TEST_ASSERT_MEMEQ(
		  f1.data, f2.data, 64 * 1024,
		  "Wrong value read from a file copy: \"" CBuildSBFmt
		  "\", expected: \"" CBuildSBFmt "\"",
		  CBuildSBArg(f1), CBuildSBArg(f2));
		cbuild_cmd_clear(&file_writer);
		cbuild_sb_clear(&f1);
		cbuild_sb_clear(&f2);
	},
	"Big file copy (Much larger than tmp buffer size)");
	TEST_CASE(
	{
		// Create test data
		cbuild_cmd_t file_writer = {0};
		cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
		cbuild_fd_t pattern_fd =
		  cbuild_fd_open_write("build/FS.c.file_check_test.true");
		cbuild_cmd_run(&file_writer, .fdstdout = &pattern_fd);
		cbuild_fd_close(pattern_fd);
		// Do testing
		bool ch1 = cbuild_file_check("build/FS.c.file_check_test.true");
		TEST_ASSERT_EQ(
		  ch1, true,
		  "Wrong file detection, file exist but check returned flase%s",
		  "");
		bool ch2 = cbuild_file_check("build/FS.c.file_check_test.false");
		TEST_ASSERT_EQ(ch2, false, "%s",
		  "Wrong file detection, file does not exist but check returned true");
		cbuild_cmd_clear(&file_writer);
	},
	"File checking");
	TEST_CASE(
	{
		// Create test data
		cbuild_cmd_t file_writer = {0};
		cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
		cbuild_fd_t pattern_fd =
		  cbuild_fd_open_write("build/FS.c.move_test.src");
		cbuild_cmd_run(&file_writer, .fdstdout = &pattern_fd);
		cbuild_fd_close(pattern_fd);
		// Do testing
		bool ret = cbuild_file_move("build/FS.c.move_test.src",
		    "build/FS.c.move_test.dst");
		TEST_ASSERT_EQ(ret, true, "Function returened error%s", "");
		bool ch1 = cbuild_file_check("build/FS.c.move_test.dst");
		TEST_ASSERT_EQ(ch1, true, "Move distination does not exist%s", "");
		bool ch2 = cbuild_file_check("build/FS.c.move_test.src");
		TEST_ASSERT_EQ(ch2, false, "Move source was not removed%s", "");
		cbuild_cmd_clear(&file_writer);
	},
	"File move");
	TEST_CASE(
	{
		// Create test data
		cbuild_cmd_t file_writer = {0};
		cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
		cbuild_fd_t pattern_fd =
		  cbuild_fd_open_write("build/FS.c.rename_test.src");
		cbuild_cmd_run(&file_writer, .fdstdout = &pattern_fd);
		cbuild_fd_close(pattern_fd);
		// Do testing
		bool ret = cbuild_file_rename("build/FS.c.rename_test.src",
		    "build/FS.c.rename_test.dst");
		TEST_ASSERT_EQ(ret, true, "Function returened error%s", "");
		bool ch1 = cbuild_file_check("build/FS.c.rename_test.dst");
		TEST_ASSERT_EQ(ch1, true, "Renamed file does not exist%s", "");
		bool ch2 = cbuild_file_check("build/FS.c.rename_test.src");
		TEST_ASSERT_EQ(ch2, false, "Source file exists%s", "");
		cbuild_cmd_clear(&file_writer);
	},
	"File rename");
	TEST_CASE(
	{
		// Create test data
		cbuild_cmd_t file_writer = {0};
		cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
		cbuild_fd_t pattern_fd =
		  cbuild_fd_open_write("build/FS.c.remove_test");
		cbuild_cmd_run(&file_writer, .fdstdout = &pattern_fd);
		cbuild_fd_close(pattern_fd);
		// Do testing
		bool ret = cbuild_file_remove("build/FS.c.remove_test");
		TEST_ASSERT_EQ(ret, true, "Function returened error%s", "");
		bool ch = cbuild_file_check("build/FS.c.remove_test");
		TEST_ASSERT_EQ(ch, false, "Removed file exist%s", "");
		cbuild_cmd_clear(&file_writer);
	},
	"File remove");
	TEST_CASE(
	{
		// Create test data
		cbuild_cmd_t dir_creator = {0};
		cbuild_cmd_append_many(&dir_creator, "mkdir",
		  "build/FS.c.dirls_test/");
		cbuild_cmd_run(&dir_creator);
		cbuild_cmd_t file_writer = {0};
		cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
		cbuild_fd_t pattern_fd =
		  cbuild_fd_open_write("build/FS.c.dirls_test/a");
		cbuild_cmd_run(&file_writer, .fdstdout = &pattern_fd);
		cbuild_fd_close(pattern_fd);
		file_writer.size = 0;
		cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
		pattern_fd = cbuild_fd_open_write("build/FS.c.dirls_test/b");
		cbuild_cmd_run(&file_writer, .fdstdout = &pattern_fd);
		cbuild_fd_close(pattern_fd);
		// Do testing
		cbuild_pathlist_t dirls = {0};
		bool ret = cbuild_dir_list("build/FS.c.dirls_test/", &dirls);
		TEST_ASSERT_EQ(ret, true, "Function returened error%s", "");
		TEST_ASSERT_EQ(dirls.size, 2, "Expected 2 files but got %zu",
		  dirls.size);
		TEST_ASSERT_STREQ(dirls.data[0], "a", "Wrong directory read at first index"
		  TEST_EXPECT_MSG(s), "a", dirls.data[0]);
		TEST_ASSERT_STREQ(dirls.data[1], "b", "Wrong directory read at second index"
		  TEST_EXPECT_MSG(s), "b", dirls.data[1]);
		cbuild_pathlist_clear(&dirls);
		cbuild_cmd_clear(&dir_creator);
		cbuild_cmd_clear(&file_writer);
	},
	"Directory list");
	TEST_CASE(
	{
		// Create test data
		cbuild_cmd_t dir_creator = {0};
		cbuild_cmd_append_many(&dir_creator, "mkdir",
		  "build/FS.c.dir_check_test.true/");
		cbuild_cmd_run(&dir_creator);
		// Do testing
		bool ch1 = cbuild_dir_check("build/FS.c.dir_check_test.true/");
		TEST_ASSERT_EQ(
		  ch1, true,
		  "Wrong file detection, file exist but check returned flase%s",
		  "");
		bool ch2 = cbuild_dir_check("build/FS.c.dir_check_test.false/");
		TEST_ASSERT_EQ(ch2, false, "%s",
		  "Wrong file detection, file does not exist but check returned true");
		cbuild_cmd_clear(&dir_creator);
	},
	"Directory checking");
	TEST_CASE(
	{
		printf("\tBase exists\n");
		bool ret1 = cbuild_dir_create("build/FS.c.dir_create_test/");
		TEST_ASSERT_EQ(ret1, true, "Function returened error%s", "");
		bool ch1 = cbuild_dir_check("build/FS.c.dir_create_test/");
		TEST_ASSERT_EQ(ch1, true, "Created directory does not exist%s", "");
		printf("\tBase don't exist\n");
		bool ret2 = cbuild_dir_create("build/FS.c.dir_create_no_base_test/dir/");
		TEST_ASSERT_EQ(ret2, true, "Function returened error%s", "");
		bool ch2 = cbuild_dir_check("build/FS.c.dir_create_no_base_test/dir");
		TEST_ASSERT_EQ(ch2, true, "Created directory does not exist%s", "");
	},
	"Directory creation");
	TEST_CASE(
	{
		// Create test data
		cbuild_dir_create("build/FS.c.dir_copy_test.src/");
		cbuild_cmd_t file_writer = {0};
		cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
		cbuild_fd_t pattern_fd =
		  cbuild_fd_open_write("build/FS.c.dir_copy_test.src/a");
		cbuild_cmd_run(&file_writer, .fdstdout = &pattern_fd);
		cbuild_fd_close(pattern_fd);
		file_writer.size = 0;
		cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
		pattern_fd = cbuild_fd_open_write("build/FS.c.dir_copy_test.src/b");
		cbuild_cmd_run(&file_writer, .fdstdout = &pattern_fd);
		cbuild_fd_close(pattern_fd);
		cbuild_dir_create("build/FS.c.dir_copy_test.src/c/");
		file_writer.size = 0;
		cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
		pattern_fd =
		  cbuild_fd_open_write("build/FS.c.dir_copy_test.src/c/d");
		cbuild_cmd_run(&file_writer, .fdstdout = &pattern_fd);
		cbuild_fd_close(pattern_fd);
		// Do testing
		bool ret = cbuild_dir_copy("build/FS.c.dir_copy_test.src/",
		    "build/FS.c.dir_copy_test.dst/");
		TEST_ASSERT_EQ(ret, true, "Function returened error%s", "");
		TEST_ASSERT_EQ(cbuild_file_check("build/FS.c.dir_copy_test.dst/a"),
		  true, "File \"a\" does not exist after copy%s", "");
		TEST_ASSERT_EQ(cbuild_file_check("build/FS.c.dir_copy_test.dst/b"),
		  true, "File \"b\" does not exist after copy%s", "");
		TEST_ASSERT_EQ(cbuild_dir_check("build/FS.c.dir_copy_test.dst/c"),
		  true, "Directory\"c\" does not exist after copy%s",
		  "");
		TEST_ASSERT_EQ(
		  cbuild_file_check("build/FS.c.dir_copy_test.dst/c/d"), true,
		  "File \"d\" does not exist after copy%s", "");
		cbuild_cmd_clear(&file_writer);
	},
	"Directory copy");
	TEST_CASE(
	{
		// Create test data
		cbuild_dir_create("build/FS.c.dir_remove_test/");
		cbuild_cmd_t file_writer = {0};
		cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
		cbuild_fd_t pattern_fd =
		  cbuild_fd_open_write("build/FS.c.dir_remove_test/a");
		cbuild_cmd_run(&file_writer, .fdstdout = &pattern_fd);
		cbuild_fd_close(pattern_fd);
		file_writer.size = 0;
		cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
		pattern_fd = cbuild_fd_open_write("build/FS.c.dir_remove_test/b");
		cbuild_cmd_run(&file_writer, .fdstdout = &pattern_fd);
		cbuild_fd_close(pattern_fd);
		cbuild_dir_create("build/FS.c.dir_remove_test/c/");
		file_writer.size = 0;
		cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
		pattern_fd = cbuild_fd_open_write("build/FS.c.dir_remove_test/c/d");
		cbuild_cmd_run(&file_writer, .fdstdout = &pattern_fd);
		cbuild_fd_close(pattern_fd);
		// Do testing
		bool ret = cbuild_dir_remove("build/FS.c.dir_remove_test/");
		TEST_ASSERT_EQ(ret, true, "Function returened error%s", "");
		TEST_ASSERT_EQ(cbuild_file_check("build/FS.c.dir_remove_test/a"),
		  false, "File \"a\" does not exist after copy%s", "");
		TEST_ASSERT_EQ(cbuild_file_check("build/FS.c.dir_remove_test/b"),
		  false, "File \"b\" does not exist after copy%s", "");
		TEST_ASSERT_EQ(cbuild_dir_check("build/FS.c.dir_remove_test/c"),
		  false, "Directory\"c\" does not exist after copy%s",
		  "");
		TEST_ASSERT_EQ(cbuild_file_check("build/FS.c.dir_remove_test/c/d"),
		  false, "File \"d\" does not exist after copy%s", "");
		cbuild_cmd_clear(&file_writer);
	},
	"Directory remove");
	TEST_CASE(
	{
		// Create test data
		cbuild_cmd_t dir_creator = {0};
		cbuild_cmd_append_many(&dir_creator, "mkdir",
		  "build/FS.c.path_type_check.dir/");
		cbuild_cmd_run(&dir_creator);
		cbuild_cmd_t file_writer = {0};
		cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
		cbuild_fd_t pattern_fd =
		  cbuild_fd_open_write("build/FS.c.path_type_check.file");
		cbuild_cmd_run(&file_writer, .fdstdout = &pattern_fd);
		cbuild_fd_close(pattern_fd);
		// Do testing
		cbuild_filetype_t f1 =
		  cbuild_path_filetype("build/FS.c.path_type_check.dir/");
		TEST_ASSERT_EQ(f1, CBUILD_FTYPE_DIRECTORY,
		  "Directory not detected properly%s", "");
		cbuild_filetype_t f2 =
		  cbuild_path_filetype("build/FS.c.path_type_check.file");
		TEST_ASSERT_EQ(f2, CBUILD_FTYPE_REGULAR,
		  "Regular file not detected properly%s", "");
		cbuild_filetype_t f3 =
		  cbuild_path_filetype("build/FS.c.path_type_check.none/");
		TEST_ASSERT_EQ(f3, CBUILD_FTYPE_OTHER,
		  "Missing file should reporst as \"OTHER\"%s", "");
		cbuild_cmd_clear(&dir_creator);
		cbuild_cmd_clear(&file_writer);
	},
	"Path typechecking");
	TEST_CASE(
	{
		const char* p1 = "some/dir/file.txt";
		const char* e1 = cbuild_path_ext(p1);
		TEST_ASSERT_STREQ(e1, "txt",
		  "Expected extension \"txt\" but got \"%s\"", e1);
		const char* p2 = "some/dir/d";
		const char* e2 = cbuild_path_ext(p2);
		TEST_ASSERT_STREQ(e2, "", "Expected extension \"\" but got \"%s\"", e2);
		const char* p3 = "f.txt";
		const char* e3 = cbuild_path_ext(p3);
		TEST_ASSERT_STREQ(e3, "txt",
		  "Expected extension \"txt\" but got \"%s\"", e3);
		const char* p4 = "f";
		const char* e4 = cbuild_path_ext(p4);
		TEST_ASSERT_STREQ(e4, "", "Expected extension \"\" but got \"%s\"", e4);
		free((void*)e1);
		free((void*)e2);
		free((void*)e3);
		free((void*)e4);
	},
	"Extracting extension from filepath");
	TEST_CASE(
	{
		const char* p1 = "some/dir/file.txt";
		const char* n1 = cbuild_path_name(p1);
		TEST_ASSERT_STREQ(n1, "file.txt",
		  "Expected filename \"file.txt\" but got \"%s\"",  n1);
		const char* p2 = "some/dir/another_dir/";
		const char* n2 = cbuild_path_name(p2);
		TEST_ASSERT_STREQ(
		  n2, "another_dir/",
		  "Expected dirname \"another_dir/\" but got \"%s\"", n2);
		const char* p3 = "a.c";
		const char* n3 = cbuild_path_name(p3);
		TEST_ASSERT_STREQ(n3, "a.c",
		  "Expected filename \"a.c\" but got \"%s\"", n3);
		free((void*)n1);
		free((void*)n2);
		free((void*)n3);
	},
	"Extracting filename from filepath");
	TEST_CASE(
	{
		const char* p1 = "some/dir/file.txt";
		const char* n1 = cbuild_path_base(p1);
		TEST_ASSERT_STREQ(n1, "some/dir/",
		  "Expected basename \"some/dir/\" but got \"%s\"",
		  n1);
		const char* p2 = "some/dir/another_dir/";
		const char* n2 = cbuild_path_base(p2);
		TEST_ASSERT_STREQ(n2, "some/dir/",
		  "Expected basename \"some/dir/\" but got \"%s\"",
		  n2);
		const char* p3 = "a.c";
		const char* n3 = cbuild_path_base(p3);
		TEST_ASSERT_STREQ(n3, "", "Expected basename \"\" but got \"%s\"",
		  n3);
		free((void*)n1);
		free((void*)n2);
		free((void*)n3);
	},
	"Extracting basename from filepath");
	TEST_CASE({ // Test cases generated by AI, it is better at generating random data
		const char* i = "/usr/bin/";
		char* o = cbuild_path_normalize(i);
		TEST_ASSERT_STREQ("/usr/bin", o, "Wrong path after normalization"
		  TEST_EXPECT_MSG(s), "/usr/bin", o);
		free(o);
		i = "/usr//bin///env";
		o = cbuild_path_normalize(i);
		TEST_ASSERT_STREQ("/usr/bin/env", o, "Wrong path after normalization"
		  TEST_EXPECT_MSG(s), "/usr/bin/env", o);
		free(o);
		i = "./src/././main.c";
		o = cbuild_path_normalize(i);
		TEST_ASSERT_STREQ("src/main.c", o, "Wrong path after normalization"
		  TEST_EXPECT_MSG(s), "src/main.c", o);
		free(o);
		i = "src/../include/header.h";
		o = cbuild_path_normalize(i);
		TEST_ASSERT_STREQ("include/header.h", o, "Wrong path after normalization"
		  TEST_EXPECT_MSG(s), "include/header.h", o);
		free(o);
		i = "../../etc/config";
		o = cbuild_path_normalize(i);
		TEST_ASSERT_STREQ("../../etc/config", o, "Wrong path after normalization"
		  TEST_EXPECT_MSG(s), "../../etc/config", o);
		free(o);
		i = "/home/user/.././docs//file.txt";
		o = cbuild_path_normalize(i);
		TEST_ASSERT_STREQ("/home/docs/file.txt", o, "Wrong path after normalization"
		  TEST_EXPECT_MSG(s), "/home/docs/file.txt", o);
		free(o);
		i = "///";
		o = cbuild_path_normalize(i);
		TEST_ASSERT_STREQ("/", o, "Wrong path after normalization"
		  TEST_EXPECT_MSG(s), "/", o);
		free(o);
		i = "./././";
		o = cbuild_path_normalize(i);
		TEST_ASSERT_STREQ(".", o, "Wrong path after normalization"
		  TEST_EXPECT_MSG(s), ".", o);
		free(o);
		i = "../a/./b/../../c/";
		o = cbuild_path_normalize(i);
		TEST_ASSERT_STREQ("../c", o, "Wrong path after normalization"
		  TEST_EXPECT_MSG(s), "../c", o);
		free(o);
		i = "";
		o = cbuild_path_normalize(i);
		TEST_ASSERT_STREQ(".", o, "Wrong path after normalization"
		  TEST_EXPECT_MSG(s), ".", o);
		free(o);
		i = "..";
		o = cbuild_path_normalize(i);
		TEST_ASSERT_STREQ("..", o, "Wrong path after normalization"
		  TEST_EXPECT_MSG(s), "..", o);
		free(o);
	}, "Path normalization");
},
"Filesystem library")
