int main(void) {
	// We use redirects here to test command
	cbuild_cmd_t cmd = {0};
	const char * TEST_STR = "231";
	cbuild_proclist_t procs = {0};
	cbuild_fd_t rd, wr;
	TEST_ASSERT(cbuild_fd_open_pipe(&rd, &wr), "Can't open pipe.");
	cbuild_cmd_append_many(&cmd, "sh", "-c", "sleep 3; printf 1");
	TEST_ASSERT(cbuild_cmd_run(&cmd, .async_threads = 2, .procs = &procs,
			.fdstdout = &wr, .no_reset = true),
		"Failed to run first command!");
	cmd.size = 0;
	cbuild_cmd_append_many(&cmd, "sh", "-c", "sleep 1; printf 2");
	TEST_ASSERT(cbuild_cmd_run(&cmd, .async_threads = 2, .procs = &procs,
			.fdstdout = &wr, .no_reset = true),
		"Failed to run first command!");
	cmd.size = 0;
	cbuild_cmd_append_many(&cmd, "sh", "-c", "printf 3");
	TEST_ASSERT(cbuild_cmd_run(&cmd, .async_threads = 2, .procs = &procs,
			.fdstdout = &wr, .no_reset = true),
		"Failed to run first command!");
	cmd.size = 0;
	cbuild_procs_wait(procs);
	cbuild_fd_close(wr);
	char str[1024];
	ssize_t num = cbuild_fd_read(rd, str, 1024);
	TEST_ASSERT_EQ(num, 3,
		"Wrong number of bytes read from a pipe" TEST_EXPECT_RMSG("%"PRId64), 3, num);
	TEST_ASSERT_MEMEQ(
		str, TEST_STR, 3,
		"Wrong string read from a pipe" TEST_EXPECT_MSG(.*s), 3, TEST_STR, 1024, str);
	cbuild_fd_close(rd);
	cbuild_cmd_clear(&cmd);
	cbuild_proclist_clear(&procs);
	return 0;
}
