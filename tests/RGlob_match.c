int main(void) {
	cbuild_glob_t glob = {0};
	const char* list[] = {
		"test_a.c",
		"test_b.h",
		"test_c.c",
		"other.c"
	};
	TEST_ASSERT(cbuild_glob_compile(&glob, "test_*.c"),
		"Failed to compile 'test_*.c'.");
	bool found = cbuild_glob_match(&glob, list, cbuild_arr_len(list));
	TEST_ASSERT(found, "Should find matches.");
	TEST_ASSERT_EQ(glob.size, 2, "Invalid match count"TEST_EXPECT_MSG(zu),
		(size_t)2, glob.size);
	TEST_ASSERT_STREQ(glob.data[0].res, "test_a.c", "Invalid match 0"TEST_EXPECT_MSG(s),
		"test_a.c", glob.data[0].res);
	TEST_ASSERT_EQ(glob.data[0].res_idx, 0, "Invalid index 0"TEST_EXPECT_MSG(zu),
		(size_t)0, glob.data[0].res_idx);
	TEST_ASSERT_STREQ(glob.data[1].res, "test_c.c", "Invalid match 1"TEST_EXPECT_MSG(s),
		"test_c.c", glob.data[1].res);
	TEST_ASSERT_EQ(glob.data[1].res_idx, 2, "Invalid index 1"TEST_EXPECT_MSG(zu),
		(size_t)2, glob.data[1].res_idx);
	cbuild_glob_free(&glob);
	return 0;
}
