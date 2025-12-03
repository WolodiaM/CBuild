int main(void) {
	// Create test data
	cbuild_cmd_t dir_creator = {0};
	const char* tdir = TEST_TEMP_FILE_EX("dir_check.true/");
	const char* fdir = TEST_TEMP_FILE_EX("dir_check.false/");
	cbuild_cmd_append_many(&dir_creator, "mkdir", "-p", tdir);
	cbuild_cmd_run(&dir_creator);
	// Do testing
	TEST_ASSERT(cbuild_dir_check(tdir), "Existing directory marked as invalid.");
	TEST_NASSERT(cbuild_dir_check(fdir), "Missing directory marked as valid.");
	cbuild_cmd_clear(&dir_creator);
	return 0;
}
