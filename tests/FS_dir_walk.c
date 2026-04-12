int as = 0;
int bs = 0;
int cs = 0;
bool walker(cbuild_dir_walk_func_args_t args) {
	cbuild_sv_t path = cbuild_sv_from_cstr(args.path);
	if (cbuild_sv_suffix(path, cbuild_sv_from_lit("/a"))) as++;
	if (cbuild_sv_suffix(path, cbuild_sv_from_lit("/b"))) bs++;
	if (cbuild_sv_suffix(path, cbuild_sv_from_lit("/c"))) cs++;
	return true;
}
bool walker_small(cbuild_dir_walk_func_args_t args) {
	cbuild_sv_t path = cbuild_sv_from_cstr(args.path);
	if (cbuild_sv_suffix(path, cbuild_sv_from_lit("/a"))) as++;
	if (cbuild_sv_suffix(path, cbuild_sv_from_lit("/b"))) bs++;
	if (cbuild_sv_suffix(path, cbuild_sv_from_lit("/c"))) cs++;
	if (args.level > 0) *args.result = CBUILD_DIR_WALK_NO_ENTER;
	return true;
}
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
	cbuild_cmd_run(&file_writer, .fdstdout = &pattern_fd, .no_reset = true);
	cbuild_fd_close(pattern_fd);
	pattern_fd = cbuild_fd_open_write(cbuild_temp_sprintf("%s/b", dir));
	cbuild_cmd_run(&file_writer, .fdstdout = &pattern_fd, .no_reset = true);
	cbuild_fd_close(pattern_fd);
	cbuild_dir_create(cbuild_temp_sprintf("%s/c", dir));
	cbuild_cmd_run(&file_writer, .no_reset = true, 
		.file_stdout = cbuild_temp_sprintf("%s/c/a", dir));
	cbuild_cmd_run(&file_writer, .no_reset = true,
		.file_stdout = cbuild_temp_sprintf("%s/c/b", dir));
	cbuild_cmd_clear(&dir_creator);
	cbuild_cmd_clear(&file_writer);
	// Do testing
	TEST_ASSERT(cbuild_dir_walk(dir, walker),
		"Failed to walk directory tree.");
	TEST_ASSERT_EQ(as, 2, "Wrong count of files named 'a'"
		TEST_EXPECT_MSG(d), 2, as);
	TEST_ASSERT_EQ(bs, 2, "Wrong count of files named 'b'"
		TEST_EXPECT_MSG(d), 2, bs);
	TEST_ASSERT_EQ(cs, 1, "Wrong count of files named 'c'"
		TEST_EXPECT_MSG(d), 1, cs);
	as = 0; bs = 0; cs = 0;
	TEST_ASSERT(cbuild_dir_walk(dir, walker_small),
		"Failed to walk directory.");
	TEST_ASSERT_EQ(as, 1, "Wrong count of files named 'a'"
		TEST_EXPECT_MSG(d), 1, as);
	TEST_ASSERT_EQ(bs, 1, "Wrong count of files named 'b'"
		TEST_EXPECT_MSG(d), 1, bs);
	TEST_ASSERT_EQ(cs, 1, "Wrong count of files named 'c'"
		TEST_EXPECT_MSG(d), 1, cs);
	return 0;
}
