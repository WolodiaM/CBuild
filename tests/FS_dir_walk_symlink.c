int as = 0;
int bs = 0;
int cs = 0;
int ds = 0;
bool walker(cbuild_dir_walk_func_args_t args) {
	cbuild_sv_t path = cbuild_sv_from_cstr(args.path);
	if (cbuild_sv_suffix(path, cbuild_sv_from_lit("/a"))) as++;
	if (cbuild_sv_suffix(path, cbuild_sv_from_lit("/b"))) bs++;
	if (cbuild_sv_suffix(path, cbuild_sv_from_lit("/c"))) cs++;
	if (cbuild_sv_suffix(path, cbuild_sv_from_lit("/d"))) ds++;
	return true;
}
int main(void) {
	// Create test data
	cbuild_cmd_t dir_creator = {0};
	const char* dir = TEST_TEMP_FILE_EX("dirls/");
	cbuild_da_append_many(&dir_creator, "mkdir", "-p", dir);
	cbuild_cmd_run(&dir_creator);
	cbuild_cmd_t file_writer = {0};
	cbuild_da_append_many(&file_writer, "printf", "ABCD");
	cbuild_cmd_run(&file_writer, .no_reset = true,
		.file_stdout = cbuild_temp_sprintf("%s/a", dir));
	cbuild_cmd_run(&file_writer, .no_reset = true,
		.file_stdout = cbuild_temp_sprintf("%s/b", dir));
	cbuild_dir_create(cbuild_temp_sprintf("%s/c", dir));
	cbuild_cmd_run(&file_writer, .no_reset = true, 
		.file_stdout = cbuild_temp_sprintf("%s/c/a", dir));
	cbuild_cmd_run(&file_writer, .no_reset = true,
		.file_stdout = cbuild_temp_sprintf("%s/c/b", dir));
	symlink("c", cbuild_temp_sprintf("%s/d", dir));
	cbuild_da_clear(&dir_creator);
	cbuild_da_clear(&file_writer);
	// Do testing
	TEST_ASSERT(cbuild_dir_walk(dir, walker),
		"Failed to walk directory tree.");
	TEST_ASSERT_EQ(as, 3, "Wrong count of files named 'a'"
		TEST_EXPECT_MSG(d), 3, as);
	TEST_ASSERT_EQ(bs, 3, "Wrong count of files named 'b'"
		TEST_EXPECT_MSG(d), 3, bs);
	TEST_ASSERT_EQ(cs, 1, "Wrong count of files named 'c'"
		TEST_EXPECT_MSG(d), 1, cs);
	TEST_ASSERT_EQ(ds, 1, "Wrong count of files named 'd'"
		TEST_EXPECT_MSG(d), 1, ds);
	return 0;
}
