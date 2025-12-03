int main(void) {
	const char* dir =	TEST_TEMP_FILE_EX("dir/");
	const char* file =	TEST_TEMP_FILE_EX("file");
	const char* none = TEST_TEMP_FILE_EX("none");
	// Create test data
	cbuild_cmd_t dir_creator = {0};
	cbuild_cmd_append_many(&dir_creator, "mkdir", "-p", dir);
	cbuild_cmd_run(&dir_creator);
	cbuild_cmd_t file_writer = {0};
	cbuild_cmd_append_many(&file_writer, "printf", "ABCD");
	cbuild_fd_t pattern_fd = cbuild_fd_open_write(file);
	cbuild_cmd_run(&file_writer, .fdstdout = &pattern_fd);
	cbuild_fd_close(pattern_fd);
	// Do testing
	TEST_ASSERT_EQ(cbuild_path_filetype(dir), CBUILD_FTYPE_DIRECTORY,
		"Directory not detected properly");
	TEST_ASSERT_EQ(cbuild_path_filetype(file), CBUILD_FTYPE_REGULAR,
		"Regular file not detected properly");
	TEST_ASSERT_EQ(cbuild_path_filetype(none), CBUILD_FTYPE_MISSING,
		"Missing file should reporst as such");
	cbuild_cmd_clear(&dir_creator);
	cbuild_cmd_clear(&file_writer);
	return 0;
}
