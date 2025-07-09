/**
 * @file FS.c
 * @author WolodiaM (w_melnyk@outlook.com)
 * @brief Tests for cbuild's filesystem APIs
 *
 *
 * @date 2024-12-07
 * @copyright (C) 2024 WolodiaM
 * @license MIT
 *
 * Copyright (C) 2024 WolodiaM
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
// Project includes
#include "../src/Command.h"
#include "../src/FS.h"
#include "../src/StringBuilder.h"
#include "framework.h"
// Code
TEST_MAIN(
		{
			TEST_CASE(
					{
						// Create test data
						cbuild_cmd_t file_writer = cbuild_cmd;
						cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
						cbuild_fd_t pattern_fd =
								cbuild_fd_open_write("build/FS.c.read_test");
						cbuild_cmd_sync_redirect(
								file_writer,
								(cbuild_cmd_fd_t){ .fdstdin  = CBUILD_INVALID_FD,
		                               .fdstdout = pattern_fd,
		                               .fdstderr = CBUILD_INVALID_FD });
						cbuild_fd_close(pattern_fd);
						// Do testing
						cbuild_sb_t sb  = cbuild_sb;
						bool        ret = cbuild_file_read("build/FS.c.read_test", &sb);
						TEST_ASSERT_EQ(ret, true, "Function returened error%s", "");
						cbuild_sb_append_null(&sb);
						TEST_ASSERT_STREQ(
								sb.data, "ABCD",
								"Wrong value read from a file: \"%s\", expected: \"%s\"",
								sb.data, "ABCD");
					},
					"File reading");
			TEST_CASE(
					{
						cbuild_sb_t sb = cbuild_sb;
						cbuild_sb_append_cstr(&sb, "ABCD");
						cbuild_file_write("build/FS.c.write_test", &sb);
						cbuild_sb_t check = cbuild_sb;
						bool        ret = cbuild_file_read("build/FS.c.write_test", &check);
						TEST_ASSERT_EQ(ret, true, "Function returened error%s", "");
						cbuild_sb_append_null(&sb);
						cbuild_sb_append_null(&check);
						TEST_ASSERT_STREQ(
								check.data, sb.data,
								"Wrong value read from a file: \"%s\", expected: \"%s\"",
								check.data, sb.data);
					},
					"File writing (and then reread)");
			TEST_CASE(
					{
						// Create test data
						cbuild_cmd_t file_writer = cbuild_cmd;
						cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
						cbuild_fd_t pattern_fd =
								cbuild_fd_open_write("build/FS.c.copy_test.src");
						cbuild_cmd_sync_redirect(
								file_writer,
								(cbuild_cmd_fd_t){ .fdstdin  = CBUILD_INVALID_FD,
		                               .fdstdout = pattern_fd,
		                               .fdstderr = CBUILD_INVALID_FD });
						cbuild_fd_close(pattern_fd);
						// Do testing
						cbuild_sb_t f1  = cbuild_sb;
						cbuild_sb_t f2  = cbuild_sb;
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
					},
					"File copy");
			TEST_CASE(
					{
						// Create test data
						cbuild_cmd_t file_writer = cbuild_cmd;
						cbuild_cmd_append_many(&file_writer, "dd", "bs=1024", "count=64",
		                               "status=none", "if=/dev/urandom",
		                               "of=build/FS.c.copy_big_test.src");
						cbuild_cmd_sync(file_writer);
						// Do testing
						cbuild_sb_t f1  = cbuild_sb;
						cbuild_sb_t f2  = cbuild_sb;
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
					},
					"Big file copy (Much larger than tmp buffer size)");
			TEST_CASE(
					{
						// Create test data
						cbuild_cmd_t file_writer = cbuild_cmd;
						cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
						cbuild_fd_t pattern_fd =
								cbuild_fd_open_write("build/FS.c.file_check_test.true");
						cbuild_cmd_sync_redirect(
								file_writer,
								(cbuild_cmd_fd_t){ .fdstdin  = CBUILD_INVALID_FD,
		                               .fdstdout = pattern_fd,
		                               .fdstderr = CBUILD_INVALID_FD });
						cbuild_fd_close(pattern_fd);
						// Do testing
						bool ch1 = cbuild_file_check("build/FS.c.file_check_test.true");
						TEST_ASSERT_EQ(
								ch1, true,
								"Wrong file detection, file exist but check returned flase%s",
								"");
						bool ch2 = cbuild_file_check("build/FS.c.file_check_test.false");
						TEST_ASSERT_EQ(ch2, false,
		                       "Wrong file detection, file does not exist but "
		                       "check returned true%s",
		                       "");
					},
					"File checking");
			TEST_CASE(
					{
						// Create test data
						cbuild_cmd_t file_writer = cbuild_cmd;
						cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
						cbuild_fd_t pattern_fd =
								cbuild_fd_open_write("build/FS.c.move_test.src");
						cbuild_cmd_sync_redirect(
								file_writer,
								(cbuild_cmd_fd_t){ .fdstdin  = CBUILD_INVALID_FD,
		                               .fdstdout = pattern_fd,
		                               .fdstderr = CBUILD_INVALID_FD });
						cbuild_fd_close(pattern_fd);
						// Do testing
						bool ret = cbuild_file_move("build/FS.c.move_test.src",
		                                    "build/FS.c.move_test.dst");
						TEST_ASSERT_EQ(ret, true, "Function returened error%s", "");
						bool ch1 = cbuild_file_check("build/FS.c.move_test.dst");
						TEST_ASSERT_EQ(ch1, true, "Move distination does not exist%s", "");
						bool ch2 = cbuild_file_check("build/FS.c.move_test.src");
						TEST_ASSERT_EQ(ch2, false, "Move source was not removed%s", "");
					},
					"File move");
			TEST_CASE(
					{
						// Create test data
						cbuild_cmd_t file_writer = cbuild_cmd;
						cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
						cbuild_fd_t pattern_fd =
								cbuild_fd_open_write("build/FS.c.rename_test.src");
						cbuild_cmd_sync_redirect(
								file_writer,
								(cbuild_cmd_fd_t){ .fdstdin  = CBUILD_INVALID_FD,
		                               .fdstdout = pattern_fd,
		                               .fdstderr = CBUILD_INVALID_FD });
						cbuild_fd_close(pattern_fd);
						// Do testing
						bool ret = cbuild_file_rename("build/FS.c.rename_test.src",
		                                      "build/FS.c.rename_test.dst");
						TEST_ASSERT_EQ(ret, true, "Function returened error%s", "");
						bool ch1 = cbuild_file_check("build/FS.c.rename_test.dst");
						TEST_ASSERT_EQ(ch1, true, "Renamed file does not exist%s", "");
						bool ch2 = cbuild_file_check("build/FS.c.rename_test.src");
						TEST_ASSERT_EQ(ch2, false, "Source file exists%s", "");
					},
					"File rename");
			TEST_CASE(
					{
						// Create test data
						cbuild_cmd_t file_writer = cbuild_cmd;
						cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
						cbuild_fd_t pattern_fd =
								cbuild_fd_open_write("build/FS.c.remove_test");
						cbuild_cmd_sync_redirect(
								file_writer,
								(cbuild_cmd_fd_t){ .fdstdin  = CBUILD_INVALID_FD,
		                               .fdstdout = pattern_fd,
		                               .fdstderr = CBUILD_INVALID_FD });
						cbuild_fd_close(pattern_fd);
						// Do testing
						bool ret = cbuild_file_remove("build/FS.c.remove_test");
						TEST_ASSERT_EQ(ret, true, "Function returened error%s", "");
						bool ch = cbuild_file_check("build/FS.c.remove_test");
						TEST_ASSERT_EQ(ch, false, "Removed file exist%s", "");
					},
					"File remove");
			TEST_CASE(
					{
						// Create test data
						cbuild_cmd_t dir_creator = cbuild_cmd;
						cbuild_cmd_append_many(&dir_creator, "mkdir",
		                               "build/FS.c.dirls_test/");
						cbuild_cmd_sync(dir_creator);
						cbuild_cmd_t file_writer = cbuild_cmd;
						cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
						cbuild_fd_t pattern_fd =
								cbuild_fd_open_write("build/FS.c.dirls_test/a");
						cbuild_cmd_sync_redirect(
								file_writer,
								(cbuild_cmd_fd_t){ .fdstdin  = CBUILD_INVALID_FD,
		                               .fdstdout = pattern_fd,
		                               .fdstderr = CBUILD_INVALID_FD });
						cbuild_fd_close(pattern_fd);
						file_writer.size = 0;
						cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
						pattern_fd = cbuild_fd_open_write("build/FS.c.dirls_test/b");
						cbuild_cmd_sync_redirect(
								file_writer,
								(cbuild_cmd_fd_t){ .fdstdin  = CBUILD_INVALID_FD,
		                               .fdstdout = pattern_fd,
		                               .fdstderr = CBUILD_INVALID_FD });
						cbuild_fd_close(pattern_fd);
						// Do testing
						cbuild_pathlist_t dirls = cbuild_pathlist;
						bool ret = cbuild_dir_list("build/FS.c.dirls_test/", &dirls);
						TEST_ASSERT_EQ(ret, true, "Function returened error%s", "");
						TEST_ASSERT_EQ(dirls.size, 2, "Expected 2 files but got %zu",
		                       dirls.size);
						bool contains_a = false;
						for (int i = 0; i < 4; i++) {
							contains_a = contains_a || (strcmp(dirls.data[i], "a") == 0);
						}
						bool contains_b = false;
						for (int i = 0; i < 4; i++) {
							contains_b = contains_b || (strcmp(dirls.data[i], "b") == 0);
						}
						TEST_ASSERT_EQ(contains_a && contains_b, true,
		                       "Directory list wrong! %s %s",
		                       contains_a ? "Directory \"a\" found."
		                                  : "Directory \"a\" does not found.",
		                       contains_b ? "Directory \"b\" found."
		                                  : "Directory \"b\" does not found.");
						cbuild_pathlist_clear(&dirls);
					},
					"Directory list");
			TEST_CASE(
					{
						// Create test data
						cbuild_cmd_t dir_creator = cbuild_cmd;
						cbuild_cmd_append_many(&dir_creator, "mkdir",
		                               "build/FS.c.dir_check_test.true/");
						cbuild_cmd_sync(dir_creator);
						// Do testing
						bool ch1 = cbuild_dir_check("build/FS.c.dir_check_test.true/");
						TEST_ASSERT_EQ(
								ch1, true,
								"Wrong file detection, file exist but check returned flase%s",
								"");
						bool ch2 = cbuild_dir_check("build/FS.c.dir_check_test.false/");
						TEST_ASSERT_EQ(ch2, false,
		                       "Wrong file detection, file does not exist but "
		                       "check returned true%s",
		                       "");
					},
					"Directory checking");
			TEST_CASE(
					{
						bool ret = cbuild_dir_create("build/FS.c.dir_create_test/");
						TEST_ASSERT_EQ(ret, true, "Function returened error%s", "");
						bool ch1 = cbuild_dir_check("build/FS.c.dir_create_test/");
						TEST_ASSERT_EQ(ch1, true, "Created directory does not exist%s", "");
					},
					"Directory creation");
			TEST_CASE(
					{
						// Create test data
						cbuild_dir_create("build/FS.c.dir_copy_test.src/");
						cbuild_cmd_t file_writer = cbuild_cmd;
						cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
						cbuild_fd_t pattern_fd =
								cbuild_fd_open_write("build/FS.c.dir_copy_test.src/a");
						cbuild_cmd_sync_redirect(
								file_writer,
								(cbuild_cmd_fd_t){ .fdstdin  = CBUILD_INVALID_FD,
		                               .fdstdout = pattern_fd,
		                               .fdstderr = CBUILD_INVALID_FD });
						cbuild_fd_close(pattern_fd);
						file_writer.size = 0;
						cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
						pattern_fd = cbuild_fd_open_write("build/FS.c.dir_copy_test.src/b");
						cbuild_cmd_sync_redirect(
								file_writer,
								(cbuild_cmd_fd_t){ .fdstdin  = CBUILD_INVALID_FD,
		                               .fdstdout = pattern_fd,
		                               .fdstderr = CBUILD_INVALID_FD });
						cbuild_fd_close(pattern_fd);
						cbuild_dir_create("build/FS.c.dir_copy_test.src/c/");
						file_writer.size = 0;
						cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
						pattern_fd =
								cbuild_fd_open_write("build/FS.c.dir_copy_test.src/c/d");
						cbuild_cmd_sync_redirect(
								file_writer,
								(cbuild_cmd_fd_t){ .fdstdin  = CBUILD_INVALID_FD,
		                               .fdstdout = pattern_fd,
		                               .fdstderr = CBUILD_INVALID_FD });
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
					},
					"Directory copy");
			TEST_CASE(
					{
						// Create test data
						cbuild_dir_create("build/FS.c.dir_remove_test/");
						cbuild_cmd_t file_writer = cbuild_cmd;
						cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
						cbuild_fd_t pattern_fd =
								cbuild_fd_open_write("build/FS.c.dir_remove_test/a");
						cbuild_cmd_sync_redirect(
								file_writer,
								(cbuild_cmd_fd_t){ .fdstdin  = CBUILD_INVALID_FD,
		                               .fdstdout = pattern_fd,
		                               .fdstderr = CBUILD_INVALID_FD });
						cbuild_fd_close(pattern_fd);
						file_writer.size = 0;
						cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
						pattern_fd = cbuild_fd_open_write("build/FS.c.dir_remove_test/b");
						cbuild_cmd_sync_redirect(
								file_writer,
								(cbuild_cmd_fd_t){ .fdstdin  = CBUILD_INVALID_FD,
		                               .fdstdout = pattern_fd,
		                               .fdstderr = CBUILD_INVALID_FD });
						cbuild_fd_close(pattern_fd);
						cbuild_dir_create("build/FS.c.dir_remove_test/c/");
						file_writer.size = 0;
						cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
						pattern_fd = cbuild_fd_open_write("build/FS.c.dir_remove_test/c/d");
						cbuild_cmd_sync_redirect(
								file_writer,
								(cbuild_cmd_fd_t){ .fdstdin  = CBUILD_INVALID_FD,
		                               .fdstdout = pattern_fd,
		                               .fdstderr = CBUILD_INVALID_FD });
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
					},
					"Directory remove");
			TEST_CASE(
					{
						// Create test data
						cbuild_cmd_t dir_creator = cbuild_cmd;
						cbuild_cmd_append_many(&dir_creator, "mkdir",
		                               "build/FS.c.path_type_check.dir/");
						cbuild_cmd_sync(dir_creator);
						cbuild_cmd_t file_writer = cbuild_cmd;
						cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
						cbuild_fd_t pattern_fd =
								cbuild_fd_open_write("build/FS.c.path_type_check.file");
						cbuild_cmd_sync_redirect(
								file_writer,
								(cbuild_cmd_fd_t){ .fdstdin  = CBUILD_INVALID_FD,
		                               .fdstdout = pattern_fd,
		                               .fdstderr = CBUILD_INVALID_FD });
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
						TEST_ASSERT_STREQ(e2, "", "Expected extension \"\" but got \"%s\"",
		                          e2);
						const char* p3 = "f.txt";
						const char* e3 = cbuild_path_ext(p3);
						TEST_ASSERT_STREQ(e3, "txt",
		                          "Expected extension \"txt\" but got \"%s\"", e3);
						const char* p4 = "f";
						const char* e4 = cbuild_path_ext(p4);
						TEST_ASSERT_STREQ(e4, "", "Expected extension \"\" but got \"%s\"",
		                          e4);
					},
					"Extracting extension from filepath");
			TEST_CASE(
					{
						const char* p1 = "some/dir/file.txt";
						const char* n1 = cbuild_path_name(p1);
						TEST_ASSERT_STREQ(n1, "file.txt",
		                          "Expected filename \"file.txt\" but got \"%s\"",
		                          n1);
						const char* p2 = "some/dir/another_dir/";
						const char* n2 = cbuild_path_name(p2);
						TEST_ASSERT_STREQ(
								n2, "another_dir/",
								"Expected dirname \"another_dir/\" but got \"%s\"", n2);
						const char* p3 = "a.c";
						const char* n3 = cbuild_path_name(p3);
						TEST_ASSERT_STREQ(n3, "a.c",
		                          "Expected filename \"a.c\" but got \"%s\"", n3);
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
					},
					"Extracting basename from filepath");
		},
		"Filesystem library")
