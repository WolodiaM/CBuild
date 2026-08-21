int main(void) {
	cbuild_glob_t glob = {0};
	// Single asterisk
	TEST_ASSERT(cbuild_glob_compile(&glob, "src/*/*.c"),
		"Failed to compile glob 'src/*/*.c'.");
	TEST_ASSERT(cbuild_glob_match_single(&glob, "src/foo/lib.c"),
		"Should match 'src/foo/lib.c'.");
	TEST_ASSERT(cbuild_glob_match_single(&glob, "src/foo/.c"),
		"Should match 'src/foo/.c'.");
	TEST_NASSERT(cbuild_glob_match_single(&glob, "src/foo/lib.h"),
		"Should not match 'src/foo/lib.h'.");
	TEST_NASSERT(cbuild_glob_match_single(&glob, "src/main.c"),
		"Should not match 'src/main.c' (no dir).");
	cbuild_glob_free(&glob);
	// Double asterisk
	TEST_ASSERT(cbuild_glob_compile(&glob, "src/**/*.c"),
		"Failed to compile glob 'src/**/*.c'.");
	TEST_ASSERT(cbuild_glob_match_single(&glob, "src/foo/lib.c"),
		"Should match 'src/foo/lib.c'.");
	TEST_ASSERT(cbuild_glob_match_single(&glob, "src/foo/.c"),
		"Should match 'src/foo/.c'.");
	TEST_NASSERT(cbuild_glob_match_single(&glob, "src/foo/lib.h"),
		"Should not match 'src/foo/lib.h'.");
	TEST_ASSERT(cbuild_glob_match_single(&glob, "src/main.c"),
		"Should match 'src/main.c' (no dir).");
	cbuild_glob_free(&glob);
	// Double asterisk no '/'
	TEST_ASSERT(cbuild_glob_compile(&glob, "src/**.c"),
		"Failed to compile glob 'src/**.c'.");
	TEST_NASSERT(cbuild_glob_match_single(&glob, "src/foo/lib.c"),
		"Should not 'src/foo/lib.c'.");
	TEST_ASSERT(cbuild_glob_match_single(&glob, "src/main.c"),
		"Should match 'src/main.c' (no dir).");
	cbuild_glob_free(&glob);
	return 0;
}
