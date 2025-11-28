int main(void) {
	// Prepare test data
	const char* tf1 = TEST_TEMP_FILE_EX("mtime");
	const char* tf2 = TEST_TEMP_FILE_EX("mtime");
	const char* tf3 = TEST_TEMP_FILE_EX("mtime");
	const char* tf4 = TEST_TEMP_FILE_EX("mtime");
	const char* tf5 = TEST_TEMP_FILE_EX("mtime");
	cbuild_fd_close(cbuild_fd_open_write(tf1));
	sleep(2);
	cbuild_fd_close(cbuild_fd_open_write(tf2));
	sleep(2);
	cbuild_fd_close(cbuild_fd_open_write(tf3));
	// Run tests
	const char* fs1[2] = {tf2, tf3};
	int r1 = cbuild_compare_mtime_many(tf1, fs1, 2);
	TEST_ASSERT_EQ(r1, 2,
		"cbuild_compare_mtime_many: Wrong result when output is older than two inputs"
		TEST_EXPECT_MSG(d),
		2, r1);
	const char* fs2[2] = {tf1, tf2};
	int r2 = cbuild_compare_mtime_many(tf3, fs2, 2);
	TEST_ASSERT_EQ(r2, 0,
		"cbuild_compare_mtime_many: Wrong result when output is newer than all inputs"
		TEST_EXPECT_MSG(d),
		0, r2);
	const char* fs3[2] = {tf2, tf3};
	int r3 = cbuild_compare_mtime_many(tf4, fs3, 2);
	TEST_ASSERT_EQ(r3, 2,
		"cbuild_compare_mtime_many: Wrong result when output does not exist"
		TEST_EXPECT_MSG(d), 2, r3);
	const char* fs4[2] = {tf2, tf4};
	int r4 = cbuild_compare_mtime_many(tf3, fs4, 2);
	TEST_ASSERT_EQ(r4, -1,
		"cbuild_compare_mtime_many: Wrong result when one input does not exist"
		TEST_EXPECT_MSG(d), -1, r4);
	const char* fs5[3] = {tf2, tf3, tf5};
	int r5 = cbuild_compare_mtime_many(tf4, fs5, 3);
	TEST_ASSERT_EQ(r5, -1,
		"cbuild_compare_mtime_many: Wrong result when one input and output do not exist"
		TEST_EXPECT_MSG(d), -1, r5);
	return 0;
}
