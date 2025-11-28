int main(void) {
	// Prepare test data
	const char* tf1 = TEST_TEMP_FILE_EX("mtime");
	const char* tf2 = TEST_TEMP_FILE_EX("mtime");
	const char* tf3 = TEST_TEMP_FILE_EX("mtime");
	cbuild_fd_close(cbuild_fd_open_write(tf1));
	sleep(2);
	cbuild_fd_close(cbuild_fd_open_write(tf2));
	// Run tests
	int r1 = cbuild_compare_mtime(tf1, tf2);
	TEST_ASSERT_EQ(r1, 1,
		"cbuild_compare_mtime: Wrong result when output is older"
		TEST_EXPECT_MSG(d), 1, r1);
	int r2 = cbuild_compare_mtime(tf2, tf1);
	TEST_ASSERT_EQ(r2, 0,
		"cbuild_compare_mtime: Wrong result when output is newer"
		TEST_EXPECT_MSG(d), 0, r2);
	int r3 = cbuild_compare_mtime(tf3, tf1);
	TEST_ASSERT_EQ(r3, 1,
		"cbuild_compare_mtime: Wrong result when output does not exist"
		TEST_EXPECT_MSG(d), 1, r3);
	int r4 = cbuild_compare_mtime(tf2, tf3);
	TEST_ASSERT_EQ(r4, -1,
		"cbuild_compare_mtime: Wrong result when input does not exist"
		TEST_EXPECT_MSG(d), -1, r4);
	int r5 = cbuild_compare_mtime(tf3, tf3);
	TEST_ASSERT_EQ(r5, -1,
		"cbuild_compare_mtime: Wrong result when both input and output do not exist"
		TEST_EXPECT_MSG(d), -1, r5);
	return 0;
}
