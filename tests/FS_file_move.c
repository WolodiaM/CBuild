int main(void) {
	// Create test data
	cbuild_cmd_t file_writer = {0};
	const char* tsrc = TEST_TEMP_FILE_EX("src");
	const char* tdst = TEST_TEMP_FILE_EX("dst");
	cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
	cbuild_fd_t pattern_fd = cbuild_fd_open_write(tsrc);
	cbuild_cmd_run(&file_writer, .fdstdout = &pattern_fd);
	cbuild_fd_close(pattern_fd);
	// Do testing
	TEST_ASSERT(cbuild_file_move(tsrc, tdst), "cbuild_file_move returned error.");
	TEST_NASSERT(cbuild_file_check(tsrc),
		"Source file still exists after a move.");
	TEST_ASSERT(cbuild_file_check(tdst),
		"Destination file does not exist after a move.");
	cbuild_sb_t content = {0};
	cbuild_file_read(tdst, &content);
	cbuild_sb_append_null(&content);
	TEST_ASSERT_STREQ(content.data, "ABCD",
		"Invalid file content after a move"TEST_EXPECT_MSG(s), "ABCD", content.data);
	cbuild_sb_clear(&content);
	cbuild_cmd_clear(&file_writer);
	return 0;
}
