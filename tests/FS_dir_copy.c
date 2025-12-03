int main(void) {
	const char* src = TEST_TEMP_FILE_EX("dir_src/");
	const char* dst = TEST_TEMP_FILE_EX("dir_dst");
	// Create test data
	cbuild_dir_create(src);
	cbuild_cmd_t file_writer = {0};
	cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
	cbuild_fd_t pattern_fd =
		cbuild_fd_open_write(cbuild_temp_sprintf("%s/a", src));
	cbuild_cmd_run(&file_writer, .fdstdout = &pattern_fd);
	cbuild_fd_close(pattern_fd);
	file_writer.size = 0;
	cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
	pattern_fd = cbuild_fd_open_write(cbuild_temp_sprintf("%s/b", src));
	cbuild_cmd_run(&file_writer, .fdstdout = &pattern_fd);
	cbuild_fd_close(pattern_fd);
	cbuild_dir_create(cbuild_temp_sprintf("%s/c/", src));
	file_writer.size = 0;
	cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
	pattern_fd = cbuild_fd_open_write(cbuild_temp_sprintf("%s/c/d", src));
	cbuild_cmd_run(&file_writer, .fdstdout = &pattern_fd);
	cbuild_fd_close(pattern_fd);
	// Do testing
	TEST_ASSERT(cbuild_dir_copy(src, dst), "cbuild_dir_copy returned error.");
	TEST_ASSERT(cbuild_file_check(cbuild_temp_sprintf("%s/a", dst)),
		"File \"a\" does not exist after copy");
	TEST_ASSERT(cbuild_file_check(cbuild_temp_sprintf("%s/b", dst)),
		"File \"b\" does not exist after copy");
	TEST_ASSERT(cbuild_dir_check(cbuild_temp_sprintf("%s/c", dst)),
		"Directory \"c\" does not exist after copy");
	TEST_ASSERT(cbuild_file_check(cbuild_temp_sprintf("%s/c/d", dst)),
		"File \"c/d\" does not exist after copy");
	cbuild_cmd_clear(&file_writer);
	return 0;
}
