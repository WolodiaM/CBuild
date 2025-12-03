int main(void) {
	const char* dir = TEST_TEMP_FILE_EX("dir_remove/");
	// Create test data
	cbuild_dir_create(dir);
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
	cbuild_dir_create(cbuild_temp_sprintf("%s/c/", dir));
	file_writer.size = 0;
	cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
	pattern_fd = cbuild_fd_open_write(cbuild_temp_sprintf("%s/c/d", dir));
	cbuild_cmd_run(&file_writer, .fdstdout = &pattern_fd);
	cbuild_fd_close(pattern_fd);
	// Do testing
	TEST_ASSERT(cbuild_dir_remove(dir), "cbuild_dir_remove returned error.");
	TEST_NASSERT(cbuild_file_check(cbuild_temp_sprintf("%s/a", dir)),
		"File \"a\" exist after copy");
	TEST_NASSERT(cbuild_file_check(cbuild_temp_sprintf("%s/b", dir)),
		"File \"b\" exist after copy");
	TEST_NASSERT(cbuild_dir_check(cbuild_temp_sprintf("%s/c", dir)),
		"Directory \"c\" exist after copy");
	TEST_NASSERT(cbuild_file_check(cbuild_temp_sprintf("%s/c/d", dir)),
		"File \"c/d\" exist after copy");
	cbuild_cmd_clear(&file_writer);
	return 0;
}
