int main(void) {
	const char* bdir = TEST_TEMP_FILE_EX("dir_create/");
	const char* nbdir = TEST_TEMP_FILE_EX("dir_create_no_base/dir/");
	TEST_ASSERT(cbuild_dir_create(bdir), "[base exists] Error during dir creation");
	TEST_ASSERT(cbuild_dir_check(bdir), "[base exists] Dir has not been created.");
	TEST_ASSERT(cbuild_dir_create(nbdir), "[no base] Error during dir creation");
	TEST_ASSERT(cbuild_dir_check(nbdir), "[no base] Dir has not been created.");
	return 0;
}
