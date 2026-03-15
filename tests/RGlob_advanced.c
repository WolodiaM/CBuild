int main(void) {
	cbuild_glob_t glob = {0};
	// Test regex-like behavior with character classes and *
	// Glob normally: [a-z] matches 1 char, but [a-z]* should match multiple.
	TEST_ASSERT(cbuild_glob_compile(&glob, "[a-z]*"),
		"Failed to compile '[a-z]*'.");
	TEST_ASSERT(cbuild_glob_match_single(&glob, "abc"),
		"Should match 'abc'.");
	TEST_ASSERT(cbuild_glob_match_single(&glob, "z"),
		"Should match 'z'.");
	TEST_NASSERT(cbuild_glob_match_single(&glob, "a1"),
		"Should not match 'a1'.");
	cbuild_glob_free(&glob);
	// Test escaping with \\#
	// \\. should match literal dot
	TEST_ASSERT(cbuild_glob_compile(&glob, "file\\#.txt"),
		"Failed to compile 'file\\.txt'.");
	TEST_ASSERT(cbuild_glob_match_single(&glob, "file.txt"),
		"Should match 'file.txt'.");
	TEST_NASSERT(cbuild_glob_match_single(&glob, "fileatxt"),
		"Should not match 'fileatxt'.");
	cbuild_glob_free(&glob);
	// Test capture groups via #( and #)
	// We want to capture the number in "test_123.c"
	TEST_ASSERT(cbuild_glob_compile(&glob, "test_#([0-9]*#).c"),
		"Failed to compile with captures.");
	TEST_ASSERT(cbuild_glob_match_single(&glob, "test_123.c"),
		"Should match 'test_123.c'.");
	TEST_ASSERT_EQ(glob.captures[1].rm_so, 5, "Invalid capture start"TEST_EXPECT_MSG(d),
		5, (int)glob.captures[1].rm_so);
	TEST_ASSERT_EQ(glob.captures[1].rm_eo, 8, "Invalid capture end"TEST_EXPECT_MSG(d),
		8, (int)glob.captures[1].rm_eo);
	cbuild_glob_free(&glob);
	// Test regex escaping of special characters.
	// ( ) { } ^ $ . | + should be escaped by default.
	TEST_ASSERT(cbuild_glob_compile(&glob, "(test).txt"),
		"Failed to compile '(test).txt'.");
	TEST_ASSERT(cbuild_glob_match_single(&glob, "(test).txt"),
		"Should match literal '(test).txt'.");
	TEST_NASSERT(cbuild_glob_match_single(&glob, "test.txt"),
		"Should not match 'test.txt'.");
	cbuild_glob_free(&glob);
	return 0;
}
