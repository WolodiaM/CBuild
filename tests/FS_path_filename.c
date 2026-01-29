int main(void) {
	const char* p1 = "some/dir/file.txt";
	const char* n1 = cbuild_path_name(p1);
	TEST_ASSERT_STREQ(n1, "file.txt", "Wrong filename extracted from \"%s\""
		TEST_EXPECT_MSG(s), p1, "file.txt", n1);
	const char* p2 = "some/dir/another_dir/";
	const char* n2 = cbuild_path_name(p2);
	TEST_ASSERT_STREQ(n2, "another_dir/", "Wrong filename extracted from \"%s\""
		TEST_EXPECT_MSG(s), p2, "another_dir/", n2);
	const char* p3 = "a.c";
	const char* n3 = cbuild_path_name(p3);
	TEST_ASSERT_STREQ(n3, "a.c", "Wrong filename extracted from \"%s\""
		TEST_EXPECT_MSG(s), p3, "a.c", n3);
	return 0;
}
