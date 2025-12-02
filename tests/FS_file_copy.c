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
	cbuild_sb_t f1  = {0};
	cbuild_sb_t f2  = {0};
	TEST_ASSERT(cbuild_file_copy(tsrc, tdst), "cbuild_file_copy returned error.");
	cbuild_file_read(tsrc, &f1);
	cbuild_file_read(tdst, &f2);
	cbuild_sb_append_null(&f1);
	cbuild_sb_append_null(&f2);
	TEST_ASSERT_STREQ(f1.data, f2.data,
		"Content of source and destination files do not match after copy"TEST_EXPECT_MSG(s), f1.data, f2.data);
	cbuild_cmd_clear(&file_writer);
	cbuild_sb_clear(&f1);
	cbuild_sb_clear(&f2);
	return 0;
}
