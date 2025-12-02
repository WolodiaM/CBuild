int main(void) {
	// Create test data
	cbuild_cmd_t file_writer = {0};
	const char* tfile = TEST_TEMP_FILE;
	cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
	cbuild_fd_t pattern_fd = cbuild_fd_open_write(tfile);
	cbuild_cmd_run(&file_writer, .fdstdout = &pattern_fd);
	cbuild_fd_close(pattern_fd);
	// Do testing
	TEST_ASSERT(cbuild_file_remove(tfile), "cbuild_file_remove returned error.");
	TEST_NASSERT(cbuild_file_check(tfile), "File exists after a remove.");
	cbuild_cmd_clear(&file_writer);
	return 0;
}
