int main(void) {
	// Create test data
	cbuild_cmd_t dir_creator = {0};
	const char* dir = TEST_TEMP_FILE_EX("dirls/");
	cbuild_cmd_append_many(&dir_creator, "mkdir", "-p", dir);
	cbuild_cmd_run(&dir_creator);
	cbuild_cmd_t file_writer = {0};
	cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
	cbuild_fd_t pattern_fd =
		cbuild_fd_open_write(cbuild_temp_sprintf("%s/a", dir));
	cbuild_cmd_run(&file_writer, .fdstdout = &pattern_fd);
	cbuild_fd_close(pattern_fd);
	file_writer.size = 0;
	cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
	pattern_fd = cbuild_fd_open_write(cbuild_temp_sprintf("%s/b", dir));
	cbuild_cmd_run(&file_writer, .fdstdout = &pattern_fd);
	cbuild_fd_close(pattern_fd);
	// Do testing
	cbuild_pathlist_t dirls = {0};
	bool ret = cbuild_dir_list(dir, &dirls);
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
	return 0;
}
