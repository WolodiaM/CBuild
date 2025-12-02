int main(void) {
	// Create test data
	cbuild_cmd_t file_writer = {0};
	const char* tsrc = TEST_TEMP_FILE_EX("src");
	const char* tdst = TEST_TEMP_FILE_EX("dst");
	cbuild_cmd_append_many(&file_writer, "dd", "bs=1024", "count=64",
		"status=none", "if=/dev/urandom", cbuild_temp_sprintf("of=%s", tsrc));
	cbuild_cmd_run(&file_writer);
	// Do testing
	cbuild_sb_t f1  = {0};
	cbuild_sb_t f2  = {0};
	TEST_ASSERT(cbuild_file_copy(tsrc, tdst), "cbuild_file_copy returened error");
	cbuild_file_read(tsrc, &f1);
	cbuild_file_read(tdst, &f2);
	TEST_ASSERT_MEMEQ(f1.data, f2.data, 64 * 1024,
		"Content of large source and destination files do not match after copy"
		TEST_EXPECT_RMSG(CBuildSBFmt), CBuildSBArg(f1), CBuildSBArg(f2));
	cbuild_cmd_clear(&file_writer);
	cbuild_sb_clear(&f1);
	cbuild_sb_clear(&f2);
	return 0;
}
