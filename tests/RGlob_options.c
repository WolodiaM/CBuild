int main(void) {
	cbuild_glob_t glob = {0};
	// Default match (wrapped in ^ and $)
	TEST_ASSERT(cbuild_glob_compile(&glob, "test"),
		"Failed to compile 'test'.");
	TEST_ASSERT(cbuild_glob_match_single(&glob, "test"),
		"Should match 'test'.");
	TEST_NASSERT(cbuild_glob_match_single(&glob, "atest"),
		"Should not match 'atest'.");
	TEST_NASSERT(cbuild_glob_match_single(&glob, "testb"),
		"Should not match 'testb'.");
	cbuild_glob_free(&glob);
	// Partial match (no ^ and $)
	TEST_ASSERT(cbuild_glob_compile(&glob, "test", .partial_match=true),
		"Failed to compile 'test' (partial).");
	TEST_ASSERT(cbuild_glob_match_single(&glob, "test"),
		"Should match 'test'.");
	TEST_ASSERT(cbuild_glob_match_single(&glob, "atest"),
		"Should match 'atest' (partial).");
	TEST_ASSERT(cbuild_glob_match_single(&glob, "testb"),
		"Should match 'testb' (partial).");
	TEST_ASSERT(cbuild_glob_match_single(&glob, "atestb"),
		"Should match 'atestb' (partial).");
	cbuild_glob_free(&glob);
	return 0;
}
