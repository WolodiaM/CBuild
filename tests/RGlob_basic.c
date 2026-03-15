int main(void) {
	cbuild_glob_t glob = {0};
	// Test *
	TEST_ASSERT(cbuild_glob_compile(&glob, "test_*.c"),
		"Failed to compile glob 'test_*.c'.");
	TEST_ASSERT(cbuild_glob_match_single(&glob, "test_alloc.c"),
		"Should match 'test_alloc.c'.");
	TEST_ASSERT(cbuild_glob_match_single(&glob, "test_.c"),
		"Should match 'test_.c'.");
	TEST_NASSERT(cbuild_glob_match_single(&glob, "test_alloc.h"),
		"Should not match 'test_alloc.h'.");
	TEST_NASSERT(cbuild_glob_match_single(&glob, "atest_alloc.c"),
		"Should not match 'atest_alloc.c' (prefix).");
	cbuild_glob_free(&glob);
	// Test ?
	TEST_ASSERT(cbuild_glob_compile(&glob, "test_?.c"),
		"Failed to compile glob 'test_?.c'.");
	TEST_ASSERT(cbuild_glob_match_single(&glob, "test_a.c"),
		"Should match 'test_a.c'.");
	TEST_NASSERT(cbuild_glob_match_single(&glob, "test_aa.c"),
		"Should not match 'test_aa.c'.");
	cbuild_glob_free(&glob);
	// Test character class
	TEST_ASSERT(cbuild_glob_compile(&glob, "file[0-9].txt"),
		"Failed to compile glob 'file[0-9].txt'.");
	TEST_ASSERT(cbuild_glob_match_single(&glob, "file1.txt"),
		"Should match 'file1.txt'.");
	TEST_ASSERT(cbuild_glob_match_single(&glob, "file9.txt"),
		"Should match 'file9.txt'.");
	TEST_NASSERT(cbuild_glob_match_single(&glob, "filea.txt"),
		"Should not match 'filea.txt'.");
	cbuild_glob_free(&glob);
	// Test inverted character class
	TEST_ASSERT(cbuild_glob_compile(&glob, "file[!0-9].txt"),
		"Failed to compile glob 'file[!0-9].txt'.");
	TEST_ASSERT(cbuild_glob_match_single(&glob, "filea.txt"),
		"Should match 'filea.txt'.");
	TEST_NASSERT(cbuild_glob_match_single(&glob, "file1.txt"),
		"Should not match 'file1.txt'.");
	cbuild_glob_free(&glob);
	
	return 0;
}
