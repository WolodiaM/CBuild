int main(void) {
	// Create test data
	cbuild_cmd_t file_writer = {0};
	const char* tfile = TEST_TEMP_FILE;
	cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
	cbuild_fd_t pattern_fd = cbuild_fd_open_write(tfile);
	cbuild_cmd_run(&file_writer, .fdstdout = &pattern_fd);
	cbuild_fd_close(pattern_fd);
	// Do testing
	cbuild_sb_t sb  = {0};
	TEST_ASSERT(cbuild_file_read(tfile, &sb), "cbuild_file_read returned error.");
	cbuild_sb_append_null(&sb);
	TEST_ASSERT_STREQ(sb.data, "ABCD",
		"Content read from file does not match expected value"TEST_EXPECT_MSG(s),
		sb.data, "ABCD");
	cbuild_cmd_clear(&file_writer);
	cbuild_sb_clear(&sb);
	return 0;
}
