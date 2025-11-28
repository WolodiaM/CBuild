int main(void) {
	// We use redirects here to test command
	cbuild_cmd_t cmd = {0};
	const char * TEST_STR = "ABCD";
	cbuild_cmd_append_many(&cmd, "printf", TEST_STR);
	cbuild_fd_t rd, wr;
	TEST_ASSERT(cbuild_fd_open_pipe(&rd, &wr), "Can't open pipe.");
	TEST_ASSERT(cbuild_cmd_run(&cmd, .fdstdout = &wr),
		"Error during function call.");
	cbuild_fd_close(wr);
	char str[1024];
	ssize_t num = cbuild_fd_read(rd, str, 1024);
	TEST_ASSERT_EQ(num, 4,
		"Wrong number of bytes read from sync stdout pipe"
		TEST_EXPECT_RMSG("%"PRId64), 4, num);
	TEST_ASSERT_MEMEQ(str, TEST_STR, 4,
		"Wrong string read from sync stdout pipe" TEST_EXPECT_RMSG(CBuildSVFmt),
		cbuild_sv_from_cstr(TEST_STR), cbuild_sv_from_parts(str, (size_t)num));
	cbuild_cmd_clear(&cmd);
	cbuild_fd_close(rd);
	return 0;
}
