int main(void) {
	const char* p1 = "some/dir/file.txt";
	const char* n1 = cbuild_path_base(p1);
	TEST_ASSERT_STREQ(n1, "some/dir/", "Wrong basename extracted from \"%s\""
		TEST_EXPECT_MSG(s), p1, "some/dir/", n1);
	const char* p2 = "some/dir/another_dir/";
	const char* n2 = cbuild_path_base(p2);
	TEST_ASSERT_STREQ(n2, "some/dir/", "Wrong basename extracted from \"%s\""
		TEST_EXPECT_MSG(s), p2, "some/dir/", n2);
	const char* p3 = "a.c";
	const char* n3 = cbuild_path_base(p3);
	TEST_ASSERT_STREQ(n3, "", "Wrong basename extracted from \"%s\""
		TEST_EXPECT_MSG(s), p3, "", n3);
	return 0;
}
