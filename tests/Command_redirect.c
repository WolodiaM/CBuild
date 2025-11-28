int main(void) {
	cbuild_cmd_t cmd = {0};
	const char* STR = "ABC";
	cbuild_cmd_append_many(&cmd, "printf", STR);
	const char* tmp_file = TEST_TEMP_FILE;
	TEST_ASSERT(cbuild_cmd_run(&cmd, .file_stdout = tmp_file),
		"Faled to run command.");
	cbuild_sb_t sb = {0};
	cbuild_file_read(tmp_file, &sb);
	TEST_ASSERT(cbuild_file_check(tmp_file),
		"Command did not create the expected redirect file: \"%s\".", tmp_file);
	TEST_ASSERT_EQ(3, sb.size, "Redirect file has wrong size"TEST_EXPECT_MSG(zu),
		(size_t)3, sb.size);
	TEST_ASSERT_MEMEQ(STR, sb.data, 3,
		"Redirect file content mismatch"TEST_EXPECT_RMSG(CBuildSBFmt),
		3, STR, CBuildSBArg(sb));
	cbuild_sb_clear(&sb);
	cbuild_cmd_clear(&cmd);
	return 0;
}
