int main(void) {
	// Create test data
	cbuild_cmd_t file_writer = {0};
	const char* ttfile = TEST_TEMP_FILE_EX("true");
	cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
	cbuild_fd_t pattern_fd = cbuild_fd_open_write(ttfile);
	cbuild_cmd_run(&file_writer, .fdstdout = &pattern_fd);
	cbuild_fd_close(pattern_fd);
	// Do testing
	TEST_ASSERT(cbuild_file_check(ttfile),
		"cbuild_file_check failed to detect valid file.");
	const char* tffile = TEST_TEMP_FILE_EX("false");
	TEST_NASSERT(cbuild_file_check(tffile),
		"cbuild_file_check erroneously detected invalid file.");
	cbuild_cmd_clear(&file_writer);
	return 0;
}
