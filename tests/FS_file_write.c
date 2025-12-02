int main(void) {
	cbuild_sb_t sb = {0};
	const char* tfile = TEST_TEMP_FILE;
	cbuild_sb_append_cstr(&sb, "ABCD");
	TEST_ASSERT(cbuild_file_write(tfile, &sb), "cbuild_file_write returned error.");
	cbuild_sb_t check = {0};
	TEST_ASSERT(cbuild_file_read(tfile, &check),
		"cbuild_file_read returned error");
	cbuild_sb_append_null(&sb);
	cbuild_sb_append_null(&check);
	TEST_ASSERT_STREQ(check.data, sb.data,
		"Wrong value read from a file"TEST_EXPECT_MSG(s), check.data, sb.data);
	cbuild_sb_clear(&sb);
	cbuild_sb_clear(&check);
	return 0;
}
