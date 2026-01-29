int main(void) {
	const char* p1 = "some/dir/file.txt";
	const char* e1 = cbuild_path_ext(p1);
	TEST_ASSERT_STREQ(e1, "txt", "Wrong extension extracted from \"%s\""
		TEST_EXPECT_MSG(s), p1, "txt", e1);
	const char* p2 = "some/dir/d";
	const char* e2 = cbuild_path_ext(p2);
	TEST_ASSERT_STREQ(e2, "", "Wrong extension extracted from \"%s\""
		TEST_EXPECT_MSG(s), p2, "", e2);
	const char* p3 = "f.txt";
	const char* e3 = cbuild_path_ext(p3);
	TEST_ASSERT_STREQ(e3, "txt", "Wrong extension extracted from \"%s\""
		TEST_EXPECT_MSG(s), p3, "txt", e3);
	const char* p4 = "f";
	const char* e4 = cbuild_path_ext(p4);
	TEST_ASSERT_STREQ(e4, "", "Wrong extension extracted from \"%s\""
		TEST_EXPECT_MSG(s), p4, "", e4);
	const char* p5 = "file.tar.gz";
	const char* e5 = cbuild_path_ext(p4, .dot = 2);
	TEST_ASSERT_STREQ(e5, "", "Wrong extension extracted from \"%s\""
		TEST_EXPECT_MSG(s), p5, "tar.gz", e5);
	return 0;
}
